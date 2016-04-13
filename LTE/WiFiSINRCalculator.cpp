#include "commondef.h"
#include "WiFiSINRCalculator.h"
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
	WiFiSINRCalculator * WiFiSINRCalculator::m_pInstance = NULL;
	WiFiSINRCalculator::WiFiSINRCalculator()
	{
		//可以把AP和UE的天线增益都存到成员变量里
		//shared_ptr<System> pSystem = System::GetCurrentSystem();
		//NodeBAntennaGain_dB = pSystem->GetConfig().GetDouble("HSPA_NodeB_Antenna_Gain");
		//UEAntennaGain_dB = pSystem->GetConfig().GetDouble("HSPA_UE_Antenna_Gain");
	}

	WiFiSINRCalculator::~WiFiSINRCalculator()
	{

	}

	WiFiSINRCalculator * WiFiSINRCalculator::Instance()
	{
		if ( m_pInstance == NULL )
			m_pInstance = new WiFiSINRCalculator();
		return m_pInstance;
	}

	double WiFiSINRCalculator::CalculateSINROfChannel( shared_ptr<WiFiChannel> pChannel ) const
	{
		double dSINR = 0.0;
		shared_ptr<WiFiAP> pAP = pChannel -> GetAP();
		shared_ptr<IWiFiUE> pUE = pChannel -> GetUE();
		size_t TxAntennaPortNum = pAP -> GetTxAntenna() -> GetAntennaPortNum();  
		size_t RxAntennaPortNum = pUE -> GetRxAntenna() -> GetAntennaPortNum();  
		for ( size_t rx = 0; rx < RxAntennaPortNum; ++rx )
		{
			for ( size_t tx = 0; tx < TxAntennaPortNum; ++tx )
			{
				double dUsefulPower = CalcUsefulPower( *pAP, *pUE, tx, rx );
				double dIoc = CalcIoc( *pAP, *pUE, tx, rx );
				double dNoisePower = CalcUENoisePower( *pAP, *pUE );
				double dIor = CalcIor();
				dSINR += dUsefulPower / (dIor+dIoc+dNoisePower);
			}
		}
		pChannel -> SetSINR(dSINR);
		return dSINR;
	}

	double WiFiSINRCalculator::CalcUsefulPower( const WiFiAP& ap, const IWiFiUE& ue, int TxAntennaIndex, int RxAntennaIndex ) const
	{
		double RxPower = 0.0;

		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const WiFiFading& FastFadingGen = pSystem -> GetWiFiFading();
		const WiFiFading& ShadowFadingGen = pSystem -> GetWiFiShadowFading(); 
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_WIFI );
		const Map & mapinstance =  pSystem -> GetWiFiMap();
		
		double TxPower = ap.GetMaxTxPower() / ap.GetTxAntenna() -> GetAntennaPortNum() ;//MIMO分集，所以除以2
		double distance = mapinstance.Distance(ap, ue);
		double pathloss = PathlossGen(distance);
		double FastFading = FastFadingGen.GetFading(ap.GetID(), ue.GetID(), TxAntennaIndex, RxAntennaIndex, distance);
		double ShadowFading = ShadowFadingGen.GetFading(ap.GetID(), ue.GetID(), TxAntennaIndex, RxAntennaIndex, distance);
		double TxAntennaGain = ap.GetTxAntenna() -> GetAntennaGain();
		double RxAntennaGain = ue.GetRxAntenna() -> GetAntennaGain();
		RxPower += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / FastFading / ShadowFading);

		return RxPower;
	}

	double WiFiSINRCalculator::CalcIoc( const WiFiAP& ap, const IWiFiUE& ue, int TxAntennaIndex, int RevAntennaIndex ) const
	{
		double Interf = 0.0;
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const WiFiFading& FastFadingGen = pSystem -> GetWiFiFading();
		const WiFiFading& ShadowFadingGen = pSystem -> GetWiFiShadowFading(); 
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_WIFI );
		const Map & mapinstance =  pSystem -> GetWiFiMap();

		const System::WiFiAPLIST& APList = pSystem->GetWiFiAPList();
		for ( System::WiFiAPLIST::const_iterator apListIter = APList.begin(); apListIter != APList.end(); ++apListIter )
		{
			
			shared_ptr<WiFiAP> NeighbourAP = apListIter->second;
			ChannelState channelState_t = NeighbourAP -> GetChannelState();
			if ( NeighbourAP->GetID() == ap.GetID() )
			{
				continue;
			}
			if (channelState_t == Busy)
			{
				double TxPower = NeighbourAP->GetMaxTxPower() / NeighbourAP->GetTxAntenna() -> GetAntennaPortNum() ;//MIMO分集，所以除以2？ 
				double distance = mapinstance.Distance(*NeighbourAP, ue);
				double pathloss = PathlossGen(distance);
				double FastFading = FastFadingGen.GetFading(NeighbourAP->GetID(), ue.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
				double ShadowFading = ShadowFadingGen.GetFading(NeighbourAP->GetID(), ue.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
				double TxAntennaGain = NeighbourAP->GetTxAntenna() -> GetAntennaGain();
				double RxAntennaGain = ue.GetRxAntenna() -> GetAntennaGain();
				Interf += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / FastFading / ShadowFading);
			}
			else
				continue;			
		}

		const System::LAANBLIST& LAANodeBList = pSystem->GetLAANBList();
		for ( System::LAANBLIST::const_iterator LAAnbListIter = LAANodeBList.begin(); LAAnbListIter != LAANodeBList.end(); ++LAAnbListIter )
		{
			shared_ptr <LAANodeB> LAANeighbourNB = LAAnbListIter->second;		
			ChannelState channelState_t = LAANeighbourNB -> GetChannelState();
			if (channelState_t == Busy)
			{
				double TxPower = LAANeighbourNB->GetMaxTxPower() / LAANeighbourNB->GetTxAntenna() -> GetAntennaPortNum() ;//MIMO分集，所以除以2？ 
				double distance = mapinstance.Distance(*LAANeighbourNB, ue);
				double pathloss = PathlossGen(distance);
				double FastFading = FastFadingGen.GetFading(LAANeighbourNB->GetID(), ue.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
				double ShadowFading = ShadowFadingGen.GetFading(LAANeighbourNB->GetID(), ue.GetID(), TxAntennaIndex, RevAntennaIndex, distance);
				double TxAntennaGain = LAANeighbourNB->GetTxAntenna() -> GetAntennaGain();
				double RxAntennaGain = ue.GetRxAntenna() -> GetAntennaGain();
				Interf += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / FastFading / ShadowFading);
			}
			else
				continue;
	     }
		return Interf;
	}

	double WiFiSINRCalculator::CalcUENoisePower( const WiFiAP& ap, const IWiFiUE& ue ) const
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		return ue.GetWiFiNoiseFigure() * pSystem->GetWiFiThermalDensity() * pSystem->GetWiFiBandWidth();
	}

	void WiFiSINRCalculator::OutputAllSINR( std::ostream& os ) const
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const WiFiFading& FastFadingGen = pSystem -> GetWiFiFading();
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_WIFI );
		double TxPower = dBToReal( pSystem->GetConfig().GetDouble( "WiFi_AP_MaxTxPower(dBm)" ) )/2;
		double dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("WiFi_UE_Noise_Figure(dB)"));
		double dNoisePower = dNoiseFigure *	pSystem->GetWiFiThermalDensity() * pSystem->GetWiFiBandWidth(); 
		shared_ptr<AMCEntity> pAMCEntity = shared_ptr<AMCEntity>(new AMCEntity(pSystem->GetWiFiAMCTable(), DB(0.0), DB(0.0)));

		using std::make_pair;
		for ( double x = -50.0; x <= 50.0; x += 0.5 )
		{
			for ( double y = -50.0; y <= 50.0; y += 0.5 )
			{
				double dSINR = 0.0;
				for ( int rx = 0; rx < 2; ++rx )
				{
					for ( int tx = 0; tx < 2; ++tx )
					{
						double distance = sqrt( x*x + y*y );
						double pathloss = PathlossGen(distance);
						double FastFading = FastFadingGen.GetFading(0, 0, tx, rx,distance);
						double dRxPower = TxPower / pathloss / FastFading;
						dSINR += dRxPower / dNoisePower;
					}
				}
				dSINR = RealTodB(dSINR);
				if( dSINR > 10000 ) dSINR = 10000;
				MCS mcsindex = pAMCEntity -> SelectMCS( DB( dSINR ) );
				double wifirate = mcsindex.GetWiFiRate();

				os << wifirate << "\t";
			}
			os << endl;
		}
	}

	double WiFiSINRCalculator::CalcIor() const
	{
		return 0.0;
	}

}