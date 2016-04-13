#include "commondef.h"
#include "LAASINRCalculator.h"
#include <exception>
#include <numeric>
#include <fstream>
#include "SimLog.h"
#include "ClockManager.h"
#include "System.h"
#include "LAAFading.h"
#include "Pathloss.h"
#include "Map.h"
#include "WiFiAP.h"
namespace LTESim
{
	LAASINRCalculator * LAASINRCalculator::m_pInstance = NULL;
	LAASINRCalculator::LAASINRCalculator()
	{
		//可以把AP和UE的天线增益都存到成员变量里
		//shared_ptr<System> pSystem = System::GetCurrentSystem();
		//NodeBAntennaGain_dB = pSystem->GetConfig().GetDouble("HSPA_NodeB_Antenna_Gain");
		//UEAntennaGain_dB = pSystem->GetConfig().GetDouble("HSPA_UE_Antenna_Gain");
	}

	LAASINRCalculator::~LAASINRCalculator()
	{

	}

	LAASINRCalculator * LAASINRCalculator::Instance()
	{
		if ( m_pInstance == NULL )
			m_pInstance = new LAASINRCalculator();
		return m_pInstance;
	}

	double LAASINRCalculator::CalculateSINROfChannel( shared_ptr<LAAChannel> pChannel ) const
	{
		double dSINR = 0.0;
		shared_ptr<LAANodeB> pNB = pChannel -> GetNB();
		shared_ptr<ILAAUE> pUE = pChannel -> GetUE();
		size_t TxAntennaPortNum = pNB -> GetTxAntenna() -> GetAntennaPortNum();  
		size_t RxAntennaPortNum = pUE -> GetRxAntenna() -> GetAntennaPortNum();  
		for ( size_t rx = 0; rx < RxAntennaPortNum; ++rx )
		{
			for ( size_t tx = 0; tx < TxAntennaPortNum; ++tx )
			{
				double dUsefulPower = CalcUsefulPower( *pNB, *pUE, tx, rx );
				double dNoisePower = CalcUENoisePower( *pNB, *pUE );
				double dIoc = CalcIoc(*pNB, *pUE, tx, rx);
				double dIor = CalcIor();
				dSINR += dUsefulPower / (dNoisePower+dIoc+dIor);
			}
		}
		pChannel -> SetSINR(dSINR);
		return dSINR;
	}

	double LAASINRCalculator::CalcUsefulPower( const LAANodeB& nb, const ILAAUE& ue, int TxAntennaIndex, int RxAntennaIndex ) const
	{
		double RxPower = 0.0;

		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const LAAFading& FastFadingGen = pSystem -> GetLAAFading();
		const LAAFading& ShadowFadingGen = pSystem -> GetLAAShadowFading(); 
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_LAA );
		const Map & mapinstance =  pSystem -> GetLAAMap();
		
		double TxPower = nb.GetMaxTxPower() / nb.GetTxAntenna() -> GetAntennaPortNum() ;//MIMO分集，所以除以2？
		double distance = mapinstance.Distance(nb, ue);
		double pathloss = PathlossGen(distance);
		double FastFading = FastFadingGen.GetFading(nb.GetID(), ue.GetID(), TxAntennaIndex, RxAntennaIndex, distance);
		double ShadowFading = ShadowFadingGen.GetFading(nb.GetID(), ue.GetID(), TxAntennaIndex, RxAntennaIndex, distance);
		double TxAntennaGain = nb.GetTxAntenna() -> GetAntennaGain();
		double RxAntennaGain = ue.GetRxAntenna() -> GetAntennaGain();
		RxPower += (TxPower * TxAntennaGain * RxAntennaGain / pathloss / FastFading / ShadowFading);

		return RxPower;
	}

	double LAASINRCalculator::CalcUENoisePower( const LAANodeB& nb, const ILAAUE& ue ) const
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		return ue.GetLAANoiseFigure() * pSystem->GetLAAThermalDensity() * pSystem->GetLAABandWidth();
	}


	double LAASINRCalculator::CalcIoc( const LAANodeB& nb, const ILAAUE& ue, int TxAntennaIndex, int RevAntennaIndex ) const
	{
		double Interf = 0.0;
        shared_ptr<System> pSystem = System::GetCurrentSystem();
		const LAAFading& FastFadingGen = pSystem -> GetLAAFading();
		const LAAFading& ShadowFadingGen = pSystem -> GetLAAShadowFading(); 
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_LAA );
		const Map & mapinstance =  pSystem -> GetLAAMap();

		const System::LAANBLIST& LAANodeBList = pSystem->GetLAANBList();
		for ( System::LAANBLIST::const_iterator LAAnbListIter = LAANodeBList.begin(); LAAnbListIter != LAANodeBList.end(); ++LAAnbListIter )
		{
			shared_ptr <LAANodeB> LAANeighbourNB = LAAnbListIter->second;
			if ( LAANeighbourNB->GetID() == nb.GetID() )
			{
				continue;
			}
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

		const System::WiFiAPLIST& APList = pSystem->GetWiFiAPList();
		for ( System::WiFiAPLIST::const_iterator apListIter = APList.begin(); apListIter != APList.end(); ++apListIter )
		{
			//改到这
			shared_ptr<WiFiAP> NeighbourAP = apListIter->second;
			ChannelState channelState_t = NeighbourAP -> GetChannelState();
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
		return Interf;
	}
		

	void LAASINRCalculator::OutputAllSINR( std::ostream& os ) const
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const LAAFading& FastFadingGen = pSystem -> GetLAAFading();
		const Pathloss& PathlossGen = pSystem -> GetPathLoss( SYSTEM_LAA );
		double TxPower = dBToReal( pSystem->GetConfig().GetDouble( "LAA_NodeB_MaxTxPower(dBm)" ) )/2;
		double dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("LAA_UE_Noise_Figure(dB)"));
		double dNoisePower = dNoiseFigure *	pSystem->GetLAAThermalDensity() * pSystem->GetLAABandWidth(); 
		shared_ptr<AMCEntity> pAMCEntity = shared_ptr<AMCEntity>(new AMCEntity(pSystem->GetLAAAMCTable(), DB(0.0), DB(0.0)));

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
				double laarate = mcsindex.GetLAARate();

				os << laarate << "\t";
			}
			os << endl;
		}
	}

	double LAASINRCalculator::CalcIor() const
	{
		return 0.0;
	}

}