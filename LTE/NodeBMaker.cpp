#include "commondef.h"
#include "NodeBMaker.h"
#include "NodeB.h"
#include "HSPANodeB.h"
#include "DownlinkScheduler.h"
#include "TDPFDownlinkScheduler.h"
#include "TDRRDownlinkScheduler.h"
#include "TDmaxC2IDownlinkScheduler.h"
#include "FDmaxC2IDownlinkScheduler.h"
#include "FDPFDownlinkScheduler.h"
#include "FDRRDownlinkScheduler.h"
#include "HSPAFDDownlinkRRScheduler.h"
#include "HSPATDDownlinkRRScheduler.h"
#include "HSPATDDownlinkPFScheduler.h"
#include "System.h"
#include "RunParameters.h"
#include "DB.h"
#include "DiversityAntenna.h"
#include "SmartAntenna.h"

namespace LTESim
{

	NodeBMaker::NodeBMaker(void)
	{
	}

	NodeBMaker::~NodeBMaker(void)
	{
	}

	//完成NodeB的初始化并获得当前NodeB
	NodeB::PTR NodeBMaker::CreateNodeB( int nbid, double posx, double posy )
	{
		const std::string & TxAntenna = System::GetCurrentSystem()->GetConfig().GetString("NodeBTXAntenna");
		int antennaPort = System::GetCurrentSystem()->GetConfig().GetInt("NodeBAntennaPort");
		double antennaGain = dBToReal(System::GetCurrentSystem()->GetConfig().GetDouble("Node_B_Antenna_Gain(db)"));
		shared_ptr<Antenna> pAntenna;
		if (TxAntenna == "Diversity")
		{
			pAntenna.reset(new DiversityAntenna(antennaGain));
			pAntenna->SetAntennaPortNum(antennaPort);
		}
		if (TxAntenna == "SmartAntenna")
		{
			pAntenna.reset(new SmartAntenna(antennaGain));
		}
		int RBPerSlot = System::GetCurrentSystem()->GetConfig().GetInt("RB_Number");
		double NodeBMaxTxPower = dBToReal(System::GetCurrentSystem()->GetConfig().GetDouble("Node_B_Maximum_Transmission_Power(dbm)"));
		NodeB::PTR pNodeB(new NodeB(nbid,POS(posx, posy), RBPerSlot, NodeBMaxTxPower, pAntenna));

		//设置下行时域调度器
		const std::string & scheduler = System::GetCurrentSystem()->GetConfig().GetString("TDScheduling_Algorithm");
		if( scheduler == std::string("PF"))
		{
			shared_ptr<TDDownlinkScheduler> pTDDLScheduler(new TDPFDownlinkScheduler(pNodeB));
			pNodeB->SetTDDLScheduler(pTDDLScheduler);
		}
		else if ( scheduler == std::string("RR") )
		{
			shared_ptr<TDDownlinkScheduler> pTDDLScheduler(new TDRRDownlinkScheduler(pNodeB));
			pNodeB->SetTDDLScheduler(pTDDLScheduler);
		}
		else if ( scheduler == std::string("maxC/I") )
		{
			shared_ptr<TDDownlinkScheduler> pTDDLScheduler(new TDmaxC2IDownlinkScheduler(pNodeB));
			pNodeB->SetTDDLScheduler(pTDDLScheduler);
		}
		else
		{
			throw std::invalid_argument("Bad scheduling algorithm");
		}
		//设置下行频域调度器
		const std::string & fdscheduler = System::GetCurrentSystem()->GetConfig().GetString("FDScheduling_Algorithm");
		if( fdscheduler == std::string("PF"))
		{
			shared_ptr<FDDownlinkScheduler> pFDDLScheduler(new FDPFDownlinkScheduler(pNodeB));
			pNodeB->SetFDDLScheduler(pFDDLScheduler);
		}
		else if ( fdscheduler == std::string("RR") )
		{
			shared_ptr<FDDownlinkScheduler> pFDDLScheduler(new FDRRDownlinkScheduler(pNodeB));
			pNodeB->SetFDDLScheduler(pFDDLScheduler);
		}
		else if ( fdscheduler == std::string("maxC/I") )
		{
			shared_ptr<FDDownlinkScheduler> pFDDLScheduler(new FDmaxC2IDownlinkScheduler(pNodeB));
			pNodeB->SetFDDLScheduler(pFDDLScheduler);
		}
		else
		{
			throw std::invalid_argument("Bad scheduling algorithm");
		}

		//返回当前的NodeB
		return pNodeB;
	}

	HSPANodeB::PTR NodeBMaker::CreateHSPANodeB(int iNBID, double dPosX, double dPoxY)
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		shared_ptr<Antenna> pAntenna;
		const std::string& szAntennaType = pSystem->GetConfig().GetString("HSPA_NodeB_Antenna_Type");
		int iAntennaPortNum = pSystem->GetConfig().GetInt( "HSPA_NodeB_Antenna_Port_Num" );
		double dAntennaGain = dBToReal( pSystem->GetConfig().GetDouble("HSPA_NodeB_Antenna_Gain") );
		if ( szAntennaType == "Diversity" )
		{
			pAntenna.reset( new DiversityAntenna(dAntennaGain) );
			pAntenna->SetAntennaPortNum( iAntennaPortNum );
		}
		else
			throw std::invalid_argument( "Unknown HSPA NodeB Antenna Type." );

		//得到最大发射功率
		double dMaxTxPower = pSystem->GetConfig().GetDouble( "HSPA_NodeB_MaxTxPower(dB)" );

		HSPANodeB::PTR pNodeB( new HSPANodeB( iNBID, POS( dPosX, dPoxY ), dBToReal( dMaxTxPower), pAntenna ) );
		const std::string& szTDDLSchAlgorithm = pSystem->GetConfig().GetString("HSPA_DL_TDSchedule_Algorithm");
		const std::string& szFDDLSchAlgorithm = pSystem->GetConfig().GetString("HSPA_DL_FDSchedule_Algorithm");
		if ( szTDDLSchAlgorithm == "PF" )
		{
			shared_ptr<ITDScheduler> pTDScheduler( new HSPATDDownlinkPFScheduler( pNodeB ) );
			pNodeB->SetTDDownlinkScheduler( pTDScheduler );
		}
		else if ( szTDDLSchAlgorithm == "RR" )
		{
			shared_ptr<ITDScheduler> pTDScheduler( new HSPATDDownlinkRRScheduler( pNodeB ) );
			pNodeB->SetTDDownlinkScheduler( pTDScheduler );
		}
		else
			throw std::invalid_argument( "未知的时域调度算法类型。" );

		if ( szFDDLSchAlgorithm == "RR" )
		{
			shared_ptr<IFDScheduler> pFDScheduler( new HSPAFDDownlinkRRScheduler( pNodeB ) );
			pNodeB->SetFDDownlinkScheduler( pFDScheduler );
		}
		else
			throw std::invalid_argument( "未知的频域调度算法类型。" );
		
		return pNodeB;
	}

	WiFiAP::PTR NodeBMaker::CreateWiFiAP(int iAPID, double dPosX, double dPosY)
	{		
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		shared_ptr<Antenna> pAntenna;
		const std::string& sAntennaType = pSystem->GetConfig().GetString("WiFi_AP_Antenna_Type");
		int iAntennaPortNum = pSystem->GetConfig().GetInt( "WiFi_AP_Antenna_Port_Num" );
		double dAntennaGain = dBToReal( pSystem->GetConfig().GetDouble("WiFi_AP_Antenna_Gain") );
		if ( sAntennaType == "Diversity" )
		{
			pAntenna.reset( new DiversityAntenna(dAntennaGain) );
			pAntenna->SetAntennaPortNum( iAntennaPortNum );
		}
		else
			throw std::invalid_argument( "Unknown WiFi AP Antenna Type." );

		//得到最大发射功率
		double dMaxTxPower = pSystem->GetConfig().GetDouble( "WiFi_AP_MaxTxPower(dBm)" );

		WiFiAP::PTR pAP(new WiFiAP(iAPID, POS(dPosX, dPosY), dBToReal( dMaxTxPower), pAntenna ));

		return pAP;

	}

	LAANodeB::PTR NodeBMaker::CreateLAANodeB(int iLAANBID, double posX, double posY)
	{		
		const std::string & TxAntenna = System::GetCurrentSystem()->GetConfig().GetString("NodeBTXAntenna");
		int antennaPort = System::GetCurrentSystem()->GetConfig().GetInt("NodeBAntennaPort");
		double antennaGain = dBToReal(System::GetCurrentSystem()->GetConfig().GetDouble("Node_B_Antenna_Gain(db)"));
		shared_ptr<Antenna> pAntenna;
		if (TxAntenna == "Diversity")
		{
			pAntenna.reset(new DiversityAntenna(antennaGain));
			pAntenna->SetAntennaPortNum(antennaPort);
		}
		if (TxAntenna == "SmartAntenna")
		{
			pAntenna.reset(new SmartAntenna(antennaGain));
		}

		//int RBPerSlot = System::GetCurrentSystem()->GetConfig().GetInt("RB_Number");

		double NodeBMaxTxPower = dBToReal(System::GetCurrentSystem()->GetConfig().GetDouble("LAA_NB_MaxTxPower(dBm)"));

		LAANodeB::PTR pNodeB(new LAANodeB(iLAANBID,POS(posX, posY),/* RBPerSlot,*/ NodeBMaxTxPower, pAntenna));

		////设置下行时域调度器
		//const std::string & scheduler = System::GetCurrentSystem()->GetConfig().GetString("TDScheduling_Algorithm");
		//if( scheduler == std::string("PF"))
		//{
		//	shared_ptr<TDDownlinkScheduler> pTDDLScheduler(new TDPFDownlinkScheduler(pNodeB));
		//	pNodeB->SetTDDLScheduler(pTDDLScheduler);
		//}
		//else if ( scheduler == std::string("RR") )
		//{
		//	shared_ptr<TDDownlinkScheduler> pTDDLScheduler(new TDRRDownlinkScheduler(pNodeB));
		//	pNodeB->SetTDDLScheduler(pTDDLScheduler);
		//}
		//else if ( scheduler == std::string("maxC/I") )
		//{
		//	shared_ptr<TDDownlinkScheduler> pTDDLScheduler(new TDmaxC2IDownlinkScheduler(pNodeB));
		//	pNodeB->SetTDDLScheduler(pTDDLScheduler);
		//}
		//else
		//{
		//	throw std::invalid_argument("Bad scheduling algorithm");
		//}
		////设置下行频域调度器
		//const std::string & fdscheduler = System::GetCurrentSystem()->GetConfig().GetString("FDScheduling_Algorithm");
		//if( fdscheduler == std::string("PF"))
		//{
		//	shared_ptr<FDDownlinkScheduler> pFDDLScheduler(new FDPFDownlinkScheduler(pNodeB));
		//	pNodeB->SetFDDLScheduler(pFDDLScheduler);
		//}
		//else if ( fdscheduler == std::string("RR") )
		//{
		//	shared_ptr<FDDownlinkScheduler> pFDDLScheduler(new FDRRDownlinkScheduler(pNodeB));
		//	pNodeB->SetFDDLScheduler(pFDDLScheduler);
		//}
		//else if ( fdscheduler == std::string("maxC/I") )
		//{
		//	shared_ptr<FDDownlinkScheduler> pFDDLScheduler(new FDmaxC2IDownlinkScheduler(pNodeB));
		//	pNodeB->SetFDDLScheduler(pFDDLScheduler);
		//}
		//else
		//{
		//	throw std::invalid_argument("Bad scheduling algorithm");
		//}

		//返回当前的NodeB
		return pNodeB;

	}
}
