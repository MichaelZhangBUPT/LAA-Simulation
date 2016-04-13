#include "commondef.h"
#include "DLIocCalculator.h"
#include "UE.h"
#include "NodeB.h"
#include <map>
#include "Channel.h"
#include "Pathloss.h"
#include "System.h"
#include "POS.h"
#include "System.h"
#include "Fading.h"
#include "Map.h"
#include "SmartAntenna.h"

using std::map;
namespace LTESim
{
	DLIocCalculator::DLIocCalculator(void)
	{
	}

	DLIocCalculator::~DLIocCalculator(void)
	{
	}

	double DLIocCalculator::CalcOtherNodeBsInterf( const NodeB &nodeb, const ILTEUE &ue, const PhyResource &RB,int RxAntenna )
	{
		double NBInterf = 0.0;
		System::NBLIST::const_iterator iter;
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const System::NBLIST  NodeBList = pSystem->GetNBList();
		const Pathloss& PLCalculator = pSystem->GetPathLoss(SYSTEM_LTE);
		const Fading& FastFadingGen = pSystem->GetFading();
		const Fading& SlowFadingGen = pSystem->GetSlowFading();
		const Map & mapinstance = pSystem->GetMap();
		for (iter = NodeBList.begin(); iter != NodeBList.end(); ++iter)
		{
			//跳过本小区
			const NodeB& NeighborNodeB = *iter->second;
			if ( NeighborNodeB.GetID() == ue.GetServingNodeB()->GetID() )
			{
				continue;
			}
			int TxAntennaPortNum = NeighborNodeB.GetTxAntenna()->GetAntennaPortNum();
			for (int TxAntennaPort = 0; TxAntennaPort < TxAntennaPortNum; ++TxAntennaPort)
			{
				double Txpower = NeighborNodeB.GetTXPower( RB );//取得改第i根发射RBIndex的发送功率
				double distance = mapinstance.Distance( NeighborNodeB, ue);
				double din = ue.GetDin();
				double Pathloss = PLCalculator(distance,din);
				double ShadowAndFastFading = 
					FastFadingGen.GetFading(NeighborNodeB.GetID(), ue.GetID(), RB.GetRB(), TxAntennaPort, RxAntenna)
					* SlowFadingGen.GetFading(NeighborNodeB.GetID(), ue.GetID(), RB.GetRB(), TxAntennaPort, RxAntenna);
                double TxAntennaGain = NeighborNodeB.GetTxAntenna()->GetAntennaGain();
                double RxAntennaGain = ue.GetRxAntenna()->GetAntennaGain();
				NBInterf += (Txpower * TxAntennaGain * RxAntennaGain / Pathloss / ShadowAndFastFading) ;
			}
		}
		return NBInterf;
	}

	shared_ptr<IocCalculator> DLIocCalculator::GetIocCalculator( void )
	{
		static shared_ptr<IocCalculator>  calc(new DLIocCalculator);
		return calc;
	}

    double DLSAIocCalculator::CalcOtherNodeBsInterf( const NodeB &nodeb, const ILTEUE &ue, const PhyResource &PhysicalResource,int RxAntenna )
    {
        double NBInterf = 0.0;
        System::NBLIST::const_iterator iter;
        shared_ptr<System> pSystem = System::GetCurrentSystem();
        const System::NBLIST  NodeBList = pSystem->GetNBList();
        const Pathloss& PLCalculator = pSystem->GetPathLoss(SYSTEM_LTE);
        const Fading& FastFadingGen = pSystem->GetFading();
        const Fading& SlowFadingGen = pSystem->GetSlowFading();
        const Map & mapinstance = pSystem->GetMap();
        for (iter = NodeBList.begin(); iter != NodeBList.end(); ++iter)
        {
            //跳过本小区
            const NodeB& NeighborNodeB = *iter->second;
            if ( NeighborNodeB.GetID() == ue.GetServingNodeB()->GetID() )
            {
                continue;
            }
            int TxAntennaPortNum = NeighborNodeB.GetTxAntenna()->GetAntennaPortNum();
            for (int TxAntennaPort = 0; TxAntennaPort < TxAntennaPortNum; ++TxAntennaPort)
            {
                double Txpower = NeighborNodeB.GetTXPower( PhysicalResource );//取得改第i根发射RBIndex的发送功率
                double distance = mapinstance.Distance( NeighborNodeB, ue);
				double din = ue.GetDin();
                double Pathloss = PLCalculator(distance,din);
                double ShadowAndFastFading = 
                    FastFadingGen.GetFading(NeighborNodeB.GetID(), ue.GetID(), PhysicalResource.GetRB(), TxAntennaPort, RxAntenna)
                    * SlowFadingGen.GetFading(NeighborNodeB.GetID(), ue.GetID(), PhysicalResource.GetRB(), TxAntennaPort, RxAntenna);
                double TxAntennaGain = NeighborNodeB.GetTxAntenna()->GetAntennaGain();
                double SAGain = 1.0;
                shared_ptr<ILTEUE> pNeighborUE = NeighborNodeB.GetUEToRB(PhysicalResource.GetRB(), PhysicalResource.GetAntennaPort());
                if (pNeighborUE)
                {
                    shared_ptr<SmartAntenna> pSAAntenna = dynamic_pointer_cast<SmartAntenna>(NeighborNodeB.GetTxAntenna());
                    if (pSAAntenna)
                    {
                        SAGain = pSAAntenna->GetSAGain(NeighborNodeB, *pNeighborUE, ue);
                    }
                    else
                    {
                        throw std::invalid_argument("Antenna is not a smart antenna");
                    }
                }
                double RxAntennaGain = ue.GetRxAntenna()->GetAntennaGain();
                NBInterf += (Txpower * TxAntennaGain * SAGain * RxAntennaGain / Pathloss / ShadowAndFastFading) ;
            }
        }
        return NBInterf;
    }

    shared_ptr<IocCalculator> DLSAIocCalculator::GetIocCalculator()
    {
        static shared_ptr<IocCalculator>  calc(new DLSAIocCalculator);
        return calc;
    }

    DLSAIocCalculator::DLSAIocCalculator()
    {

    }

    DLSAIocCalculator::~DLSAIocCalculator()
    {

    }
}
 
