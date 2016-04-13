#include "commondef.h"
#include "C2ICalculator.h"
#include "HSPAChannel.h"
#include <exception>
#include "HSPAFading.h"
#include <numeric>
#include <fstream>
#include "SimLog.h"
#include "ClockManager.h"

namespace LTESim
{
	C2ICalculator * C2ICalculator::m_pInstance = NULL;
	//std::ofstream ftest_C2I("../Output/C2I.txt");
	//std::ofstream Pathloss_value("../Output/Pathloss.txt");
	//std::ofstream Pathloss_other("../Output/PathOther.txt");
	C2ICalculator::C2ICalculator()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		NodeBAntennaGain_dB = pSystem->GetConfig().GetDouble("HSPA_NodeB_Antenna_Gain");
		UEAntennaGain_dB = pSystem->GetConfig().GetDouble("HSPA_UE_Antenna_Gain");
		CableLoss_dB = pSystem->GetConfig().GetDouble("HSPA_UE_Cable_Loss");
		UEOtherLoss_dB = pSystem->GetConfig().GetDouble("HSPA_UE_Other_Loss");
	}

	C2ICalculator * C2ICalculator::Instance()
	{
		if ( m_pInstance == NULL )
			m_pInstance = new C2ICalculator();
		return m_pInstance;
	}

	void C2ICalculator::CalculateC2IOfChannel(shared_ptr<HSPAChannel> pChannel )
	{
		//double C2I = 0;
		//set<int>::const_iterator iter = (pChannel->GetStreamSet()).begin();
		//for (; iter != (pChannel->GetStreamSet()).end(); ++iter)
		//{
		//	MatrixCalc(pChannel);
		//	
		//	double SigPower = GetSelfPower(pChannel, *iter);

		//	double Ior = CalcIor(pChannel, *iter);
	
		//	double Ioc = CalcIoc(pChannel, *iter);
		//
		//	C2I += SigPower / (Ior + Ioc + m_NoiseMatrix[*iter][*iter].real() );
		//	//cout << endl;
		//	//cout << "Ior " << Ior << "  Ioc  " << Ioc << " Noise " << m_NoiseMatrix[*iter][*iter].real() << endl;
		//	
		//}
		////cout << endl;
		////cout << "C2I  " << C2I << endl;
		//pChannel->RestoreC2I(C2I);

		//////////////////////////////////////////////////////////////////////////
		//采用分集，MRC合并；计算每根接收天线的C2I,然后相加
		double C2I = 0;
		shared_ptr<HSPANodeB> nodeb = pChannel->GetNodeB();
		shared_ptr<IHSPAUE>	ue = pChannel->GetUE();
		shared_ptr<Antenna> pTxAntenna = nodeb->GetAntenna();
		size_t TxantennaPort = pTxAntenna->GetAntennaPortNum();
        shared_ptr<Antenna> pRxAntenna = ue->GetRxAntenna();
		size_t RxantennaPort = pRxAntenna->GetAntennaPortNum();
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const double	BAND_WIDTH	= 5e6;  
		const HSPAFading& fading = pSystem->GetHSPAFading();
		//计算接收天线
		for (size_t rx = 0; rx < RxantennaPort; ++rx)
		{
			for (size_t tx = 0; tx < TxantennaPort; ++tx)
			{
				//小区内干扰功率
				double Ior = 0.0;
				//小区间干扰功率
				double NBInterf = 0.0;
				//有用信号功率
				double SelfPower = 0.0;
				System::HSPANBLIST::const_iterator iter;
				const System::HSPANBLIST  NodeBList = pSystem->GetHSPANBList();
				for (iter = NodeBList.begin(); iter != NodeBList.end(); ++iter)
				{
					const HSPANodeB& NeighborNodeB = *iter->second;
					shared_ptr<HSPANodeB> pNeighborNodeB = iter->second;
					if ( NeighborNodeB.GetID() == ue->GetServingHSPANodeB()->GetID() )
					{
						double PropLoss_db = 0.0;
						double AntennaGain_db = 0.0;
						if (pChannel->GetLinkDirection() == DOWNLINK)
						{
							double fastfadingself = RealTodB( norm(fading.GetFading(pSystem->ConvertNBAbsoluteIDToOffsetID(pNeighborNodeB->GetID(), SYSTEM_HSPA), ue->GetID(), 0, tx, rx) ) );							
							double fPropLoss_dBself  = SCMPropLossDBDL(pNeighborNodeB, ue) + fastfadingself;
							double txpower = pChannel->GetTxPower()/TxantennaPort;
							SelfPower = txpower * dBToReal(-fPropLoss_dBself);
							//SimLog::Instance()->LogC2I( pChannel->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), SelfPower );
							//SimLog::Instance()->LogC2ICal( ue->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), ue->GetType(), dBToReal(-fPropLoss_dBself));
							//cout << "TX  " << txpower << endl;
							//Pathloss_value << ue->GetID() << "  " << dBToReal(-fPropLoss_dBself) << endl;
						}
						else
						{
							std::cerr << "Channel must be DL" << endl;
						}
					}
					else
					{	
						double fastfading = RealTodB( norm(fading.GetFading(pSystem->ConvertNBAbsoluteIDToOffsetID(pNeighborNodeB->GetID(), SYSTEM_HSPA ), ue->GetID(), 0, tx, rx) ) );
						double fPropLoss_dB  = SCMPropLossDBDL(pNeighborNodeB, ue) + fastfading;
						vector<shared_ptr<IHSPAUE>> UEtemp = pNeighborNodeB->GetScheduledUE();
						//if (UEtemp.size() > 1)
						//{
						//	throw std::runtime_error( "Error. More than one UE are scheduled" );
						//}

						double TxPowerStream = pNeighborNodeB->GetTransmitPower() / double(TxantennaPort);
						if (TxPowerStream< 0.000000000001 && TxPowerStream > -0.0000000000001)
						{
							cout << "Holly crap " << endl;
						}
						if ( TxPowerStream > 0.0 )
						{						
							double RxPowerStream = TxPowerStream * dBToReal(-fPropLoss_dB);
							//Pathloss_other << ue->GetID() << "  " << dBToReal(-fPropLoss_dB) << endl;
							//SimLog::Instance()->LogC2I( pChannel->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), RxPowerStream );
							NBInterf += RxPowerStream;
						}

					}
				}
				double Noisepower  = ue->GetNoiseFigure() * dBToReal(pSystem->GetConfig().GetDouble("HSPA_UE_Noise_Density") ) * BAND_WIDTH;
				//cout << ue->GetNoiseFigure() <<"  " << dBToReal(pSystem->GetConfig().GetDouble("HSPA_UE_Noise_Density") ) << endl;
				C2I += SelfPower / (Ior + NBInterf + Noisepower );
			}
		}		
		pChannel->RestoreC2I(C2I);
	}

	double C2ICalculator::CalculateEffectiveC2I(shared_ptr<HSPAChannel> pChannel )
	{
		int codes = 0;
		for (int i = 0; i < HSPASLOTPERSUBFRAME; ++i)
		{
			codes += (int)(pChannel->GetCodeChannelSet().size()); 
		}
		double effecC2I = std::accumulate(pChannel->GetC2IVector().begin(), pChannel->GetC2IVector().end(), 0.0);
		//effecC2I /= pChannel->GetC2IVector().size();
		return effecC2I / codes;		
	}

	double C2ICalculator::GetSelfPower(shared_ptr<HSPAChannel> pChannel, int stream)
	{
		double PropLoss_db = 0.0;
		double AntennaGain_db = 0.0;
		shared_ptr<HSPANodeB> nodeb = pChannel->GetNodeB();
		shared_ptr<IHSPAUE>	ue = pChannel->GetUE();
		//fileselfPath << ue->GetUEId() << "    " << dBToRealValue(AntennaGain_db - fPropLoss_dB) << "    " << AntennaGain_db << "    " << fPropLoss_dB << endl;
		if (pChannel->GetLinkDirection() == DOWNLINK)
		{
			double txpower = pChannel->GetTxPower() * norm(m_KMatrix[stream][stream]);
			PropLoss_db = SCMPropLossDBDL(nodeb, ue);
			return txpower * dBToReal(-PropLoss_db);
		}
		else
		{
			std::cerr << "DLMIMOMMSE::Channel must be DL" << endl;
			return 0.0;
		}
	}

	double C2ICalculator::SCMPropLossDBDL(shared_ptr<HSPANodeB> nodeb, shared_ptr<IHSPAUE>	ue)
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const Map & mapinstance =  pSystem->GetMap();
		double distance = mapinstance.Distance(*nodeb,*ue);
		double pathloss = RealTodB(pSystem->GetPathLoss(SYSTEM_HSPA)(distance));
		const Fading& SlowFadingGen = pSystem->GetSlowFading();
		double ShadowFading = RealTodB(SlowFadingGen.GetFading(nodeb->GetID(), ue->GetID(), 0, 0, 0));
		double pathlossPlusShadowFading = pathloss + ShadowFading;
		

		double	fPropagationLoss_dB	= pathlossPlusShadowFading - NodeBAntennaGain_dB - UEAntennaGain_dB + CableLoss_dB + UEOtherLoss_dB;
		return	fPropagationLoss_dB;
	}

	 void C2ICalculator::MatrixCalc(shared_ptr<HSPAChannel> pChannel)
	 {
		 shared_ptr<HSPANodeB> nodeb = pChannel->GetNodeB();
		 shared_ptr<IHSPAUE>	ue = pChannel->GetUE();
		 const double	BAND_WIDTH	= 5e6;          
		 Matrix HMatrix = InialMatrix(12, 2);
		 HMatrix = HMatrixGen(pChannel);
		 Matrix Wrow1 = pChannel->GetWeightDL();
		 Matrix WMatrix = InialMatrix(2, 2);
		 WMatrix[0][0] = Wrow1[0][0];
		 WMatrix[0][1] = Wrow1[0][0];
		 WMatrix[1][0] = Wrow1[1][0];
		 WMatrix[1][1] = -Wrow1[1][0];
		 shared_ptr<System> pSystem = System::GetCurrentSystem();
		 double UENoiseFigure_dB = pSystem->GetConfig().GetDouble("HSPA_UE_Noise_Figure");
		 double UENoiseDensity_dB = pSystem->GetConfig().GetDouble("HSPA_UE_Noise_Density");
		 double UENoisePower = dBToReal(UENoiseFigure_dB + UENoiseDensity_dB)*BAND_WIDTH;

		 m_NoiseMatrix = MatrixMultiplyNumber( InialUnitMatrix(2), UENoisePower );
		 Matrix HWtemp = MatrixMultiply(HMatrix, WMatrix );
		 Matrix temp = MatrixPlus( MatrixMultiply(MatrixConjTranspose(HWtemp), HWtemp ), m_NoiseMatrix );
		 m_GMatrix = MatrixMultiply( MatrixInverse( MatrixPlus( MatrixMultiply(MatrixConjTranspose(HWtemp), HWtemp ), m_NoiseMatrix )), MatrixConjTranspose(HWtemp));
		 m_NoiseMatrix = MatrixMultiplyNumber( MatrixMultiply( m_GMatrix, MatrixConjTranspose(m_GMatrix) ),  UENoisePower );
		 m_KMatrix = MatrixMultiply(m_GMatrix, HWtemp);

	 }

	  Matrix C2ICalculator::HMatrixGen(shared_ptr<HSPAChannel> pChannel)
	  {
		  shared_ptr<System> pSystem = System::GetCurrentSystem();
		  const HSPAFading& FastFadingGen = System::GetCurrentSystem()->GetHSPAFading();
		  shared_ptr<HSPANodeB> nodeb = pChannel->GetNodeB();
		  shared_ptr<IHSPAUE>	ue = pChannel->GetUE();

		  Matrix HMatrix = InialMatrix(12, 2);
		 
		  for(int rxantnum = 0;rxantnum < 2;++rxantnum)
		  {			
			  for(int txantnum = 0;txantnum < 2;++txantnum)
			  {	
				  for (int pathindex = 0; pathindex < 6; ++pathindex)
				  {
					  HMatrix[rxantnum * 6 + pathindex][txantnum] = FastFadingGen.GetFading(pSystem->ConvertNBAbsoluteIDToOffsetID(nodeb->GetID(),SYSTEM_HSPA), ue->GetID(),pathindex,txantnum,rxantnum);
				  }
			  }
		  }
		  return HMatrix;
	  }

	   double C2ICalculator::CalcIor(shared_ptr<HSPAChannel> pChannel, int stream)
	   {
		   shared_ptr<HSPANodeB> nodeb = pChannel->GetNodeB();
		   shared_ptr<IHSPAUE>	ue = pChannel->GetUE();
		   double fPropLoss_dB  = SCMPropLossDBDL(nodeb, ue);
		   const int Stream = 2;
		   const int InterfStreamIndex = 1 - stream;
		   double TxPower = pChannel->GetTxPower();
		   double RxPower = TxPower * dBToReal( - fPropLoss_dB);
		   double Interf = norm(m_KMatrix[stream][InterfStreamIndex] ) * RxPower;
		   return Interf;
	   }

	   double C2ICalculator::CalcIoc(shared_ptr<HSPAChannel> pChannel, int stream)
	   {
		   double NBInterf = 0.0;
		   System::HSPANBLIST::const_iterator iter;
		   shared_ptr<System> pSystem = System::GetCurrentSystem();
		   const System::HSPANBLIST  NodeBList = pSystem->GetHSPANBList();
		   
		   shared_ptr<HSPANodeB> nodeb = pChannel->GetNodeB();
		   shared_ptr<IHSPAUE>	ue = pChannel->GetUE();
		   
		   for (iter = NodeBList.begin(); iter != NodeBList.end(); ++iter)
		   {
			   
			   const HSPANodeB& NeighborNodeB = *iter->second;
			   shared_ptr<HSPANodeB> pNeighborNodeB = iter->second;
			   if ( NeighborNodeB.GetID() == ue->GetServingHSPANodeB()->GetID() )
			   {
				   continue;
			   }
			   else
			   {	
				   Matrix HotherMatrix = InialMatrix(12, 2);
					HotherMatrix = HMatrixGen(pChannel);

				   double fPropLoss_dB  = SCMPropLossDBDL(pNeighborNodeB, ue);
				   //fileIocPath << ue->GetUEId() << "    " << dBToRealValue(AntennaGain_db - fPropLoss_dB) << "    " << AntennaGain_db << "    " << fPropLoss_dB << endl;
				   const int Stream = 2;
				   double TxPowerStream[Stream];
				   double RxPowerStream[Stream];
				   vector<shared_ptr<IHSPAUE>> UEtemp = pNeighborNodeB->GetScheduledUE();
				   if (UEtemp.size() > 1)
				   {
					   throw std::runtime_error( "Error. More than one UE are scheduled" );
				   }
				   vector<shared_ptr<IHSPAUE>>::iterator UE_itr = UEtemp.begin();
				   for ( ; UE_itr != UEtemp.end() ; ++UE_itr)
				   {
					   for (int index = 0; index < Stream; ++index)
					   {
						   TxPowerStream[index] = pNeighborNodeB->GetTransmitPower();
							if (TxPowerStream[index]< 0.000000000001 && TxPowerStream[index] > -0.0000000000001)
							{
								cout << "Holly crap " << endl;
							}
						   if ( TxPowerStream[index] > 0.0 )
						   {
							   Matrix Wrow1 = pChannel->GetWeightDL();
							   Matrix WMatrix = InialMatrix(2, 2);
							   WMatrix[0][0] = Wrow1[0][0];
							   WMatrix[1][0] = Wrow1[1][0];
							   WMatrix[0][1] = Wrow1[0][0];
							   WMatrix[1][1] = -Wrow1[1][0];
							   Matrix K_Matrix = MatrixMultiply( m_GMatrix, MatrixMultiply(HotherMatrix, WMatrix) );
							//   cout << endl;
							//cout << "Matrix " << endl;
							//PrintMatrix(WMatrix);
							//cout << " Norm " << norm(K_Matrix[stream][index]) << endl;
							   RxPowerStream[index] = TxPowerStream[index] * dBToReal(-fPropLoss_dB);
							   if (norm(K_Matrix[stream][index]) < 0.00000001 && norm(K_Matrix[stream][index]) > -0.00000001)
							   {
								   cout << "Another holly crap " << endl;
							   }
							   NBInterf += norm(K_Matrix[stream][index]) * RxPowerStream[index];
						   }

					   }
					   //cout << endl;
					   //cout << "NBInterf  " << NBInterf << endl;
				   }

			   }	
		   }
		   return NBInterf;
	   }

	   Matrix C2ICalculator::MatrixGenerator(shared_ptr<HSPAChannel> pChannel)
	   {
		   Matrix HMatrix = InialMatrix(12, 2);
		   HMatrix = HMatrixGen(pChannel);//ue->GetHMatrix(freq);
		   
		   Matrix weighttemp = InialMatrix(2, 1);
		   double tempMax = -9999;
		   int weightnum = -1;	
		   for (int weightindex = 0; weightindex < 4; ++weightindex)
		   {
			   weighttemp[0][0] = HSPAMIMOWeight[weightindex][0];
			   weighttemp[1][0] = HSPAMIMOWeight[weightindex][1];
			   Matrix HWtemp = MatrixMultiply( HMatrix, weighttemp);
			   double temp = MatrixMultiply( MatrixConjTranspose(HWtemp), HWtemp )[0][0].real();
			   if (temp > tempMax)
			   {
				   tempMax = temp;
				   weightnum = weightindex;
			   }
		   }
		   weighttemp[0][0] = HSPAMIMOWeight[weightnum][0];
		   weighttemp[1][0] = HSPAMIMOWeight[weightnum][1];
		   
		   return weighttemp;
	   }
}