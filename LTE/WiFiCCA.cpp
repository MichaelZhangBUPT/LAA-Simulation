#include "commondef.h"
#include "WiFiCCA.h"
#include <exception>
#include <numeric>
#include <fstream>
#include "SimLog.h"
#include "ClockManager.h"
#include "System.h"
#include "WiFiFading.h"
#include "Pathloss.h"
#include "Map.h"

namespace LTESim
{
	WiFiCCACalculator * WiFiCCACalculator::m_pInstance = NULL;
	WiFiCCACalculator::WiFiCCACalculator()
	{

	}

	WiFiCCACalculator::~WiFiCCACalculator()
	{

	}

	WiFiCCACalculator * WiFiCCACalculator::Instance()
	{
		if ( m_pInstance == NULL )
			m_pInstance = new WiFiCCACalculator();
		return m_pInstance;
	}

	bool WiFiCCACalculator::CalculateCCA( shared_ptr<WiFiAP> pAP ) const
	{
		bool bCCAState = 1;//默认为空闲
		double dCCA_SUM = 0.0;
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		
		//接收到相邻wifi的信号强度
		const System::WiFiAPLIST& WiFiAPList = pSystem->GetWiFiAPList();
		for ( System::WiFiAPLIST::const_iterator apListIter = WiFiAPList.begin(); apListIter != WiFiAPList.end(); ++apListIter )
		{
				shared_ptr<WiFiAP> pOtherAP = apListIter->second;
				size_t TxAntennaPortNum = pOtherAP -> GetTxAntenna() -> GetAntennaPortNum();   
				for (size_t tx = 0; tx < TxAntennaPortNum; ++tx)
				{
					size_t rx = 1;
					double dCCA_single = 0;
					ChannelState channelState_t = pOtherAP -> GetChannelState();
					if (channelState_t == Busy)
					{
						dCCA_single = CalculateRxPower(*pAP,*pOtherAP,tx,rx);
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
					dCCA_single= CalculateRxPower(*pLAANB,*pAP,tx,rx);
				}	
				dCCA_SUM += dCCA_single;
			}
			
		}
		double LAACCAThr = dBToReal(pSystem -> GetConfig().GetDouble("CCA_EDThr_WiFi(dBm)"));
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
		/*std::ofstream CCA("../Output/wifiCCARESULT.txt",std::ios::app);
		CCA <<test<<endl;*/
		return bCCAState;
	}
		


	double WiFiCCACalculator::CalculateRxPower( const WiFiAP& ap1, const WiFiAP& ap2, int TxAntennaIndex, int RevAntennaIndex ) const
	{
		double RxPower = 0.0;

		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const WiFiFading& FastFadingGen = pSystem -> GetWiFiFading();
		const WiFiFading& ShadowFadingGen = pSystem -> GetWiFiShadowFading(); 
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_WIFI );
		const Map & mapinstance =  pSystem -> GetWiFiMap();

		double TxPower = ap2.GetMaxTxPower() / ap2.GetTxAntenna() -> GetAntennaPortNum() ;//MIMO分集，所以除以2？
		double distance = mapinstance.Distance(ap1, ap2);
		double pathloss = PathlossGen(distance);
		double FastFading = FastFadingGen.GetFading(ap1.GetID(), ap2.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
		double ShadowFading = ShadowFadingGen.GetFading(ap1.GetID(), ap2.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
		double TxAntennaGain = ap2.GetTxAntenna() -> GetAntennaGain();
		double RxAntennaGain = 1.0;
		RxPower += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / FastFading / ShadowFading);

		return RxPower;
	}


	double WiFiCCACalculator::CalculateRxPower( const LAANodeB& nb,const WiFiAP& ap, int TxAntennaIndex, int RevAntennaIndex ) const
	{
		double RxPower = 0.0;

		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const WiFiFading& FastFadingGen = pSystem -> GetWiFiFading();
		const WiFiFading& ShadowFadingGen = pSystem -> GetWiFiShadowFading(); 
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_LAA );
		const Map & mapinstance =  pSystem -> GetLAAMap();

		double TxPower = nb.GetMaxTxPower() / nb.GetTxAntenna() -> GetAntennaPortNum() ;//MIMO分集，所以除以2？
		double distance = mapinstance.Distance(nb, ap);
		double pathloss = PathlossGen(distance);
		double FastFading = FastFadingGen.GetFading(ap.GetID(), nb.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
		double ShadowFading = ShadowFadingGen.GetFading(ap.GetID(), nb.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
		double TxAntennaGain = nb.GetTxAntenna() -> GetAntennaGain();
		double RxAntennaGain = 1.0;
		RxPower += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / FastFading / ShadowFading);

		return RxPower;
	}
}