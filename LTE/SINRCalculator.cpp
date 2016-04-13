#include "commondef.h"
#include <math.h>
#include <map>
#include <boost/foreach.hpp>
#include "SINRCalculator.h"
#include "DLIocCalculator.h"
#include "DLIorCalculator.h"
#include "System.h"
#include "Fading.h"
#include "Pathloss.h"
#include "Map.h"

namespace LTESim
{
    SINRCalculator SINRCalculator::ms_dlCalculator;
	SINRCalculator::SINRCalculator(void)
	{
	}

	SINRCalculator::~SINRCalculator(void)
	{
	}
#pragma region 获取实例的方法
    void SINRCalculator::InitDLCalculator( shared_ptr<IorCalculator> pIorCalc, shared_ptr<IocCalculator> pIocCalc )
    {
        ms_dlCalculator.m_pIorCalc = pIorCalc;
        ms_dlCalculator.m_pIocCalc = pIocCalc;
    }
	SINRCalculator& SINRCalculator::GetDLCalculator()
	{
		return ms_dlCalculator; 
	}
#pragma endregion 获取实例的方法

	//主方法
	void SINRCalculator::CalculateSINRofChannel( const shared_ptr<Channel> pRxChannel, int iframe, int islot ) const
	{
		//shared_ptr<NodeB> pNodeb = pRxChannel->GetActiveNodeB();
		shared_ptr<ILTEUE> pUE = pRxChannel->GetActiveUE();
		Channel::PhyResourceList RBList = pRxChannel->GetPhyResourceList();

        shared_ptr<NodeB> pNodeB = pRxChannel->GetActiveNodeB();
        shared_ptr<Antenna> pTxAntenna = pNodeB->GetTxAntenna();

		//计算每个当前时隙RB的SINR
        size_t pos = 0;
        size_t antennaPort = pTxAntenna->GetAntennaPortNum();
 
        //该表的维数为RB数*发射天线端口数
        vector< vector<double> > tempSINRList1(pRxChannel->GetRBNum(islot));

        //对于使用的当前时隙的每个RB循环
        BOOST_FOREACH(Channel::PhyResourceList::value_type& PhyRes, RBList)
		{
			if (PhyRes->GetSlot() == islot && PhyRes->GetFrames() == iframe)
			{
				double sinrofrb = CalcSINRofOneRB(*pNodeB, *pUE, *PhyRes);
				tempSINRList1[pos/antennaPort].push_back(sinrofrb);
                ++pos;
			}
		}

        //该表的维数为RB数
        vector<double> tempSINRList2;
        tempSINRList2.reserve(pRxChannel->GetRBNum(islot) / antennaPort);
        pTxAntenna->CombineSINR(tempSINRList1, tempSINRList2);	
		//更新channel的sinr列表
		pRxChannel->UpdateSINRListofSlot(tempSINRList2, iframe, islot);
	}
	
	double SINRCalculator::CalcSINRofOneRB( const NodeB & nodeb, const ILTEUE & ue,  const PhyResource &RB) const
	{
		const int iUERecAntennaPortNum = ue.GetRxAntenna()->GetAntennaPortNum();
		double SINROfOneRB = 0.0; 

		//因为接收使用的是MRC分集合并，所以直接将各接收天线SINR相加
		for(int RxAntenna = 0; RxAntenna < iUERecAntennaPortNum; ++RxAntenna)
		{
			//这一步应当和天线算法有关，之后需要独立出来
			SINROfOneRB += CalcSINRofOneRBofOneRecvAntenna(nodeb,ue,RB, RxAntenna);
		}
		return SINROfOneRB;
	}
	//计算每根接受天线上每个RB的SINR
    double SINRCalculator::CalcSINRofOneRBofOneRecvAntenna( const NodeB & nodeb, const ILTEUE & ue, const PhyResource &RB, int RevAntennaIndex ) const
    {		
		int RBIndex = RB.GetRB();
		double SigPower = CalcSelfRBPower(nodeb,ue, RB,RevAntennaIndex);
		double Ior = CalcIor();
		double Ioc = CalcIoc(nodeb,ue, RB, RevAntennaIndex);
		double NoisePower = CalcUENoisePower(nodeb, ue, RB);
		double SINR_temp = SigPower/(Ioc+Ior+NoisePower);
		return SINR_temp;
	}



	//小区间干扰
	double SINRCalculator::CalcIoc( const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB ,int RevAntennaIndex) const
	{
		return m_pIocCalc->CalcIoc(nodeb,ue,RB, RevAntennaIndex);
		throw UnFinishedFunc();
	}

	double SINRCalculator::CalcSelfRBPower( const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB, int RevAntennaIndex ) const
	{
		//发射天线数
		//int NBTxAntennaNum = nodeb.GetTxAntenna()->GetAntennaPortNum();
		double RvPower = 0.0;

		const Fading& FastFadingGen = System::GetCurrentSystem()->GetFading();
		const Fading& SlowFadingGen = System::GetCurrentSystem()->GetSlowFading();
		shared_ptr<System> pSystem = System::GetCurrentSystem();
        const Map & mapinstance =  pSystem->GetMap();
        double TxPower = RB.GetTxPower();		
        double distance = mapinstance.Distance(nodeb,ue);
		double din = ue.GetDin();
        double pathloss = pSystem->GetPathLoss(SYSTEM_LTE)(distance,din);
        double ShadowAndFastFading = FastFadingGen.GetFading(nodeb.GetID(), ue.GetID(), RB.GetRB(), RB.GetAntennaPort(), RevAntennaIndex)
            * SlowFadingGen.GetFading(nodeb.GetID(), ue.GetID(), RB.GetRB(), RB.GetAntennaPort(), RevAntennaIndex);
        //对于pathloss的除法可以提出来到外面做，节省一点算法时间
        double TxAntennaGain = nodeb.GetTxAntenna()->GetAntennaGain();
        double RxAntennaGain = ue.GetRxAntenna()->GetAntennaGain();
        RvPower += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / ShadowAndFastFading);

		return RvPower;
	}

	//计算UE的噪声功率
	double SINRCalculator::CalcUENoisePower( const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB ) const
	{
		return ue.GetNoiseFigure() * System::GetCurrentSystem()->GetThermalDensity() * System::GetCurrentSystem()->GetRBBandWidth();
	}
	//小区内干扰
	double SINRCalculator::CalcIor() const
	{
		return 0.0;
	}
	//信道模型计算
	double SINRCalculator::EESMCalculator( vector<double>& vSINR, const Channel &channel ) const
	{
		double dbeita = GetBeta(channel.GetMCS());
		int RBNum = channel.GetRBNum();
		double temp = 0.0;
		for (int i = 0; i < RBNum; ++i)
		{			
			temp += pow(M_E, -vSINR[i]/dbeita);
		}
		double SINREff = -dbeita*log10(temp/RBNum)/log10(M_E);
		return SINREff;
	}

	double SINRCalculator::GetBeta( const MCS &mcs ) const
	{
		switch(mcs.ToIndex())
		{
		case 0:  return 1.49;
		case 1:	return 1.49;
		case 2:	return 1.57;
		case 3:	return 1.69;
		case 4:	return 1.69;
		case 5:  return 4.56;
		case 6:	return 6.42;
		case 7:  return 7.33;
		case 8:	return 7.68;
		case 9:	return 15.5;
		case 10:	return 19.6;
		case 11:	return 28.6;
		default: throw std::logic_error("The MCS is not right in the GetBeta /n");
		}

	}

}

