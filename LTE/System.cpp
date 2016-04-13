#include "commondef.h"
#include <map>
#include <iomanip>
#include <boost/foreach.hpp>
#include <numeric>
//#include <algorithm>
#include "System.h"
#include "Configuration.h"
#include "ClockManager.h"
#include "ClockReporter.h"
#include "RandomManager.h"
#include "RunParameters.h"
#include "TableReader.h"
#include "MCSTable.h"
#include "AMCTable.h"
#include "DB.h"
#include "NodeBMaker.h"
#include "UEMaker.h"
#include "HighLayerControllerMaker.h"
#include "NodeB.h"
#include "HSPANodeB.h"
#include "UE.h"
#include "HSPAUE.h"
#include "AWGNFading.h"
#include "SCMFading.h"
#include "RayleighFading.h"
#include "SlowFading.h"
#include "Pathloss.h"
#include "Decisioner.h"
#include "Map.h"
#include "ServiceStatistic.h"
#include "RayleighFading.h"
#include "SystemResult.h"
#include "DLIorCalculator.h"
#include "DLIocCalculator.h"
#include "SmartAntenna.h"
#include "DiversityAntenna.h"
#include "SINRCalculator.h"
#include "HSPAFading.h"
#include "HSPAAWGNFading.h"
#include "HSPASCMFading.h"
#include "HSPARayleighFading.h"
#include "SimLog.h"
#include "ServiceComposite.h"
#include "WiFiUE.h"
#include "WiFiAP.h"
#include "WiFiRiceFading.h"
#include "WiFiShadowFading.h"
#include "WiFiSINRCalculator.h"
#include "LTEWiFiUE.h"

#include "LAAUE.h"
#include "LAANodeB.h"
#include "LAARiceFading.h"
#include "LAAShadowFading.h"
#include "LAASINRCalculator.h"

namespace LTESim
{
#pragma region ������������

	System::System(const shared_ptr<const RunParameters>& parameters)
		:m_uemaker(new UEMaker), 
		m_nbmaker(new NodeBMaker), 
		m_pControllerMaker( new HighLayerControllerMaker() ),
		m_pParameters(parameters),
		m_dWiFiThermalDensity(0),
		m_dLAAThermalDensity(0),
		m_ThermalDensity(0),
		m_RBBandWidth(0), 
		m_dWiFiBandWidth(0),
		m_dLAABandWidth(0),
		m_pThrStatistic( new ThroughputStatistic() ),
		m_pHSPAThrStatistic( new ThroughputStatistic() ),
		m_pDualModeThrStatisticLTEPart( new ThroughputStatistic() ),
		m_pDualModeThrStatisticHSPAPart( new ThroughputStatistic() ),
		m_pWiFiThrStatistic( new ThroughputStatistic() ),
		m_pLAAThrStatistic( new ThroughputStatistic() ),
		m_pLTEWiFiThrStatisticLTEPart( new ThroughputStatistic() ),
		m_pLTEWiFiThrStatisticWiFiPart( new ThroughputStatistic() ),
		m_pLTEPacketTimeStatistic( new PacketTimeStatistic() ),
		m_pHSPAPacketTimeStatistic( new PacketTimeStatistic() ),
		m_pWiFiPacketTimeStatistic( new PacketTimeStatistic() ),
		m_pLAAPacketTimeStatistic( new PacketTimeStatistic() ),
		m_pDualPacketTimeStatistic( new PacketTimeStatistic() ),
		m_pLTEWiFiPacketTimeStatisticLTEPart( new PacketTimeStatistic() ),
		m_pLTEWiFiPacketTimeStatisticWiFiPart( new PacketTimeStatistic() ),
		m_pLTETransIntervalStatistic( new TransIntervalStatistic() ),
		m_pWiFiTransIntervalStatistic( new TransIntervalStatistic() ),
		m_pLAATransIntervalStatistic( new TransIntervalStatistic() ),
		m_pLTEPacketLossStatistic( new PacketLossStatistic() ),
		m_pWiFiPacketLossStatistic( new PacketLossStatistic() ),
		m_pLAAPacketLossStatistic( new PacketLossStatistic() ),
		m_pLTEWiFiPacketLossStatisticLTEPart( new PacketLossStatistic() ),
		m_pLTEWiFiPacketLossStatisticWiFiPart( new PacketLossStatistic() ),
		m_pLTEChannelUtilizationStatistic( new ChannelUtilizationStatistic() ),
		m_pWiFiChannelUtilizationStatistic( new ChannelUtilizationStatistic() ),
		m_pLAAChannelUtilizationStatistic( new ChannelUtilizationStatistic() ),
		m_pLTEWiFiChannelUtilizationStatisticLTEPart( new ChannelUtilizationStatistic() ),
		m_pLTEWiFiChannelUtilizationStatisticWiFiPart( new ChannelUtilizationStatistic() ),
		m_pResult(new SystemResult)
		//,m_fout("../Output/C2I-AverThr.txt")
	{

	}


	System::~System(void)
	{

	}


#pragma endregion

#pragma region ������
#pragma region ��ʼ��
	//ϵͳ��ʼ�������������վ���ƶ�̨
	bool System::Init()
	{

		//����ȫ�ַ��ʵ�Ϊ��System��֮���GetCurrentSystem���ر�System
        UpdateCurrentSystem();
    
        //ʱ������
		ClockManager::Instance().Reset();

        //����MCS��AMC��
		InitTables();
		//��ʼ������ģ��
		InitMap();

		//��ʼ��С��������Ҫ���ʼ�������վ
		InitNodeB();

		//��ʼ��С���ڵ�UE
		InitUE();

		//��ʼ���߲������
		InitHighLayerController();
     
        //��ʼ��·��
        InitPathLoss();

		//WiFiϵͳ����
		m_dWiFiBandWidth = GetConfig().GetDouble("WiFi_BandWidth(MHz)") * 1e6;
		//WiFi�����ܶ�
		m_dWiFiThermalDensity = dBToReal(GetConfig().GetDouble("WiFi_UE_Thermal_Density(dBm)"));

		m_dLAABandWidth = GetConfig().GetDouble("WiFi_BandWidth(MHz)") * 1e6;
		//WiFi�����ܶ�
		m_dLAAThermalDensity = dBToReal(GetConfig().GetDouble("WiFi_UE_Thermal_Density(dBm)"));

        //��ʼ��˥��  ����˥��ʼ��  �ĵ�����
        InitSlowFading();
        InitFastFading();	 

	
        //��ʼ���������������
        InitArray();

		//��ʼ���о���
		InitDecisioner();

		//ϵͳ����
		m_RBBandWidth = GetConfig().GetDouble("RB_Bandwidth(KHz)") * 1000;

        //�����ܶ�
		m_ThermalDensity = dBToReal(GetConfig().GetDouble("Thermal_Density(dbm)"));	

        //ϵͳ����ʱ����0
		m_time = 0;
 
        //��ʼ�����ż�����
        InitSINRCalculator();
		
		if (0) 
		{	//���WiFiС��SINR�ֲ�������
			std::ofstream osinrfile;
			osinrfile.open( "../Output/WiFiSINR.txt", std::ios::trunc );
			WiFiSINRCalculator::Instance() -> OutputAllSINR( osinrfile );
		}

        return true;
	}

	//����MCS���AMC����Ҫ����fading��Ϊ�����ܿ����޸ĳ�static������������Ӧ�ı��޸ĳ�static��Ա����
	void System::InitTables()
	{
		std::string TableLocation = GetConfig().GetString("BLER_TABLE_LOCATION");
		size_t row = 20, col =2;
		std::vector<shared_ptr<const TableReader> > TableReaders;
#pragma region QPSK
		shared_ptr<TableReader> pTableReader(new TextTableReader(TableLocation+"qpsk_1_5.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qpsk_1_3.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qpsk_1_2.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qpsk_2_3.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qpsk_3_4.txt", row, col));
		TableReaders.push_back(pTableReader);
#pragma endregion 

#pragma region QAM16
		pTableReader.reset(new TextTableReader(TableLocation + "qam16_1_2.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam16_2_3.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam16_3_4.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam16_4_5.txt", row, col));
		TableReaders.push_back(pTableReader);
#pragma endregion 

#pragma region QAM64
		pTableReader.reset(new TextTableReader(TableLocation + "qam64_3_5.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam64_2_3.txt", row, col));
		TableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam64_4_5.txt", row, col));
		TableReaders.push_back(pTableReader);
#pragma endregion 

		m_pMCSTables.reset(new MCSTable(TableReaders));
		
#pragma region HSPAMCStable
		row = 12; col = 2;
		std::vector<shared_ptr<const TableReader> > HSPATableReaders;
		pTableReader.reset(new TextTableReader(TableLocation + "MCS1.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS2.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS3.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS4.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS5.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS6.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS7.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS8.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS9.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS10.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "MCS11.txt", row, col));
		HSPATableReaders.push_back(pTableReader);
#pragma endregion 

		m_pHSPAMCSTables.reset(new MCSTable(HSPATableReaders));

#pragma region WiFiMCSTables
		row = 15; col = 2;
		std::vector<shared_ptr<const TableReader> > WiFiTableReaders;
		pTableReader.reset(new TextTableReader(TableLocation + "802.11n 2x2 1Stream Div/MCS0.txt", row, col));
		WiFiTableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "802.11n 2x2 1Stream Div/MCS1.txt", row, col));
		WiFiTableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "802.11n 2x2 1Stream Div/MCS2.txt", row, col));
		WiFiTableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "802.11n 2x2 1Stream Div/MCS3.txt", row, col));
		WiFiTableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "802.11n 2x2 1Stream Div/MCS4.txt", row, col));
		WiFiTableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "802.11n 2x2 1Stream Div/MCS5.txt", row, col));
		WiFiTableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "802.11n 2x2 1Stream Div/MCS6.txt", row, col));
		WiFiTableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "802.11n 2x2 1Stream Div/MCS7.txt", row, col));
		WiFiTableReaders.push_back(pTableReader);
#pragma endregion

		m_pWiFiMCSTables.reset(new MCSTable(WiFiTableReaders));

#pragma region LAAMCStables
		row = 20; col = 2;
		std::vector<shared_ptr<const TableReader> > LAATableReaders;
//QPSK
		pTableReader.reset(new TextTableReader(TableLocation+"qpsk_1_5.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qpsk_1_3.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qpsk_1_2.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qpsk_2_3.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qpsk_3_4.txt", row, col));
		LAATableReaders.push_back(pTableReader);

//QAM16
		pTableReader.reset(new TextTableReader(TableLocation + "qam16_1_2.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam16_2_3.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam16_3_4.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam16_4_5.txt", row, col));
		LAATableReaders.push_back(pTableReader);

// QAM64
		pTableReader.reset(new TextTableReader(TableLocation + "qam64_3_5.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam64_2_3.txt", row, col));
		LAATableReaders.push_back(pTableReader);
		pTableReader.reset(new TextTableReader(TableLocation + "qam64_4_5.txt", row, col));
		LAATableReaders.push_back(pTableReader);

#pragma endregion
		m_pLAAMCSTables.reset(new MCSTable(TableReaders));

#pragma region AMC��
		const double TargetBLER = GetConfig().GetDouble("AMC_BLER_TARGET");
		const double HSPATargetBLER = GetConfig().GetDouble("HSPA_AMC_BLER_TARGET");
		const double WiFiTargetBLER = GetConfig().GetDouble("WiFi_AMC_BLER_TARGET");
		const double LAATargetBLER = GetConfig().GetDouble("LAA_AMC_BLER_TARGET");
		m_pAMCTable.reset(new AMCTable(*m_pMCSTables, TargetBLER, SYSTEM_LTE));
		m_pHSPAAMCTable.reset(new AMCTable(*m_pHSPAMCSTables, HSPATargetBLER, SYSTEM_HSPA));
		m_pWiFiAMCTable.reset(new AMCTable(*m_pWiFiMCSTables, WiFiTargetBLER, SYSTEM_WIFI));
		m_pLAAAMCTable.reset(new AMCTable(*m_pLAAMCSTables, LAATargetBLER, SYSTEM_LAA));

#pragma endregion AMC��
	}

	void System::InitDecisioner()
	{
		//ȫ�ֿ�����һ���о���������Ӱ���о����
		m_pLTEDecisioner.reset(new DecisionerImp(GetMCSTables()));

		m_pHSPADecisioner.reset(new DecisionerImp(GetHSPAMCSTables()));

		m_pWiFiDecisioner.reset(new DecisionerImp(GetWiFiMCSTables()));

		m_pLAADecisioner.reset(new DecisionerImp(GetLAAMCSTables()));
	}

	void System::InitHighLayerController()
	{
		//����С���뾶
		double  iNBRadius = GetConfig().GetInt("Inter_Site_Distance") * std::sqrt(3.0) / 3.0;
		//ÿһ��С������һ���߲�����������Ը߲��������������NodeB��������ȣ�
		//��������ͬID��Ӧ��eNodeB������Ҳһ��
		int iControllerNum = GetConfig().GetInt("NodeB_Number");
		for ( int iIndex = 0; iIndex < iControllerNum; ++iIndex )
		{
			AddHighLayerController( m_pControllerMaker->CreateHighLayerController( iIndex,
																																iNBRadius * m_pMap->GetNBPos(iIndex).GetX(), 
																																iNBRadius * m_pMap->GetNBPos(iIndex).GetY() ) );
		}
	}

	void System::InitMap()
	{
		//��ʼ��7С��ģ��
//		m_pMap = shared_ptr<Map>(new SevenCells);
		//WiFi���õ���Բ��С��
		m_pWiFiMap = shared_ptr<Map>(new Indoor);
		m_pLAAMap = shared_ptr<Map>(new CircleCell);
	}

	void System::InitNodeB()
	{
		//��վ����, ���ڿ��ǵ����ʱLTE��HSPA�Ļ�վ�ǹ�վ��ַ�ģ����Ի�վ��Ŀ��λ�ö�һ��
		int iNodeBNumber = GetConfig().GetInt("NodeB_Number");
		//����С���뾶
		double  iNBRadius = GetConfig().GetInt("Inter_Site_Distance") * std::sqrt(3.0) / 3.0;
		double	dNBAPDistance = GetConfig().GetInt("NodeB_AP_Distance");	//��վ��WiFi AP�ľ���

		//��ʼ��NodeBNumber����վ
		for (int i = 0; i < iNodeBNumber ; ++i)
		{
			double x = iNBRadius * ( m_pMap -> GetNBPos(i).GetX() );
			double y = iNBRadius * ( m_pMap -> GetNBPos(i).GetY() );
			AddNodeB( m_nbmaker -> CreateNodeB( i, x, y ) );
			AddHSPANodeB( m_nbmaker -> CreateHSPANodeB( i + iNodeBNumber, x, y ) );
		}
		//��ʼ��Indoor����
        int iWiFiNumber = GetConfig().GetInt("WiFi_Number");
        //�õ�С���뾶
        double iIndoorRadius = GetConfig().GetInt("Circle_Cell_Radius");
        //��ʼ��wifi��LAA�ڵ�(LAAδ��!)
        for(int i=0;i<iWiFiNumber;++i)
        {
            double x = m_pWiFiMap ->GetNBPos(i).GetX();
            double y = m_pWiFiMap ->GetNBPos(i).GetY();
			double xx = m_pLAAMap ->GetNBPos(i).GetX();
			double yy = m_pLAAMap ->GetNBPos(i).GetY();
            AddWiFiAP(m_nbmaker ->CreateWiFiAP(i,x,y));
			AddLAANB(m_nbmaker ->CreateLAANodeB(i,xx,yy));//����λ�û���Ҫ��һ��
        }

		
	}

	void System::InitUE()
	{
		RandomManager& randman = RandomManager::GetRandomManager();
		int iLTEUENumber = GetConfig().GetInt("LTE_UE_Number");					//LTE UE�ĸ���
		int iLTEUENumInHotspot = iLTEUENumber * GetConfig().GetDouble("P_LTEUEInHotspot"); //LTEUE�������ȵ���ĸ���
		int iHSPAUENumber = GetConfig().GetInt( "HSPA_UE_Number" );		//HSPA UE�ĸ���
		int iDualModeUENumber = GetConfig().GetInt( "DualMode_UE_Number" );
		int iLTEWiFiUENumber = GetConfig().GetInt("LTEWiFi_UE_Number");
		int iNodeBNumber = GetConfig().GetInt("NodeB_Number");				//��վ�ĸ���
		int iAPNumber = iNodeBNumber;
		double  dNBRadius = GetConfig().GetInt("Inter_Site_Distance") * std::sqrt(3.0) / 3.0;	//С���뾶
		int iNodeBAPDis = GetConfig().GetInt("NodeB_AP_Distance");				//NodeB��AP�ľ���

		bool bFTPVoIPCoexist = GetConfig().GetBool("FTP_VoIP_Coexist");
		int iFTPLTEUENum = floor( GetConfig().GetInt("LTE_UE_Number")*GetConfig().GetDouble("FTP_UE_Percentage")+0.5 );//��������ȡ��
		int iVoIPLTEUENum = floor( GetConfig().GetInt("LTE_UE_Number")*GetConfig().GetDouble("VoIP_UE_Percentage")+0.5 );
		
		int iWiFiNumber = GetConfig().GetInt("WiFi_Number");
		double	dWiFiRadius = GetConfig().GetInt("Circle_Cell_Radius");
		int iWiFiUENumber = GetConfig().GetInt("WiFi_UE_Number");	//WiFi UE�ĸ���

		int iLAANumber = GetConfig().GetInt("LAA_Number");
		double	dLAARadius = GetConfig().GetInt("Circle_Cell_Radius");
		int iLAAUENumber = GetConfig().GetInt("LAA_UE_Number");
		
		const Map& instWiFiMap = GetWiFiMap();
		const Map& instLAAMap = GetLAAMap();
		//const Map& instIndoorMap = GetIndoorMap();
		//���û����ȵķ��䵽ÿ����վ����ÿ����վ�ڵ������پ��ȷֲ���ʵ����UE�ľ��ȷֲ�
		int iUEIDBase = 0;
#pragma region �ж�û�к��վ�Ͳ�ִ��
		if (iNodeBNumber > 0)
		{
		const Map& instMap = GetMap();
		//LTE�û��ĳ�ʼ��
		for (int iUEIndex = 0; iUEIndex < iLTEUENumber; iUEIndex++)
		{
			int AssociateNBID = iUEIndex % iNodeBNumber;
            POS relativePOS;
            bool RegeneratePos = false;
            do 
            {   
                RegeneratePos = false;
                relativePOS = instMap.GenerateUEPOS(dNBRadius);	//UE��������ڻ�վ������
				POS realPos(relativePOS.GetX() + GetNodeB(AssociateNBID)->GetPos().GetX(),
									relativePOS.GetY() + GetNodeB(AssociateNBID)->GetPos().GetY());
                double RefDistance = Distance(GetNodeB(AssociateNBID)->GetPos(), realPos);
                BOOST_FOREACH(const NBLIST::value_type& NodeBIter, GetNBList())
                {
                    shared_ptr<NodeB> pNodeB = NodeBIter.second;
                    if (pNodeB->GetID() == AssociateNBID)
                    {
                        continue;
                    }
					//���ﻹ��Ҫ����Ƿ���뱾С��������ģ����������С���ľ������������Ҫ���½�������λ��
					//��������֤IDΪi��UE���϶��Ǿ���i%NodeBNumber��NodeB�������
                    double distanceToNeighborNodeB = Distance(pNodeB->GetPos(), realPos);
                    if (distanceToNeighborNodeB < RefDistance)
                    {
						RegeneratePos = true;
                        break;
                    }
                }
            } while (RegeneratePos);

			if ( iUEIndex >= iLTEUENumber-iLTEUENumInHotspot )
			{
				relativePOS = instWiFiMap.GenerateUEPOS(dWiFiRadius);
				relativePOS = POS( relativePOS.GetX()+iNodeBAPDis, relativePOS.GetY() );
			}

			string servicetype;
			if ( iFTPLTEUENum ) 
			{
				servicetype="FTP_Model_2";
				iFTPLTEUENum--;
			}
			else if ( iVoIPLTEUENum ) 
			{
				servicetype="VoIP";
				iVoIPLTEUENum--;
			}
            
			AddLTEUE( m_uemaker->CreateLTEUE(iUEIndex + iUEIDBase
				,  relativePOS.GetX() + GetNodeB(AssociateNBID)->GetPos().GetX()
				,  relativePOS.GetY() + GetNodeB(AssociateNBID)->GetPos().GetY()
				, bFTPVoIPCoexist, servicetype) );
		}

#pragma region ����UE
		//HSPA UE�ĳ�ʼ��
		iUEIDBase += iLTEUENumber;
		for( int iUEIndex = 0; iUEIndex < iHSPAUENumber; iUEIndex++ )
		{
			int AssociateNBID = iUEIndex % iNodeBNumber;
			POS relativePOS;
			bool RegeneratePos = false;
			do 
			{   
				RegeneratePos = false;
				relativePOS = instMap.GenerateUEPOS(dNBRadius);	//UE��������ڻ�վ������
				POS realPos(relativePOS.GetX() + GetNodeB(AssociateNBID)->GetPos().GetX(),
									relativePOS.GetY() + GetNodeB(AssociateNBID)->GetPos().GetY());
				double RefDistance = Distance(GetNodeB(AssociateNBID)->GetPos(), realPos);
				BOOST_FOREACH(const NBLIST::value_type& NodeBIter, GetNBList())
				{
					shared_ptr<NodeB> pNodeB = NodeBIter.second;
					if (pNodeB->GetID() == AssociateNBID)
					{
						continue;
					}
					//���ﻹ��Ҫ����Ƿ���뱾С��������ģ����������С���ľ������������Ҫ���½�������λ��
					//��������֤IDΪi��UE���϶��Ǿ���i%NodeBNumber��NodeB�������
					double distanceToNeighborNodeB = Distance(pNodeB->GetPos(), realPos);
					if (distanceToNeighborNodeB < RefDistance)
					{
						RegeneratePos = true;
						break;
					}
				}
			} while (RegeneratePos);

			AddHSPAUE(m_uemaker->CreateHSPAUE(iUEIndex + iUEIDBase
				,  relativePOS.GetX() + GetNodeB(AssociateNBID)->GetPos().GetX()
				,  relativePOS.GetY() + GetNodeB(AssociateNBID)->GetPos().GetY()) );
		}

		//Dual UE�ĳ�ʼ��
		iUEIDBase += iHSPAUENumber;
		for ( int iUEIndex = 0; iUEIndex < iDualModeUENumber; iUEIndex++ )
		{
			int AssociateNBID = iUEIndex % iNodeBNumber;
			POS relativePOS;
			bool RegeneratePos = false;
			do 
			{   
				RegeneratePos = false;
				relativePOS = instMap.GenerateUEPOS(dNBRadius);	//UE��������ڻ�վ������
				POS realPos(relativePOS.GetX() + GetNodeB(AssociateNBID)->GetPos().GetX(),
									relativePOS.GetY() + GetNodeB(AssociateNBID)->GetPos().GetY());
				double RefDistance = Distance(GetNodeB(AssociateNBID)->GetPos(), realPos);
				BOOST_FOREACH(const NBLIST::value_type& NodeBIter, GetNBList())
				{
					shared_ptr<NodeB> pNodeB = NodeBIter.second;
					if (pNodeB->GetID() == AssociateNBID)
					{
						continue;
					}
					//���ﻹ��Ҫ����Ƿ���뱾С��������ģ����������С���ľ������������Ҫ���½�������λ��
					//��������֤IDΪi��UE���϶��Ǿ���i%NodeBNumber��NodeB�������
					double distanceToNeighborNodeB = Distance(pNodeB->GetPos(), realPos);
					if (distanceToNeighborNodeB < RefDistance)
					{
						RegeneratePos = true;
						break;
					}
				}
			} while (RegeneratePos);

			AddDualModeLTEHSPAUE(	m_uemaker->CreateDualModeLTEHSPAUE(
															iUEIndex + iUEIDBase,  
															relativePOS.GetX() + GetNodeB(AssociateNBID)->GetPos().GetX(),  
															relativePOS.GetY() + GetNodeB(AssociateNBID)->GetPos().GetY()		)	);		
		}

		//LTEWiFi UE�ĳ�ʼ��
		iUEIDBase += iDualModeUENumber;
		for ( int iUEIndex = 0; iUEIndex < iLTEWiFiUENumber; iUEIndex++ )
		{
			int AssociateAPID = iUEIndex % iAPNumber;
			POS relativePOS = instWiFiMap.GenerateUEPOS(dWiFiRadius);	//UE���������AP������
			POS realPos( relativePOS.GetX() + GetWiFiAPFromOffsetID(AssociateAPID)->GetPos().GetX(),
					relativePOS.GetY() + GetWiFiAPFromOffsetID(AssociateAPID)->GetPos().GetY() );

			AddLTEWiFiUE( m_uemaker->CreateLTEWiFiUE( iUEIndex + iUEIDBase, realPos.GetX(), realPos.GetY() ) );		
		}

#pragma endregion ����UE

		//WiFiUE��ʼ��
		iUEIDBase += iLTEWiFiUENumber;
		bool bWiFiFTPVoIPCoexist = GetConfig().GetBool("WiFi_FTP_VoIP_Coexist");
		int iFTPWiFiUENum = floor( GetConfig().GetInt("WiFi_UE_Number")*GetConfig().GetDouble("WiFi_FTP_UE_Percentage")+0.5 );//��������ȡ��
		int iVoIPWiFiUENum = floor( GetConfig().GetInt("WiFi_UE_Number")*GetConfig().GetDouble("WiFi_VoIP_UE_Percentage")+0.5 );
		for (int iUEIndex = 0; iUEIndex < iWiFiUENumber; iUEIndex++)
		{
			int AssociateAPID = iUEIndex % iAPNumber;
			POS relativePOS = instWiFiMap.GenerateUEPOS(dWiFiRadius);	//UE���������AP������
			POS realPos( relativePOS.GetX() + GetWiFiAPFromOffsetID(AssociateAPID)->GetPos().GetX(),
				relativePOS.GetY() + GetWiFiAPFromOffsetID(AssociateAPID)->GetPos().GetY() );

			string servicetype;
			if ( iFTPWiFiUENum ) 
			{
				servicetype="FTP_Model_2";
				iFTPWiFiUENum--;
			}
			else if ( iVoIPWiFiUENum ) 
			{
				servicetype="VoIP";
				iVoIPWiFiUENum--;
			}

			AddWiFiUE( m_uemaker->CreateWiFiUE( iUEIndex + iUEIDBase, realPos.GetX(), realPos.GetY(), bWiFiFTPVoIPCoexist, servicetype ) );
		}

		//����AP��4������UE��ID�ֱ�Ϊ0~3
		iUEIDBase += iWiFiUENumber;
		int num = 0;
		for ( WiFiAPLIST::iterator iter = m_WiFiAPList.begin() ; iter != m_WiFiAPList.end(); iter++, num++ )
		{
			WiFiAPPTR pAP = iter->second;
			POS APPos = pAP -> GetPos();
			AddWiFiUE( m_uemaker->CreateWiFiUE( num + iUEIDBase, APPos.GetX(), APPos.GetY(), true, "VoIP" ) );
		}
		}
#pragma endregion
		//WiFiUE��ʼ��
		bool bWiFiFTPVoIPCoexist = GetConfig().GetBool("WiFi_FTP_VoIP_Coexist");
		int iFTPWiFiUENum = GetConfig().GetInt("WiFi_UE_Number");//*GetConfig().GetDouble("WiFi_FTP_UE_Percentage")+0.5 );//��������ȡ��
		int iVoIPWiFiUENum = 0;//floor( GetConfig().GetInt("WiFi_UE_Number")*GetConfig().GetDouble("WiFi_VoIP_UE_Percentage")+0.5 );
		for (int iUEIndex = 0; iUEIndex < iWiFiUENumber; iUEIndex++)
		{
			int AssociateAPID = iUEIndex % iWiFiNumber;
			POS relativePOS = instWiFiMap.GenerateUEPOS(dWiFiRadius);	//UE���������AP������
			POS realPos( relativePOS.GetX() + GetWiFiAP(AssociateAPID)->GetPos().GetX(),
				relativePOS.GetY() + GetWiFiAP(AssociateAPID)->GetPos().GetY() );

			string servicetype;
			if ( iFTPWiFiUENum ) 
			{
				servicetype="FTP_Model_2";
				iFTPWiFiUENum--;
			}
			else if ( iVoIPWiFiUENum ) 
			{
				servicetype="VoIP";
				iVoIPWiFiUENum--;
			}

			AddWiFiUE( m_uemaker->CreateWiFiUE( iUEIndex + iUEIDBase, realPos.GetX(), realPos.GetY(), bWiFiFTPVoIPCoexist, servicetype ) );
		}

		//����AP������UE
		//iUEIDBase += iWiFiUENumber;
		//int num = 0;
		//for ( WiFiAPLIST::iterator iter = m_WiFiAPList.begin() ; iter != m_WiFiAPList.end(); iter++, num++ )
		//{
		//	WiFiAPPTR pAP = iter->second;
		//	POS APPos = pAP -> GetPos();
		//	AddWiFiUE( m_uemaker->CreateWiFiUE( num + iUEIDBase, APPos.GetX(), APPos.GetY(), true, "VoIP" ) );
		//}


		//LAAUE��ʼ��
		iUEIDBase += iWiFiUENumber;
		bool bLAAFTPVoIPCoexist = GetConfig().GetBool("LAA_FTP_VoIP_Coexist");
		int iFTPLAAUENum = GetConfig().GetInt("LAA_UE_Number");//*GetConfig().GetDouble("LAA_FTP_UE_Percentage")+0.5 );//��������ȡ��
		int iVoIPLAAUENum = 0;//floor( GetConfig().GetInt("LAA_UE_Number")*GetConfig().GetDouble("LAA_VoIP_UE_Percentage")+0.5 );
		for (int iUEIndexa = 0; iUEIndexa < iLAAUENumber; iUEIndexa++)
		{
			int AssociateAPID = iUEIndexa % iLAANumber;
			POS relativePOS = instLAAMap.GenerateUEPOS(dLAARadius);	//UE���������AP������
			POS realPos( relativePOS.GetX() + GetLAANB(AssociateAPID)->GetPos().GetX(),
				relativePOS.GetY() + GetLAANB(AssociateAPID)->GetPos().GetY() );

			string servicetype;
			if ( iFTPLAAUENum ) 
			{
				servicetype="FTP_Model_2";
				iFTPLAAUENum--;
			}
			else if ( iVoIPLAAUENum ) 
			{
				servicetype="VoIP";
				iVoIPLAAUENum--;
			}

			AddLAAUE( m_uemaker->CreateLAAUE( iUEIndexa + iUEIDBase, realPos.GetX(), realPos.GetY(), bLAAFTPVoIPCoexist, servicetype ) );
		}
		
	//	//����LAA������UE
	//	iUEIDBase += iLAAUENumber;
	//	int numa = 0;
	//	for ( LAANBLIST::iterator iter = m_LAANBList.begin() ; iter != m_LAANBList.end(); iter++, numa++ )
	//	{
	//		LAANBPTR pNB = iter->second;
	//		POS LAANBPos = pNB -> GetPos();
	//		AddLAAUE( m_uemaker->CreateLAAUE( numa + iUEIDBase, LAANBPos.GetX(), LAANBPos.GetY(), true, "VoIP" ) );
	//	}
	}


	void System::InitPathLoss()
	{
		double freq = GetConfig().GetDouble("Carrier_Frequency(MHz)");								//��Ƶ
		//double hNodeB = GetConfig().GetDouble("HEIGHT_OF_Nodeb_ANTENNA(m)");	//��վ�߶�
		//double hUE = GetConfig().GetDouble("HEIGHT_OF_UE_ANTENNA(m)");				//UE�߶�
		//bool CentralCityAdjustment = GetConfig().GetBool("Metropolitan_Area");					//�Ƿ�������
		double wififreq = GetConfig().GetDouble("WiFi_Carrier_Frequency(MHz)");
		double laafreq = GetConfig().GetDouble("LAA_Carrier_Frequency(MHz)");
		m_pLTEPathLoss.reset(new UMa_NLOS(freq));
		//m_pLTEPathLoss.reset(new UMi_NLOS(freq));
		//m_pLTEPathLoss.reset(new Cost231_Hata(freq));
		m_pHSPAPathLoss.reset(new HSPACost231_Hata());
		m_pWiFiPathLoss.reset(new WiFiPathLoss(wififreq));
		m_pLAAPathloss.reset(new LAAPathloss(laafreq));
	}
#pragma region Fading��
    void System::InitFastFading()
    {
		System::PTR pSystem = System::GetCurrentSystem();
		//int iLTEUENum = pSystem->GetConfig().GetInt("LTE_UE_Number");
		//int iHSPAUENum = pSystem->GetConfig().GetInt("HSPA_UE_Number");
		//int iDualUENum = pSystem->GetConfig().GetInt("DualMode_UE_Number");
		//int iWiFiUENum = pSystem->GetConfig().GetInt("WiFi_UE_Number");
		//int iLTEWiFiUENum = pSystem->GetConfig().GetInt("LTEWiFi_UE_Number");
		const std::string& sFastFadingType = pSystem->GetConfig().GetString("FastFadingType");
		if ( sFastFadingType == "AWGN" )
			m_pFastFading.reset( new AWGNFading() );
		else if( sFastFadingType == "SCM" )
			m_pFastFading.reset( new SCMFading() );
		else if( sFastFadingType == "Rayleigh" )
			m_pFastFading.reset( new RayleighFading() );
		else
			throw std::invalid_argument( "Unknown fast fading type" );

		const std::string& sHSPAFastFadingType = pSystem->GetConfig().GetString( "HSPA_FastFading_Type" );
		if ( sHSPAFastFadingType == "AWGN" )
			m_pHSPAFastFading.reset( new HSPAAWGNFading());
		else if ( sHSPAFastFadingType == "SCM" )
			m_pHSPAFastFading.reset( new HSPASCMFading());
		else if ( sHSPAFastFadingType == "Rayleigh" )
			m_pHSPAFastFading.reset( new HSPARayleighFading());
		else
			throw std::invalid_argument( "Unknown HSPA Fast Fading Type" );

		const std::string& sWiFiFastFadingType = pSystem->GetConfig().GetString("WiFi_FastFading_Type");
		if ( sWiFiFastFadingType == "Rice" )
			m_pWiFiFastFading.reset( new WiFiRiceFading() ); 
		else
			throw std::invalid_argument( "Unknown WiFi Fast Fading Type" );

		const std::string& sLAAFastFadingType = pSystem->GetConfig().GetString("WiFi_FastFading_Type");
		if ( sLAAFastFadingType == "Rice" )
			m_pLAAFastFading.reset( new LAARiceFading() ); 
		else
			throw std::invalid_argument( "Unknown LAA Fast Fading Type" );
	}

	void System::InitSlowFading()
	{
		m_pSlowFading.reset(new SlowFading());
		m_pWiFiShadowFading.reset(new WiFiShadowFading());
		m_pLAAShadowFading.reset(new LAAShadowFading());
	}
#pragma endregion Fading��
    void System::InitArray()
    {
        const std::string & TxAntenna = System::GetCurrentSystem()->GetConfig().GetString("NodeBTXAntenna"); 
        
        if (TxAntenna == "SmartAntenna")
        {
            std::vector<double> SAGainArray;
            SAGainArray.reserve(360);
            std::string SAGainFileName = System::GetCurrentSystem()->GetConfig().GetString("SAGainFile");
            ifstream SAGainFile(SAGainFileName.c_str());
            if (!SAGainFile)
            {
                throw std::invalid_argument("Can't open SA Gain File");
            }
            double gain;
            while (!SAGainFile.eof() && (SAGainFile >> gain))
            {
                SAGainArray.push_back(dBToReal(gain));
            }
            SmartAntenna::InitSAGainArray(SAGainArray, GetNBList().size(), GetUEList().size());
        }
    }

    void System::InitSINRCalculator()
    {
        const std::string & TxAntenna = System::GetCurrentSystem()->GetConfig().GetString("NodeBTXAntenna"); 
        shared_ptr<IorCalculator> pIorCalculator;
        shared_ptr<IocCalculator> pIocCalculator;
        if (TxAntenna == "SmartAntenna")
        {
            pIorCalculator.reset(new DLIorCalculator);
            pIocCalculator.reset(new DLSAIocCalculator);
        }
        else if (TxAntenna == "Diversity")
        {
            pIorCalculator.reset(new DLIorCalculator);
            pIocCalculator.reset(new DLIocCalculator);
        }
        SINRCalculator::InitDLCalculator(pIorCalculator, pIocCalculator);
    }
#pragma endregion ��ʼ��

	//���з���timespan��ʱ�䵥λ  �ĵ�������
	void System::Run( int timespan )
	{
		const string& s = GetConfig().GetString("FRAME_CONFIGURATION");
		int iFramePerHighLayerUpdate = GetFramesPerHighLayerClockUpdate();
		for (int iFrame = 0; iFrame < timespan; ++iFrame)
		{
			//ClockManager::Instance().GetFrameClock().Update();
			//ÿiFramePerHighLayerUpdate��HighLayerClock���и���һ��
			//if ( iFrame % iFramePerHighLayerUpdate == 0 )
			//	ClockManager::Instance().GetHighLayerClock().Update();
			//if( iFrame == 0 )
			//	ClockManager::Instance().GetTxClock().Update();  //Ϊ��ִ�зָ����ݵ�����

			for ( int iSubFrame = 0; iSubFrame < GetLTESubFramesPerFrame(); ++iSubFrame )
			{
				//ClockManager::Instance().GetHSPASubFrameClock().Update();

				for ( int iStandardTimeSlice = 0; iStandardTimeSlice < GetStandardTimeSlicesPerHSPASubFrame()/2; ++iStandardTimeSlice )
				{
					std::cout << "\rFrame:"	
						<< ClockManager::Instance().GetFrameClock().GetClock() + 1<< "/" << timespan
						<< "\tTimeSlice:" << std::setw(2) << ClockManager::Instance().GetStandardClock().GetClock() + 1;

					//if ( iStandardTimeSlice % 6 == 0 )
					//	ClockManager::Instance().GetLTESubFrameClock().Update();

					//if ( iStandardTimeSlice % 4 == 0 )
					//{
					//	ClockManager::Instance().GetHSPASlotClock().Update();
					//	m_pHSPAFastFading->Update();
					//}

					//if ( iStandardTimeSlice % 3 == 0 )
					//{
					//	ClockManager::Instance().GetLTESlotClock().Update();
					//	ClockManager::Instance().GetTxClock().Update();
					//	ClockManager::Instance().GetRxClock().Update();
					//	m_pFastFading->Update();
					//}

					if ( iStandardTimeSlice % 3 == 0 )
					{
						for ( int iWiFi = 0; iWiFi < WIFISTANDARDPERLTESLOT; ++iWiFi )
						{
							ClockManager::Instance().GetWiFiClock().Update();
							ClockManager::Instance().GetLAAClock().Update();
							ClockManager::Instance().GetWiFiClock().Tick();
							ClockManager::Instance().GetLAAClock().Tick();
						}
					}

					if ( iStandardTimeSlice % 3 == 0 )
					{
						ClockManager::Instance().GetRxClock().Tick();
						ClockManager::Instance().GetTxClock().Tick();
						ClockManager::Instance().GetLTESlotClock().Tick();
						ClockManager::Instance().GetSystemClock().Tick();
					}

					//if ( iStandardTimeSlice % 4 == 0 )
					//	ClockManager::Instance().GetHSPASlotClock().Tick();

					if( iStandardTimeSlice % 6 == 0 )
						ClockManager::Instance().GetLTESubFrameClock().Tick();

					ClockManager::Instance().GetStandardClock().Tick();
				}

			//	ClockManager::Instance().GetHSPASubFrameClock().Tick();
			}

			ClockManager::Instance().GetFrameClock().Tick();

			//if ( iFrame % iFramePerHighLayerUpdate == 0 )
			//	ClockManager::Instance().GetHighLayerClock().Tick();
			//�ۼӷ���ʱ�䣬��֡Ϊ��λ
			++m_time;
			//��˥�ĸ���ʱ��Ҳ��֡Ϊ��λ������ShadowUpdatePeriod֡���и���
			if ( m_time % ShadowUpdatePeriod == 0 )
			{
				m_pSlowFading->Update();
			}
		}
 	}

	void System::End()
	{
		for (	UELIST::iterator iter = m_uelist.begin(); iter != m_uelist.end(); ++iter)
		{
			shared_ptr<Service> pService = iter -> second -> GetService( SYSTEM_LTE );
			pService -> Accept(*m_pThrStatistic);
			//pService -> Accept(*m_pLTETransIntervalStatistic);
			pService -> Accept(*m_pLTEPacketTimeStatistic);
			pService -> Accept(*m_pLTEPacketLossStatistic);
			//pService -> Accept(*m_pLTEChannelUtilizationStatistic);
		}

		for ( HSPAUELIST::iterator hspaUEIter = m_HSPAUEList.begin(); hspaUEIter != m_HSPAUEList.end(); ++hspaUEIter)
		{
			shared_ptr<Service> pService = hspaUEIter -> second -> GetService( SYSTEM_HSPA );
			pService -> Accept(*m_pHSPAThrStatistic);
			//hspaUEIter->second->GetService( SYSTEM_HSPA )->Accept(*m_pHSPAPacketTimeStatistic);
			SimLog::Instance()->LogMaxTransTimeNum( hspaUEIter->second->GetID(), dynamic_pointer_cast<HSPAUE>(hspaUEIter->second)->GetMaxTransTimeNum() );
		}

		//To Be Modified -- ����˫ģUE����������ͳ��
		//˫ģUE����������Ҫ�ֱ�ͳ��������ϵͳ��Service�����������Լ�һ���ܵ�������
		for( DUALMODELTEHSPAUELIST::iterator dualModeUEIter = m_DualModeLTEHSPAUEList.begin(); dualModeUEIter != m_DualModeLTEHSPAUEList.end(); ++dualModeUEIter )
		{
			//dualModeUEIter->second->GetService( SYSTEM_LTE )->Accept( *m_pThrStatistic );
			//dualModeUEIter->second->GetService( SYSTEM_HSPA )->Accept( *m_pHSPAThrStatistic );
			dualModeUEIter->second->GetService( SYSTEM_LTE )->Accept( *m_pDualModeThrStatisticLTEPart );
			dualModeUEIter->second->GetService( SYSTEM_HSPA )->Accept( *m_pDualModeThrStatisticHSPAPart );
		}

		/*����10�൱�ڳ���10ms�ٳ���1k���õ��ĵ�λ��Kbps*/
		USER_MESSAGE( m_pThrStatistic->GetSumStatistic() / GetStatisticTime() / FRAME_LENGTH_MS );
		USER_MESSAGE( m_pHSPAThrStatistic->GetSumStatistic() / GetStatisticTime() / FRAME_LENGTH_MS );

		vector<double> ltecu,wificu,laacu;
		double iSumRBNum = m_time * LTESUBFRAMEPERFRAME * GetConfig().GetInt("RB_Number");
		double iSumTime_us = m_time * FRAME_LENGTH_MS * 1000;

		size_t iLTENBNum = m_nblist.size();
		std::vector<double> nbthr(m_nblist.size(),0.0);
		std::vector<vector<double> > nbMCS(m_nblist.size()); 
		for(NBLIST::iterator pNB = m_nblist.begin(); pNB != m_nblist.end(); ++pNB)
		{
			ThroughputStatistic nbthrsta;
			MCSStatistic nbMCSsta;	//YRM
			for (NodeB::UELIST::const_iterator pUE = pNB->second->GetUEList().begin();  pUE != pNB->second->GetUEList().end(); ++pUE)
			{
				pUE->second.lock()->GetService( SYSTEM_LTE )->Accept(nbthrsta);
				pUE->second.lock()->GetService( SYSTEM_LTE )->Accept(nbMCSsta);	//YRM
			}
			nbthr[pNB->second->GetID()] = nbthrsta.GetSumStatistic();
			nbMCS[pNB->second->GetID()] = nbMCSsta.GetStatistic();	//NodeB��MCSͳ��

			ltecu.push_back( pNB->second->GetUsedRBNum()/iSumRBNum );
		}

		size_t iHSPANBNum = m_HSPANBList.size();
		std::vector<double> HSPAnbthr(iHSPANBNum,0.0);
		std::vector<vector<double> > HSPAnbMCS(m_HSPANBList.size()); 
		for(HSPANBLIST::iterator pNB = m_HSPANBList.begin(); pNB != m_HSPANBList.end(); ++pNB)
		{
			ThroughputStatistic nbthrsta;
			MCSStatistic HSPAnbMCSsta;	
			for (HSPANodeB::UELIST::const_iterator pUE = pNB->second->GetUEList().begin();  pUE != pNB->second->GetUEList().end(); ++pUE)
			{
				pUE->second.lock()->GetService( SYSTEM_HSPA )->Accept(nbthrsta);
				pUE->second.lock()->GetService( SYSTEM_HSPA )->Accept(HSPAnbMCSsta);
			}
			//HSPA NodeB��ID����LTE NodeB��ID�Ļ��������ӵ�
			HSPAnbthr[pNB->second->GetID() % iLTENBNum] = nbthrsta.GetSumStatistic();
			HSPAnbMCS[pNB->second->GetID() % iLTENBNum] = HSPAnbMCSsta.GetStatistic();	//HSPA NodeB��MCSͳ��
		}

		for ( WiFiUELIST::iterator wifiUEIter = m_WiFiUEList.begin(); wifiUEIter != m_WiFiUEList.end(); ++wifiUEIter )
		{
			shared_ptr<Service> pService = wifiUEIter -> second -> GetService( SYSTEM_WIFI );
			pService -> Accept(*m_pWiFiThrStatistic);
			pService -> Accept(*m_pWiFiPacketTimeStatistic);
			pService -> Accept(*m_pWiFiPacketLossStatistic);
			//pService -> Accept(*m_pWiFiChannelUtilizationStatistic);
			//pService -> Accept(*m_pWiFiTransIntervalStatistic);
		}

		vector<int> vecCollisionNum;
		for ( WiFiAPLIST::iterator iter = m_WiFiAPList.begin(); iter != m_WiFiAPList.end(); iter++ )
		{
			vecCollisionNum.push_back( iter->second->GetSumCollisionNum() );
			wificu.push_back( iter->second->GetTransmitDataTime()/iSumTime_us );
		}

		for ( LAAUELIST::iterator laaUEIter = m_LAAUEList.begin(); laaUEIter != m_LAAUEList.end(); ++laaUEIter )
		{
			shared_ptr<Service> pService =laaUEIter -> second -> GetService( SYSTEM_LAA );
			pService -> Accept(*m_pLAAThrStatistic);
			pService -> Accept(*m_pLAAPacketTimeStatistic);
			pService -> Accept(*m_pLAAPacketLossStatistic);
			//pService -> Accept(*m_pWiFiChannelUtilizationStatistic);
			//pService -> Accept(*m_pWiFiTransIntervalStatistic);
		}

		vector<int> veclaaCollisionNum;
		for ( LAANBLIST::iterator laaiter = m_LAANBList.begin(); laaiter != m_LAANBList.end(); laaiter++ )
		{
			veclaaCollisionNum.push_back( laaiter->second->GetSumCollisionNum() );
			laacu.push_back( laaiter->second->GetTransmitDataTime()/iSumTime_us );
		}

		for( LTEWiFiUELIST::iterator iterLTEWiFiUE = m_LTEWiFiUEList.begin(); iterLTEWiFiUE != m_LTEWiFiUEList.end(); ++iterLTEWiFiUE )
		{
			iterLTEWiFiUE->second->GetService( SYSTEM_LTE ) ->Accept( *m_pLTEWiFiThrStatisticLTEPart );
			iterLTEWiFiUE->second->GetService( SYSTEM_WIFI )->Accept( *m_pLTEWiFiThrStatisticWiFiPart );
			iterLTEWiFiUE->second->GetService( SYSTEM_LTE ) ->Accept( *m_pLTEWiFiPacketTimeStatisticLTEPart );
			iterLTEWiFiUE->second->GetService( SYSTEM_WIFI )->Accept( *m_pLTEWiFiPacketTimeStatisticWiFiPart );	
			iterLTEWiFiUE->second->GetService( SYSTEM_LTE ) ->Accept( *m_pLTEWiFiPacketLossStatisticLTEPart );
			iterLTEWiFiUE->second->GetService( SYSTEM_WIFI )->Accept( *m_pLTEWiFiPacketLossStatisticWiFiPart );
			//iterLTEWiFiUE->second->GetService( SYSTEM_LTE ) ->Accept( *m_pLTEWiFiChannelUtilizationStatisticLTEPart );
			//iterLTEWiFiUE->second->GetService( SYSTEM_WIFI )->Accept( *m_pLTEWiFiChannelUtilizationStatisticWiFiPart );	
		}

		//���ɽ��
		m_pResult->SetSimDuration( GetStatisticTime() * 10 );
		m_pResult->AddUEThr( m_pThrStatistic->GetStatistic() );
		m_pResult->AddNBThr( nbthr );
		m_pResult->AddNBMCS( nbMCS );
		m_pResult->AddHSPANBMCS( HSPAnbMCS );
		m_pResult->AddHSPAUEThr( m_pHSPAThrStatistic->GetStatistic() );
		m_pResult->AddHSPANBThr( HSPAnbthr );

		m_pResult->AddLTEPacketTime( m_pLTEPacketTimeStatistic->GetStatistic(), m_pLTEPacketTimeStatistic->GetAllStatistic() );
		m_pResult->AddWiFiPacketTime( m_pWiFiPacketTimeStatistic->GetStatistic(), m_pWiFiPacketTimeStatistic->GetAllStatistic() );
		m_pResult->AddLAAPacketTime( m_pLAAPacketTimeStatistic->GetStatistic(), m_pLAAPacketTimeStatistic->GetAllStatistic() );
		m_pResult->AddLTEWiFiPacketTime( m_pLTEWiFiPacketTimeStatisticLTEPart->GetStatistic(), m_pLTEWiFiPacketTimeStatisticLTEPart->GetAllStatistic(), SYSTEM_LTE );
		m_pResult->AddLTEWiFiPacketTime( m_pLTEWiFiPacketTimeStatisticWiFiPart->GetStatistic(), m_pLTEWiFiPacketTimeStatisticWiFiPart->GetAllStatistic(), SYSTEM_WIFI );

		m_pResult->AddLTEPacketLoss( m_pLTEPacketLossStatistic->GetStatistic(), m_pLTEPacketLossStatistic->GetAllStatistic() );
		m_pResult->AddWiFiPacketLoss( m_pWiFiPacketLossStatistic->GetStatistic(), m_pWiFiPacketLossStatistic->GetAllStatistic() );
		m_pResult->AddLAAPacketLoss( m_pLAAPacketLossStatistic->GetStatistic(), m_pLAAPacketLossStatistic->GetAllStatistic() );
		m_pResult->AddLTEWiFiPacketLoss( m_pLTEWiFiPacketLossStatisticLTEPart->GetStatistic(), m_pLTEWiFiPacketLossStatisticLTEPart->GetAllStatistic(), SYSTEM_LTE );
		m_pResult->AddLTEWiFiPacketLoss( m_pLTEWiFiPacketLossStatisticWiFiPart->GetStatistic(), m_pLTEWiFiPacketLossStatisticWiFiPart->GetAllStatistic(), SYSTEM_WIFI );


		double cu1 = std::accumulate(ltecu.begin(),ltecu.end(),0.0) / ltecu.size();
		double cu2 = std::accumulate(wificu.begin(),wificu.end(),0.0) / wificu.size();
		m_pResult->AddChannelUtilization(cu1,cu2);
		m_pResult->AddDualModeUEThr( m_pDualModeThrStatisticLTEPart->GetStatistic(), SYSTEM_LTE );
		m_pResult->AddDualModeUEThr( m_pDualModeThrStatisticHSPAPart->GetStatistic(), SYSTEM_HSPA );
		m_pResult->AddWiFiUEThr( m_pWiFiThrStatistic->GetStatistic() );
		m_pResult->AddLAAUEThr( m_pLAAThrStatistic->GetStatistic() );
		m_pResult->AddLTEWiFiUEThr( m_pLTEWiFiThrStatisticLTEPart->GetStatistic(), SYSTEM_LTE );
		m_pResult->AddLTEWiFiUEThr( m_pLTEWiFiThrStatisticWiFiPart->GetStatistic(), SYSTEM_WIFI );
		m_pResult->AddWiFiCollisionNum( vecCollisionNum );
		m_pResult->AddLAACollisionNum( veclaaCollisionNum );
		//m_pResult->SetWiFiSimDuration( GetConfig().GetInt("WiFi_Drop_Length(us)") );
	}

	//ϵͳ����ʱ��
	int System::GetTotalTime() const
	{
		return m_time;
	}
	//ϵͳ����ʱ��
	int System::GetConvergeTime() const
	{
		return GetConfig().GetInt("Converge_Time(frame)");
	}
	//ϵͳͳ��ʱ��
	int System::GetStatisticTime() const
	{
		return GetTotalTime() - GetConvergeTime();
	}
	
	bool System::StatisticEnabled() const
	{
		if(GetTotalTime() < GetConvergeTime())
			return false;
		return true;
	}

	//���һ��Drop�Ľ��
	const shared_ptr<SystemResult> System::GetResult() const
	{
		return m_pResult;
	}

#pragma endregion ������


#pragma region �ն˹���

	//����LTE UE
	bool System::AddLTEUE( const System::UEPTR pUE )
	{
		using std::make_pair;
		if (m_uelist.count(pUE->GetID()))	//UE�Ƿ��Ѿ�����
		{
			std::stringstream ss;
			ss << "There is already a LTE UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_uelist.insert(make_pair(pUE->GetID(), pUE));			//������UE

		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForAllClock( pTimeClient );	//ע����UE��ʱ��
		pUE->Attach(shared_from_this());	//��System����UE
		return true;
	}

	//����HSPA UE
	bool System::AddHSPAUE(const HSPAUEPTR pUE)
	{
		using std::make_pair;
		if (m_HSPAUEList.count(pUE->GetID()))	//UE�Ƿ��Ѿ�����
		{
			std::stringstream ss;
			ss << "There is already a HSPA UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_HSPAUEList.insert(make_pair(pUE->GetID(), pUE));	//������UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForAllClock( pTimeClient );	//ע����UE��ʱ��
		pUE->Attach(shared_from_this());	//��System����UE
		return true;
	}

	//����Dual Mode UE
	bool System::AddDualModeLTEHSPAUE(const DUALMODELTEHSPAUEPTR pUE )
	{
		using std::make_pair;
		if (m_DualModeLTEHSPAUEList.count(pUE->GetID()))	//UE�Ƿ��Ѿ�����
		{
			std::stringstream ss;
			ss << "There is already a DualMode UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_DualModeLTEHSPAUEList.insert(make_pair(pUE->GetID(), pUE));	//������UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForAllClock( pTimeClient );	//ע����UE��ʱ��
		pUE->Attach(shared_from_this());	//��System����UE
		return true;
	}

	//����WiFi UE
	bool System::AddWiFiUE(const WiFiUEPTR pUE)
	{
		using std::make_pair;
		if (m_WiFiUEList.count(pUE->GetID()))	//UE�Ƿ��Ѿ�����
		{
			std::stringstream ss;
			ss << "There is already a WiFi UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_WiFiUEList.insert(make_pair(pUE->GetID(), pUE));	//������UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForWiFiClock( pTimeClient );	//ע����UE��ʱ��
		pUE->Attach(shared_from_this());	//��System����UE
		return true;
	}
	//����LAA UE
	bool System::AddLAAUE(const LAAUEPTR pUE)
	{
		using std::make_pair;
		if (m_LAAUEList.count(pUE->GetID()))	//UE�Ƿ��Ѿ�����
		{
			std::stringstream ss;
			ss << "There is already a LAA UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_LAAUEList.insert(make_pair(pUE->GetID(), pUE));	//������UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForLAAClock( pTimeClient );	//ע����UE��ʱ��
		pUE->Attach(shared_from_this());	//��System����UE
		return true;
	}

	bool System::AddLTEWiFiUE(const LTEWiFiUEPTR pUE)
	{
		using std::make_pair;
		if (m_LTEWiFiUEList.count(pUE->GetID()))	//UE�Ƿ��Ѿ�����
		{
			std::stringstream ss;
			ss << "There is already a LTEWiFi UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_LTEWiFiUEList.insert(make_pair(pUE->GetID(), pUE));	//������UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForAllClock( pTimeClient );	//ע����UE��ʱ��
		ClockManager::Instance().RegisterForWiFiClock( pTimeClient );	//ע�ᵽWiFiʱ��
		pUE->Attach(shared_from_this());	//��System����UE
		return true;
	}

	System::IUEPTR System::GetBaseUE(int iID)
	{
		if (iID < m_uelist.size())
		{
			return GetUE( iID );
		}
		else if (iID < m_uelist.size() + m_HSPAUEList.size())
		{
			return GetHSPAUE( iID );
		}
		else if(iID < m_uelist.size() + m_HSPAUEList.size() + m_DualModeLTEHSPAUEList.size())
		{
			return GetDualModeLTEHSPAUE( iID );
		}
		else if( iID < m_uelist.size() + m_HSPAUEList.size() + m_DualModeLTEHSPAUEList.size() + m_LTEWiFiUEList.size() )
		{
			return GetLTEWiFiUE( iID );
		}
		else
		{
			return IUEPTR();
		}
	}

	//��ȡUE
	System::UEPTR System::GetUE( int ID )
	{
		if (m_uelist.count(ID) == 1)
		{
			return m_uelist[ID];
		}
		USER_MESSAGE("Can't find UE " << ID);
		return ILTEUE::PTR();	//���ؿ�ָ��
	}

	System::UEPTR System::GetUEFromOffsetID( int iID )
	{
		int iAbsoluteID = ConvertUEOffsetIDToAbsoluteID( iID, SYSTEM_LTE );
		
		return GetUE( iAbsoluteID );
	}

	System::HSPAUEPTR System::GetHSPAUE(int iID)
	{
		HSPAUELIST::iterator mapIter = m_HSPAUEList.find( iID );
		if ( mapIter != m_HSPAUEList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find HSPA UE " << iID);
		return IHSPAUE::PTR();
	}

	System::HSPAUEPTR System::GetHSPAUEFromOffsetID(int iID)
	{
		int iAbsoluteID = ConvertUEOffsetIDToAbsoluteID( iID, SYSTEM_HSPA );
		
		return GetHSPAUE( iAbsoluteID );
	}

	System::DUALMODELTEHSPAUEPTR System::GetDualModeLTEHSPAUE(int iID)
	{
		DUALMODELTEHSPAUELIST::iterator mapIter = m_DualModeLTEHSPAUEList.find( iID );

		if( mapIter != m_DualModeLTEHSPAUEList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find Dual Mode UE " << iID);
		return IDualModeLTEHSPAUE::PTR();
	}

	System::DUALMODELTEHSPAUEPTR System::GetDualModeLTEHSPAUEFromOffsetID(int iID)
	{
		int iAbsoluteID = ConvertUEOffsetIDToAbsoluteID( iID, SYSTEM_DUAL );

		return GetDualModeLTEHSPAUE( iAbsoluteID );
	}

	System::LTEWiFiUEPTR System::GetLTEWiFiUE( int iID )
	{
		LTEWiFiUELIST::iterator mapIter = m_LTEWiFiUEList.find( iID );
		if( mapIter != m_LTEWiFiUEList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find LTEWiFi UE " << iID);
		return ILTEWiFiUE::PTR();
	}

	System::WiFiUEPTR System::GetWiFiUE( int iID )
	{
		WiFiUELIST::iterator mapIter = m_WiFiUEList.find( iID );
		if( mapIter != m_WiFiUEList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find WiFi UE " << iID);
		return IWiFiUE::PTR();
	}

	//��ȡUE�б�
	const System::UELIST& System::GetUEList() const
	{
		return m_uelist;
	}

	const System::HSPAUELIST& System::GetHSPAUEList() const
	{
		return m_HSPAUEList;
	}

	//ɾ��UE
	System::UEPTR System::RemoveUE( int ID )
	{
		ILTEUE::PTR pUE = GetUE(ID);
		m_nblist.erase(ID);
		return pUE;
	}

	//���UEList
	void System::ClearUE()
	{
		m_uelist.clear();
	}

#pragma endregion �ն˹���

#pragma region NodeB����

	int System::ConvertNBAbsoluteIDToOffsetID(int iID, SystemType systemType)
	{
		switch(systemType)
		{
		case SYSTEM_LTE:
			return iID;
		case SYSTEM_HSPA:
			return (iID - (int)m_nblist.size());
		case SYSTEM_WIFI:
			return (iID - (int)m_nblist.size() - (int)m_HSPANBList.size());
		case SYSTEM_DUAL:
		default:
			throw std::invalid_argument( "Invalid System Type" );
		}
	}

	int System::ConvertNBOffsetIDToAbsoluteID(int iID, SystemType systemType )
	{
		switch(systemType)
		{
		case SYSTEM_LTE:
			return iID;
		case SYSTEM_HSPA:
			return (iID + (int)m_nblist.size());
		case SYSTEM_WIFI:
			return (iID + (int)m_nblist.size() + (int)m_HSPANBList.size());
		case SYSTEM_DUAL:
		default:
			throw std::invalid_argument( "Invalid System Type" );
		}
	}

	int System::ConvertUEAbsoluteIDToOffsetID(int iID, SystemType systemType )
	{
		switch(systemType)
		{
		case SYSTEM_LTE:
			return iID;
		case SYSTEM_HSPA:
			return (iID - (int)m_uelist.size());
		case SYSTEM_DUAL:
			return (iID - (int)m_uelist.size() - (int)m_HSPAUEList.size());
		default:
			throw std::invalid_argument( "Invalid System Type" );
		}
	}

	int System::ConvertUEOffsetIDToAbsoluteID(int iID, SystemType systemType )
	{
		switch(systemType)
		{
		case SYSTEM_LTE:
			return iID;
		case SYSTEM_HSPA:
			return (iID + (int)m_uelist.size());
		case SYSTEM_DUAL:
			return (iID + (int)m_uelist.size() + (int)m_HSPAUEList.size());
		default:
			throw std::invalid_argument( "Invalid System Type" );
		}
	}

	//����NodeB
	bool System::AddNodeB( const System::NBPTR pNodeB )
	{
		using std::make_pair;
		if (m_nblist.count(pNodeB->GetID()))	//�жϸ�NodeB�Ƿ����
		{
			std::stringstream ss;
			ss << "There is already a NodeB with ID " << pNodeB->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_nblist.insert(make_pair(pNodeB->GetID(), pNodeB));	//������NodeB
		pNodeB->Attach(shared_from_this());	//ʹNodeB��System����
		ClockManager::Instance().RegisterForAllClock( pNodeB );	//ע����NodeB��ʱ��
		return true;
	}

	bool System::AddHSPANodeB(const LTESim::System::HSPANBPTR pNodeB)
	{
		using std::make_pair;
		if (m_HSPANBList.count(pNodeB->GetID()))								//�жϸ�NodeB�Ƿ����
		{
			std::stringstream ss;
			ss << "There is already a NodeB with ID " << pNodeB->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_HSPANBList.insert(make_pair(pNodeB->GetID(), pNodeB));	//������NodeB
		pNodeB->Attach(shared_from_this());											//ʹNodeB��System����
		ClockManager::Instance().RegisterForAllClock( pNodeB );				//ע����NodeB��ʱ��
		return true;
	}

	bool System::AddWiFiAP(const WiFiAPPTR pAP)
	{
		using std::make_pair;
		if ( m_WiFiAPList.count( pAP->GetID() ) )			
		{
			std::stringstream ss;
			ss << "There is already a AP with ID " << pAP->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_WiFiAPList.insert(make_pair(pAP->GetID(), pAP));	
		pAP->Attach(shared_from_this());						
		//ClockManager::Instance().RegisterForAllClock( pAP );	
		ClockManager::Instance().RegisterForWiFiClock( pAP );
		return true;
	}
	bool System::AddLAANB(const LAANBPTR pNodeB)
	{
		using std::make_pair;
		if ( m_LAANBList.count( pNodeB->GetID() ) )			
		{
			std::stringstream ss;
			ss << "There is already a NB with ID " << pNodeB->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_LAANBList.insert(make_pair(pNodeB->GetID(), pNodeB));	
		pNodeB->Attach(shared_from_this());						
		//ClockManager::Instance().RegisterForAllClock( pAP );	
		ClockManager::Instance().RegisterForLAAClock( pNodeB );
		return true;
	}
	//���NodeB�б�
	const System::NBLIST& System::GetNBList() const
	{
		return m_nblist;
	}

	const System::HSPANBLIST& System::GetHSPANBList() const
	{
		return m_HSPANBList;
	}

	const System::DUALMODELTEHSPAUELIST& System::GetDualModeLTEHSPAUEList() const
	{
		return m_DualModeLTEHSPAUEList;
	}

	const System::WiFiAPLIST& System::GetWiFiAPList() const
	{
		return m_WiFiAPList;
	}

	const System::LAANBLIST& System::GetLAANBList() const
	{
		return m_LAANBList;
	}

	const System::LTEWiFiUELIST& System::GetLTEWiFiUEList() const
	{
		return m_LTEWiFiUEList;
	}

	const System::WiFiUELIST& System::GetWiFiUEList() const
	{
		return m_WiFiUEList;
	}

	const System::LAAUELIST& System::GetLAAUEList() const
	{
		return m_LAAUEList;
	}

	//��ȡNodeB
	System::NBPTR System::GetNodeB( int ID )
	{
		if (m_nblist.count(ID) == 1)
		{
			return m_nblist[ID];
		}
		USER_MESSAGE("Can't find NodeB "<< ID);
		return NodeB::PTR();
	}

	System::NBPTR System::GetNodeBFromOffsetID(int iID)
	{
		int iAbsoluteID = ConvertNBOffsetIDToAbsoluteID( iID, SYSTEM_LTE );
		NBLIST::iterator mapIter = m_nblist.find( iID );
		if ( mapIter != m_nblist.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find NodeB "<< iAbsoluteID);
		return NodeB::PTR();
	}

	System::HSPANBPTR System::GetHSPANodeB( int iID )
	{
		HSPANBLIST::iterator mapIter = m_HSPANBList.find( iID );
		if ( mapIter != m_HSPANBList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find HSPA NodeB "<< iID);
		return HSPANodeB::PTR();
	}

	System::HSPANBPTR System::GetHSPANodeBFromOffsetID( int iID )
	{
		int iAbsoluteID = ConvertNBOffsetIDToAbsoluteID( iID, SYSTEM_HSPA );
		HSPANBLIST::iterator mapIter = m_HSPANBList.find( iAbsoluteID );
		if ( mapIter != m_HSPANBList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find HSPA NodeB "<< iAbsoluteID);
		return HSPANodeB::PTR();
	}

	System::WiFiAPPTR System::GetWiFiAP( int iID )
	{
		WiFiAPLIST::iterator mapIter = m_WiFiAPList.find( iID );
		if ( mapIter != m_WiFiAPList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find WiFi AP "<< iID);
		return WiFiAP::PTR();
	}

	System::WiFiAPPTR System::GetWiFiAPFromOffsetID( int iID )
	{
		int iAbsoluteID = ConvertNBOffsetIDToAbsoluteID( iID, SYSTEM_WIFI );
		WiFiAPLIST::iterator mapIter = m_WiFiAPList.find( iAbsoluteID );
		if ( mapIter != m_WiFiAPList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find WiFi AP "<< iAbsoluteID);
		return WiFiAP::PTR();
	}

	System::LAANBPTR System::GetLAANB( int iID )
	{
		LAANBLIST::iterator mapIter = m_LAANBList.find( iID );
		if ( mapIter != m_LAANBList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find WiFi AP "<< iID);
		return LAANodeB::PTR();
	}

	System::LAANBPTR System::GetLAANBFromOffsetID( int iID )
	{
		int iAbsoluteID = ConvertNBOffsetIDToAbsoluteID( iID, SYSTEM_LAA );
		LAANBLIST::iterator mapIter = m_LAANBList.find( iAbsoluteID );
		if ( mapIter != m_LAANBList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find LAA AP "<< iAbsoluteID);
		return LAANodeB::PTR();
	}
	//ɾ��NodeB
	System::NBPTR System::RemoveNodeB( int ID )
	{
		NodeB::PTR pNodeB = GetNodeB(ID);
		m_nblist.erase(ID);
		return pNodeB;
	}

	//���nodeBList
	void System::ClearNodeB()
	{
		m_nblist.clear();
	}

	bool System::AddHighLayerController( const HIGHLAYERCONTROLLERPTR pHighLayerController )
	{
		int iID = pHighLayerController->GetID();
		if ( m_HighLayerControllerList.count( iID ) )
		{
			std::stringstream ss;
			ss << "There is already a high layer controller with ID: " << iID << std::endl;
			throw std::invalid_argument( ss.str() );
		}
		m_HighLayerControllerList.insert( std::make_pair( iID, pHighLayerController ) );
		pHighLayerController->Attach( shared_from_this() );
		ClockManager::Instance().RegisterForAllClock( pHighLayerController );
		return true;
	}

	System::HIGHLAYERCONTROLLERPTR System::GetAssociateHighLayerController( NBPTR pLTENodeB )
	{
		return GetHighLayerController( ConvertNBAbsoluteIDToOffsetID( pLTENodeB->GetID(), SYSTEM_LTE ) );
	}

	System::HIGHLAYERCONTROLLERPTR System::GetAssociateHighLayerController( HSPANBPTR pHSPANodeB )
	{
		return GetHighLayerController( ConvertNBAbsoluteIDToOffsetID( pHSPANodeB->GetID(), SYSTEM_HSPA ) );
	}

	System::HIGHLAYERCONTROLLERPTR System::GetHighLayerController( int iID )
	{
		HIGHLAYERCONTROLLERLIST::iterator mapIter = m_HighLayerControllerList.find( iID );
		if ( mapIter != m_HighLayerControllerList.end() )
			return mapIter->second;

		USER_MESSAGE("Can't find HighLayerController "<< iID);
		return HighLayerController::PTR();
	}

#pragma endregion

#pragma region ϵͳ����

	int System::GetLTESubFramesPerFrame() const
	{
		//1��֡����10����֡
		return LTESUBFRAMEPERFRAME;
	}

	int System::GetHSPASubFramePerFrame() const
	{
		return HSPASUBFRAMEPERFRAME;
	}

	int System::GetStandardTimeSlicesPerHSPASubFrame() const
	{
		return  STANDARDTIMESLICEPERHSPASUBFRAME;
	}

	int System::GetFramesPerHighLayerClockUpdate() const
	{
		return m_pParameters->GetInt( "Frames_Per_HighLayerUpdate" );
	}

	//��ǰ���ò���
	const RunParameters& System::GetConfig() const
	{
		return *m_pParameters;
	}
	//MCS��
	const MCSTable& System::GetMCSTables() const
	{
		return *m_pMCSTables;
	}
	const MCSTable& System::GetHSPAMCSTables() const
	{
		return *m_pHSPAMCSTables;
	}
	const MCSTable& System::GetWiFiMCSTables() const
	{
		return *m_pWiFiMCSTables;
	}
	const MCSTable& System::GetLAAMCSTables() const
	{
		return *m_pLAAMCSTables;
	}
	//AMC��
	const System::AMCTABLE& System::GetAMCTable() const
	{
		return *m_pAMCTable->GetTable();
	}

	const System::AMCTABLE& System::GetHSPAAMCTable() const
	{
		return *m_pHSPAAMCTable->GetTable();
	}

	const System::AMCTABLE& System::GetWiFiAMCTable() const
	{
		return *m_pWiFiAMCTable->GetTable();
	}

	const System::AMCTABLE& System::GetLAAAMCTable() const
	{
		return *m_pLAAAMCTable->GetTable();
	}

	//��˥��
	const Fading& System::GetFading() const
	{
		return *m_pFastFading;
	}
	const HSPAFading& System::GetHSPAFading() const
	{
		return *m_pHSPAFastFading;
	}
	const WiFiFading& System::GetWiFiFading() const
	{
		return *m_pWiFiFastFading;
	}
	const LAAFading& System::GetLAAFading() const
	{
		return *m_pLAAFastFading;
	}
	//��˥
    const Fading& System::GetSlowFading() const
	{
		return *m_pSlowFading;
	}
	const WiFiFading& System::GetWiFiShadowFading() const
	{
		return *m_pWiFiShadowFading;
	}
	const LAAFading& System::GetLAAShadowFading() const
	{
		return *m_pLAAShadowFading;
	}
	//·��
	const Pathloss& System::GetPathLoss(SystemType systemType) const
	{
		switch(systemType)
		{
		case SYSTEM_LTE:
			return *m_pLTEPathLoss;
		case SYSTEM_HSPA:
			return *m_pHSPAPathLoss;
		case SYSTEM_WIFI:
			return *m_pWiFiPathLoss;
		case SYSTEM_LAA:
			return *m_pLAAPathloss;
		default:
			throw std::invalid_argument( "Invalid System Type." );
		}
	}
	//ϵͳ��������Ϣ
	const Map& System::GetMap() const
	{
		return *m_pMap;
	}
	/*const Map& System::GetIndoorMap() const
	{
		return *m_pWiFiMap;
	}*/
	const Map& System::GetWiFiMap() const
	{
		return *m_pWiFiMap;
	}
	const Map& System::GetLAAMap() const
	{
		return *m_pLAAMap;
	}
	//ϵͳ�о���
	const Decisioner& System::GetDecisioner( SystemType sysType ) const
	{
		if ( sysType == SYSTEM_LTE )
			return *m_pLTEDecisioner;
		else if( sysType == SYSTEM_HSPA )
			return *m_pHSPADecisioner;
		else if( sysType == SYSTEM_WIFI )
			return *m_pWiFiDecisioner;
		else if( sysType == SYSTEM_LAA )
			return *m_pLAADecisioner;
		else
			throw std::invalid_argument( "Unknown system type identifier" );
	}
	//�����ܶ�
	double System::GetThermalDensity() const
	{
		return m_ThermalDensity;
	}
	double System::GetWiFiThermalDensity() const
	{
		return m_dWiFiThermalDensity;
	}
	double System::GetLAAThermalDensity() const
	{
		return m_dLAAThermalDensity;
	}

#pragma endregion

#pragma region ��̬��ȫ�ַ��ʵ�

	shared_ptr<System>  System::UpdateCurrentSystem()
	{
		shared_ptr<System> pOld = ms_pCurrentSystem;
		ms_pCurrentSystem = shared_from_this();
		return pOld;
	}
	shared_ptr<System> System::ms_pCurrentSystem;
	shared_ptr<System> System::GetCurrentSystem()
	{
		return ms_pCurrentSystem;
	}

	double System::GetRBBandWidth() const
	{
		return m_RBBandWidth;
	}

	double System::GetWiFiBandWidth() const
	{
		return m_dWiFiBandWidth;
	}

	double System::GetLAABandWidth() const
	{
		return m_dLAABandWidth;
	}

#pragma endregion ��̬��ȫ�ַ��ʵ�

}



