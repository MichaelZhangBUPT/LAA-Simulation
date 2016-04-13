#include "commondef.h"
#include "LAACCA.h"
#include <exception>
#include <numeric>
#include <fstream>
#include "SimLog.h"
#include "ClockManager.h"
#include "System.h"
#include "LAAFading.h"
#include "Pathloss.h"
#include "Map.h"
namespace LTESim
{
	LAACCACalculator * LAACCACalculator::m_pInstance = NULL;
	LAACCACalculator::LAACCACalculator()
	{

	}

	LAACCACalculator::~LAACCACalculator()
	{

	}

	LAACCACalculator * LAACCACalculator::Instance()
	{
		if ( m_pInstance == NULL )
			m_pInstance = new LAACCACalculator();
		return m_pInstance;
	}


bool LAACCACalculator::CalculateCCA( shared_ptr<LAANodeB> pLAANodeB ) const
	{
		bool bCCAState = 1;//默认为空闲
		double dCCA_SUM = 0.0;
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		
		//接收到相邻wifi的信号强度
		const System::WiFiAPLIST& WiFiAPList = pSystem->GetWiFiAPList();
		for ( System::WiFiAPLIST::const_iterator apListIter = WiFiAPList.begin(); apListIter != WiFiAPList.end(); ++apListIter )
		{
				shared_ptr<WiFiAP> pAP = apListIter->second;
				size_t TxAntennaPortNum = pAP -> GetTxAntenna() -> GetAntennaPortNum();   
				for (size_t tx = 0; tx < TxAntennaPortNum; ++tx)
				{
					size_t rx = 1;
					double dCCA_single = 0;
					ChannelState channelState_t = pAP -> GetChannelState();
					if (channelState_t == Busy)
					{
						dCCA_single = CalculateRxPower(*pLAANodeB,*pAP,tx,rx);
					}
					    dCCA_SUM += dCCA_single;
				}
								
		}
		
		//接收到相邻LAANodeB的信号强度
		const System::LAANBLIST& LAANodeBList = pSystem->GetLAANBList();
		for ( System::LAANBLIST::const_iterator LAAnbListIter = LAANodeBList.begin(); LAAnbListIter != LAANodeBList.end(); ++LAAnbListIter )
		{
			shared_ptr<LAANodeB> pLAANB = LAAnbListIter->second;
			size_t TxAntennaPortNum = pLAANB -> GetTxAntenna() -> GetAntennaPortNum();  
			for (size_t tx = 0; tx < TxAntennaPortNum; ++tx)
			{
				size_t rx = 1;
				double dCCA_single = 0;
				ChannelState channelState_t = pLAANB -> GetChannelState();
				if (channelState_t == Busy)
				{
					dCCA_single= CalculateRxPower(*pLAANodeB,*pLAANB,tx,rx);
				}	
				dCCA_SUM += dCCA_single;
			}
			
		}
		double LAACCAThr = dBToReal(pSystem -> GetConfig().GetDouble("CCA_EDThr_LAA(dBm)"));
		//int test = 0;
		if (dCCA_SUM > LAACCAThr)
		{
			bCCAState = false;
		} 
		else
		{
			bCCAState = true;
			//test = 1;
		}
		/*std::ofstream CCA("../Output/LAACCARESULT.txt",std::ios::app);
		CCA <<test<<endl;*/
		return bCCAState;
	}

		


double LAACCACalculator::CalculateRxPower( const LAANodeB& nb1, const LAANodeB& nb2, int TxAntennaIndex, int RevAntennaIndex ) const
	{
		double RxPower = 0.0;

		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const LAAFading& FastFadingGen = pSystem -> GetLAAFading();
		const LAAFading& ShadowFadingGen = pSystem -> GetLAAShadowFading(); 
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_LAA );
		const Map & mapinstance =  pSystem -> GetLAAMap();

		double TxPower = nb2.GetMaxTxPower() / nb2.GetTxAntenna() -> GetAntennaPortNum() ;//MIMO分集，所以除以2？
		double distance = mapinstance.Distance(nb1, nb2);
		double pathloss = PathlossGen(distance);
		double FastFading = FastFadingGen.GetFading(nb1.GetID(), nb2.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
		double ShadowFading = ShadowFadingGen.GetFading(nb1.GetID(), nb2.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
		double TxAntennaGain = nb2.GetTxAntenna() -> GetAntennaGain();
		double RxAntennaGain = 1.0;
		RxPower += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / FastFading / ShadowFading);

		return RxPower;
	}









double LAACCACalculator::CalculateRxPower( const LAANodeB& nb,const WiFiAP& ap, int TxAntennaIndex, int RevAntennaIndex ) const
	{
		double RxPower = 0.0;

		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const LAAFading& FastFadingGen = pSystem -> GetLAAFading();
		const LAAFading& ShadowFadingGen = pSystem -> GetLAAShadowFading(); 
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_WIFI );
		const Map & mapinstance =  pSystem -> GetWiFiMap();

		double TxPower = ap.GetMaxTxPower() / ap.GetTxAntenna() -> GetAntennaPortNum() ;//MIMO分集，所以除以2？
		double distance = mapinstance.Distance(nb, ap);
		double pathloss = PathlossGen(distance);
		double FastFading = FastFadingGen.GetFading(nb.GetID(), ap.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
		double ShadowFading = ShadowFadingGen.GetFading(nb.GetID(), ap.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
		double TxAntennaGain = ap.GetTxAntenna() -> GetAntennaGain();
		double RxAntennaGain = 1.0;
		RxPower += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / FastFading / ShadowFading);

		return RxPower;
	}


}

