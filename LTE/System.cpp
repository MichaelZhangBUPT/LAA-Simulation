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
#pragma region 构造析构函数

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

#pragma region 主流程
#pragma region 初始化
	//系统初始化函数，加入基站和移动台
	bool System::Init()
	{

		//更新全局访问点为本System，之后的GetCurrentSystem返回本System
        UpdateCurrentSystem();
    
        //时钟清零
		ClockManager::Instance().Reset();

        //读入MCS表、AMC表
		InitTables();
		//初始化拓扑模型
		InitMap();

		//初始化小区，根据要求初始化多个基站
		InitNodeB();

		//初始化小区内的UE
		InitUE();

		//初始化高层控制器
		InitHighLayerController();
     
        //初始化路损
        InitPathLoss();

		//WiFi系统带宽
		m_dWiFiBandWidth = GetConfig().GetDouble("WiFi_BandWidth(MHz)") * 1e6;
		//WiFi底噪密度
		m_dWiFiThermalDensity = dBToReal(GetConfig().GetDouble("WiFi_UE_Thermal_Density(dBm)"));

		m_dLAABandWidth = GetConfig().GetDouble("WiFi_BandWidth(MHz)") * 1e6;
		//WiFi底噪密度
		m_dLAAThermalDensity = dBToReal(GetConfig().GetDouble("WiFi_UE_Thermal_Density(dBm)"));

        //初始化衰落  快慢衰初始化  改到这里
        InitSlowFading();
        InitFastFading();	 

	
        //初始化智能天线增益表
        InitArray();

		//初始化判决器
		InitDecisioner();

		//系统带宽
		m_RBBandWidth = GetConfig().GetDouble("RB_Bandwidth(KHz)") * 1000;

        //底噪密度
		m_ThermalDensity = dBToReal(GetConfig().GetDouble("Thermal_Density(dbm)"));	

        //系统运行时间置0
		m_time = 0;
 
        //初始化干扰计算器
        InitSINRCalculator();
		
		if (0) 
		{	//输出WiFi小区SINR分布的数据
			std::ofstream osinrfile;
			osinrfile.open( "../Output/WiFiSINR.txt", std::ios::trunc );
			WiFiSINRCalculator::Instance() -> OutputAllSINR( osinrfile );
		}

        return true;
	}

	//读入MCS表和AMC表，还要读入fading表（为了性能可能修改成static方法，并将相应的表修改成static成员变量
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

#pragma region AMC表
		const double TargetBLER = GetConfig().GetDouble("AMC_BLER_TARGET");
		const double HSPATargetBLER = GetConfig().GetDouble("HSPA_AMC_BLER_TARGET");
		const double WiFiTargetBLER = GetConfig().GetDouble("WiFi_AMC_BLER_TARGET");
		const double LAATargetBLER = GetConfig().GetDouble("LAA_AMC_BLER_TARGET");
		m_pAMCTable.reset(new AMCTable(*m_pMCSTables, TargetBLER, SYSTEM_LTE));
		m_pHSPAAMCTable.reset(new AMCTable(*m_pHSPAMCSTables, HSPATargetBLER, SYSTEM_HSPA));
		m_pWiFiAMCTable.reset(new AMCTable(*m_pWiFiMCSTables, WiFiTargetBLER, SYSTEM_WIFI));
		m_pLAAAMCTable.reset(new AMCTable(*m_pLAAMCSTables, LAATargetBLER, SYSTEM_LAA));

#pragma endregion AMC表
	}

	void System::InitDecisioner()
	{
		//全局可以用一个判决器，不会影响判决结果
		m_pLTEDecisioner.reset(new DecisionerImp(GetMCSTables()));

		m_pHSPADecisioner.reset(new DecisionerImp(GetHSPAMCSTables()));

		m_pWiFiDecisioner.reset(new DecisionerImp(GetWiFiMCSTables()));

		m_pLAADecisioner.reset(new DecisionerImp(GetLAAMCSTables()));
	}

	void System::InitHighLayerController()
	{
		//计算小区半径
		double  iNBRadius = GetConfig().GetInt("Inter_Site_Distance") * std::sqrt(3.0) / 3.0;
		//每一个小区分配一个高层控制器，所以高层控制器的数量和NodeB的数量相等，
		//并且与相同ID对应的eNodeB的坐标也一致
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
		//初始化7小区模型
//		m_pMap = shared_ptr<Map>(new SevenCells);
		//WiFi采用单个圆形小区
		m_pWiFiMap = shared_ptr<Map>(new Indoor);
		m_pLAAMap = shared_ptr<Map>(new CircleCell);
	}

	void System::InitNodeB()
	{
		//基站个数, 现在考虑的情况时LTE和HSPA的基站是共站共址的，所以基站数目和位置都一样
		int iNodeBNumber = GetConfig().GetInt("NodeB_Number");
		//计算小区半径
		double  iNBRadius = GetConfig().GetInt("Inter_Site_Distance") * std::sqrt(3.0) / 3.0;
		double	dNBAPDistance = GetConfig().GetInt("NodeB_AP_Distance");	//基站与WiFi AP的距离

		//初始化NodeBNumber个基站
		for (int i = 0; i < iNodeBNumber ; ++i)
		{
			double x = iNBRadius * ( m_pMap -> GetNBPos(i).GetX() );
			double y = iNBRadius * ( m_pMap -> GetNBPos(i).GetY() );
			AddNodeB( m_nbmaker -> CreateNodeB( i, x, y ) );
			AddHSPANodeB( m_nbmaker -> CreateHSPANodeB( i + iNodeBNumber, x, y ) );
		}
		//初始化Indoor场景
        int iWiFiNumber = GetConfig().GetInt("WiFi_Number");
        //得到小区半径
        double iIndoorRadius = GetConfig().GetInt("Circle_Cell_Radius");
        //初始化wifi和LAA节点(LAA未加!)
        for(int i=0;i<iWiFiNumber;++i)
        {
            double x = m_pWiFiMap ->GetNBPos(i).GetX();
            double y = m_pWiFiMap ->GetNBPos(i).GetY();
			double xx = m_pLAAMap ->GetNBPos(i).GetX();
			double yy = m_pLAAMap ->GetNBPos(i).GetY();
            AddWiFiAP(m_nbmaker ->CreateWiFiAP(i,x,y));
			AddLAANB(m_nbmaker ->CreateLAANodeB(i,xx,yy));//这里位置还需要改一改
        }

		
	}

	void System::InitUE()
	{
		RandomManager& randman = RandomManager::GetRandomManager();
		int iLTEUENumber = GetConfig().GetInt("LTE_UE_Number");					//LTE UE的个数
		int iLTEUENumInHotspot = iLTEUENumber * GetConfig().GetDouble("P_LTEUEInHotspot"); //LTEUE中撒在热点里的个数
		int iHSPAUENumber = GetConfig().GetInt( "HSPA_UE_Number" );		//HSPA UE的个数
		int iDualModeUENumber = GetConfig().GetInt( "DualMode_UE_Number" );
		int iLTEWiFiUENumber = GetConfig().GetInt("LTEWiFi_UE_Number");
		int iNodeBNumber = GetConfig().GetInt("NodeB_Number");				//基站的个数
		int iAPNumber = iNodeBNumber;
		double  dNBRadius = GetConfig().GetInt("Inter_Site_Distance") * std::sqrt(3.0) / 3.0;	//小区半径
		int iNodeBAPDis = GetConfig().GetInt("NodeB_AP_Distance");				//NodeB和AP的距离

		bool bFTPVoIPCoexist = GetConfig().GetBool("FTP_VoIP_Coexist");
		int iFTPLTEUENum = floor( GetConfig().GetInt("LTE_UE_Number")*GetConfig().GetDouble("FTP_UE_Percentage")+0.5 );//四舍五入取整
		int iVoIPLTEUENum = floor( GetConfig().GetInt("LTE_UE_Number")*GetConfig().GetDouble("VoIP_UE_Percentage")+0.5 );
		
		int iWiFiNumber = GetConfig().GetInt("WiFi_Number");
		double	dWiFiRadius = GetConfig().GetInt("Circle_Cell_Radius");
		int iWiFiUENumber = GetConfig().GetInt("WiFi_UE_Number");	//WiFi UE的个数

		int iLAANumber = GetConfig().GetInt("LAA_Number");
		double	dLAARadius = GetConfig().GetInt("Circle_Cell_Radius");
		int iLAAUENumber = GetConfig().GetInt("LAA_UE_Number");
		
		const Map& instWiFiMap = GetWiFiMap();
		const Map& instLAAMap = GetLAAMap();
		//const Map& instIndoorMap = GetIndoorMap();
		//将用户均匀的分配到每个基站，在每个基站内的坐标再均匀分布；实现了UE的均匀分布
		int iUEIDBase = 0;
#pragma region 判定没有宏基站就不执行
		if (iNodeBNumber > 0)
		{
		const Map& instMap = GetMap();
		//LTE用户的初始化
		for (int iUEIndex = 0; iUEIndex < iLTEUENumber; iUEIndex++)
		{
			int AssociateNBID = iUEIndex % iNodeBNumber;
            POS relativePOS;
            bool RegeneratePos = false;
            do 
            {   
                RegeneratePos = false;
                relativePOS = instMap.GenerateUEPOS(dNBRadius);	//UE相对于所在基站的坐标
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
					//这里还需要检查是否距离本小区是最近的，如果离相邻小区的距离更近，则需要重新进行生成位置
					//这样来保证ID为i的UE，肯定是距离i%NodeBNumber的NodeB距离最近
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

#pragma region 其他UE
		//HSPA UE的初始化
		iUEIDBase += iLTEUENumber;
		for( int iUEIndex = 0; iUEIndex < iHSPAUENumber; iUEIndex++ )
		{
			int AssociateNBID = iUEIndex % iNodeBNumber;
			POS relativePOS;
			bool RegeneratePos = false;
			do 
			{   
				RegeneratePos = false;
				relativePOS = instMap.GenerateUEPOS(dNBRadius);	//UE相对于所在基站的坐标
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
					//这里还需要检查是否距离本小区是最近的，如果离相邻小区的距离更近，则需要重新进行生成位置
					//这样来保证ID为i的UE，肯定是距离i%NodeBNumber的NodeB距离最近
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

		//Dual UE的初始化
		iUEIDBase += iHSPAUENumber;
		for ( int iUEIndex = 0; iUEIndex < iDualModeUENumber; iUEIndex++ )
		{
			int AssociateNBID = iUEIndex % iNodeBNumber;
			POS relativePOS;
			bool RegeneratePos = false;
			do 
			{   
				RegeneratePos = false;
				relativePOS = instMap.GenerateUEPOS(dNBRadius);	//UE相对于所在基站的坐标
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
					//这里还需要检查是否距离本小区是最近的，如果离相邻小区的距离更近，则需要重新进行生成位置
					//这样来保证ID为i的UE，肯定是距离i%NodeBNumber的NodeB距离最近
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

		//LTEWiFi UE的初始化
		iUEIDBase += iDualModeUENumber;
		for ( int iUEIndex = 0; iUEIndex < iLTEWiFiUENumber; iUEIndex++ )
		{
			int AssociateAPID = iUEIndex % iAPNumber;
			POS relativePOS = instWiFiMap.GenerateUEPOS(dWiFiRadius);	//UE相对于所在AP的坐标
			POS realPos( relativePOS.GetX() + GetWiFiAPFromOffsetID(AssociateAPID)->GetPos().GetX(),
					relativePOS.GetY() + GetWiFiAPFromOffsetID(AssociateAPID)->GetPos().GetY() );

			AddLTEWiFiUE( m_uemaker->CreateLTEWiFiUE( iUEIndex + iUEIDBase, realPos.GetX(), realPos.GetY() ) );		
		}

#pragma endregion 其他UE

		//WiFiUE初始化
		iUEIDBase += iLTEWiFiUENumber;
		bool bWiFiFTPVoIPCoexist = GetConfig().GetBool("WiFi_FTP_VoIP_Coexist");
		int iFTPWiFiUENum = floor( GetConfig().GetInt("WiFi_UE_Number")*GetConfig().GetDouble("WiFi_FTP_UE_Percentage")+0.5 );//四舍五入取整
		int iVoIPWiFiUENum = floor( GetConfig().GetInt("WiFi_UE_Number")*GetConfig().GetDouble("WiFi_VoIP_UE_Percentage")+0.5 );
		for (int iUEIndex = 0; iUEIndex < iWiFiUENumber; iUEIndex++)
		{
			int AssociateAPID = iUEIndex % iAPNumber;
			POS relativePOS = instWiFiMap.GenerateUEPOS(dWiFiRadius);	//UE相对于所在AP的坐标
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

		//代表AP的4个虚拟UE，ID分别为0~3
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
		//WiFiUE初始化
		bool bWiFiFTPVoIPCoexist = GetConfig().GetBool("WiFi_FTP_VoIP_Coexist");
		int iFTPWiFiUENum = GetConfig().GetInt("WiFi_UE_Number");//*GetConfig().GetDouble("WiFi_FTP_UE_Percentage")+0.5 );//四舍五入取整
		int iVoIPWiFiUENum = 0;//floor( GetConfig().GetInt("WiFi_UE_Number")*GetConfig().GetDouble("WiFi_VoIP_UE_Percentage")+0.5 );
		for (int iUEIndex = 0; iUEIndex < iWiFiUENumber; iUEIndex++)
		{
			int AssociateAPID = iUEIndex % iWiFiNumber;
			POS relativePOS = instWiFiMap.GenerateUEPOS(dWiFiRadius);	//UE相对于所在AP的坐标
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

		//代表AP的虚拟UE
		//iUEIDBase += iWiFiUENumber;
		//int num = 0;
		//for ( WiFiAPLIST::iterator iter = m_WiFiAPList.begin() ; iter != m_WiFiAPList.end(); iter++, num++ )
		//{
		//	WiFiAPPTR pAP = iter->second;
		//	POS APPos = pAP -> GetPos();
		//	AddWiFiUE( m_uemaker->CreateWiFiUE( num + iUEIDBase, APPos.GetX(), APPos.GetY(), true, "VoIP" ) );
		//}


		//LAAUE初始化
		iUEIDBase += iWiFiUENumber;
		bool bLAAFTPVoIPCoexist = GetConfig().GetBool("LAA_FTP_VoIP_Coexist");
		int iFTPLAAUENum = GetConfig().GetInt("LAA_UE_Number");//*GetConfig().GetDouble("LAA_FTP_UE_Percentage")+0.5 );//四舍五入取整
		int iVoIPLAAUENum = 0;//floor( GetConfig().GetInt("LAA_UE_Number")*GetConfig().GetDouble("LAA_VoIP_UE_Percentage")+0.5 );
		for (int iUEIndexa = 0; iUEIndexa < iLAAUENumber; iUEIndexa++)
		{
			int AssociateAPID = iUEIndexa % iLAANumber;
			POS relativePOS = instLAAMap.GenerateUEPOS(dLAARadius);	//UE相对于所在AP的坐标
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
		
	//	//代表LAA的虚拟UE
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
		double freq = GetConfig().GetDouble("Carrier_Frequency(MHz)");								//载频
		//double hNodeB = GetConfig().GetDouble("HEIGHT_OF_Nodeb_ANTENNA(m)");	//基站高度
		//double hUE = GetConfig().GetDouble("HEIGHT_OF_UE_ANTENNA(m)");				//UE高度
		//bool CentralCityAdjustment = GetConfig().GetBool("Metropolitan_Area");					//是否在市区
		double wififreq = GetConfig().GetDouble("WiFi_Carrier_Frequency(MHz)");
		double laafreq = GetConfig().GetDouble("LAA_Carrier_Frequency(MHz)");
		m_pLTEPathLoss.reset(new UMa_NLOS(freq));
		//m_pLTEPathLoss.reset(new UMi_NLOS(freq));
		//m_pLTEPathLoss.reset(new Cost231_Hata(freq));
		m_pHSPAPathLoss.reset(new HSPACost231_Hata());
		m_pWiFiPathLoss.reset(new WiFiPathLoss(wififreq));
		m_pLAAPathloss.reset(new LAAPathloss(laafreq));
	}
#pragma region Fading表
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
#pragma endregion Fading表
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
#pragma endregion 初始化

	//运行仿真timespan个时间单位  改到这里了
	void System::Run( int timespan )
	{
		const string& s = GetConfig().GetString("FRAME_CONFIGURATION");
		int iFramePerHighLayerUpdate = GetFramesPerHighLayerClockUpdate();
		for (int iFrame = 0; iFrame < timespan; ++iFrame)
		{
			//ClockManager::Instance().GetFrameClock().Update();
			//每iFramePerHighLayerUpdate，HighLayerClock进行更新一次
			//if ( iFrame % iFramePerHighLayerUpdate == 0 )
			//	ClockManager::Instance().GetHighLayerClock().Update();
			//if( iFrame == 0 )
			//	ClockManager::Instance().GetTxClock().Update();  //为了执行分割数据的命令

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
			//累加仿真时间，以帧为单位
			++m_time;
			//慢衰的更新时间也以帧为单位，经过ShadowUpdatePeriod帧进行更新
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

		//To Be Modified -- 加入双模UE的吞吐量的统计
		//双模UE的吞吐量需要分别统计在两个系统中Service的吞吐量，以及一个总的吞吐量
		for( DUALMODELTEHSPAUELIST::iterator dualModeUEIter = m_DualModeLTEHSPAUEList.begin(); dualModeUEIter != m_DualModeLTEHSPAUEList.end(); ++dualModeUEIter )
		{
			//dualModeUEIter->second->GetService( SYSTEM_LTE )->Accept( *m_pThrStatistic );
			//dualModeUEIter->second->GetService( SYSTEM_HSPA )->Accept( *m_pHSPAThrStatistic );
			dualModeUEIter->second->GetService( SYSTEM_LTE )->Accept( *m_pDualModeThrStatisticLTEPart );
			dualModeUEIter->second->GetService( SYSTEM_HSPA )->Accept( *m_pDualModeThrStatisticHSPAPart );
		}

		/*除以10相当于除以10ms再乘以1k，得到的单位是Kbps*/
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
			nbMCS[pNB->second->GetID()] = nbMCSsta.GetStatistic();	//NodeB的MCS统计

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
			//HSPA NodeB的ID是在LTE NodeB的ID的基础上增加的
			HSPAnbthr[pNB->second->GetID() % iLTENBNum] = nbthrsta.GetSumStatistic();
			HSPAnbMCS[pNB->second->GetID() % iLTENBNum] = HSPAnbMCSsta.GetStatistic();	//HSPA NodeB的MCS统计
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

		//生成结果
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

	//系统运行时间
	int System::GetTotalTime() const
	{
		return m_time;
	}
	//系统收敛时间
	int System::GetConvergeTime() const
	{
		return GetConfig().GetInt("Converge_Time(frame)");
	}
	//系统统计时间
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

	//获得一个Drop的结果
	const shared_ptr<SystemResult> System::GetResult() const
	{
		return m_pResult;
	}

#pragma endregion 主流程


#pragma region 终端管理

	//加入LTE UE
	bool System::AddLTEUE( const System::UEPTR pUE )
	{
		using std::make_pair;
		if (m_uelist.count(pUE->GetID()))	//UE是否已经存在
		{
			std::stringstream ss;
			ss << "There is already a LTE UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_uelist.insert(make_pair(pUE->GetID(), pUE));			//加入新UE

		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForAllClock( pTimeClient );	//注册新UE的时钟
		pUE->Attach(shared_from_this());	//将System传给UE
		return true;
	}

	//加入HSPA UE
	bool System::AddHSPAUE(const HSPAUEPTR pUE)
	{
		using std::make_pair;
		if (m_HSPAUEList.count(pUE->GetID()))	//UE是否已经存在
		{
			std::stringstream ss;
			ss << "There is already a HSPA UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_HSPAUEList.insert(make_pair(pUE->GetID(), pUE));	//加入新UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForAllClock( pTimeClient );	//注册新UE的时钟
		pUE->Attach(shared_from_this());	//将System传给UE
		return true;
	}

	//加入Dual Mode UE
	bool System::AddDualModeLTEHSPAUE(const DUALMODELTEHSPAUEPTR pUE )
	{
		using std::make_pair;
		if (m_DualModeLTEHSPAUEList.count(pUE->GetID()))	//UE是否已经存在
		{
			std::stringstream ss;
			ss << "There is already a DualMode UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_DualModeLTEHSPAUEList.insert(make_pair(pUE->GetID(), pUE));	//加入新UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForAllClock( pTimeClient );	//注册新UE的时钟
		pUE->Attach(shared_from_this());	//将System传给UE
		return true;
	}

	//加入WiFi UE
	bool System::AddWiFiUE(const WiFiUEPTR pUE)
	{
		using std::make_pair;
		if (m_WiFiUEList.count(pUE->GetID()))	//UE是否已经存在
		{
			std::stringstream ss;
			ss << "There is already a WiFi UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_WiFiUEList.insert(make_pair(pUE->GetID(), pUE));	//加入新UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForWiFiClock( pTimeClient );	//注册新UE的时钟
		pUE->Attach(shared_from_this());	//将System传给UE
		return true;
	}
	//加入LAA UE
	bool System::AddLAAUE(const LAAUEPTR pUE)
	{
		using std::make_pair;
		if (m_LAAUEList.count(pUE->GetID()))	//UE是否已经存在
		{
			std::stringstream ss;
			ss << "There is already a LAA UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_LAAUEList.insert(make_pair(pUE->GetID(), pUE));	//加入新UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForLAAClock( pTimeClient );	//注册新UE的时钟
		pUE->Attach(shared_from_this());	//将System传给UE
		return true;
	}

	bool System::AddLTEWiFiUE(const LTEWiFiUEPTR pUE)
	{
		using std::make_pair;
		if (m_LTEWiFiUEList.count(pUE->GetID()))	//UE是否已经存在
		{
			std::stringstream ss;
			ss << "There is already a LTEWiFi UE with ID " << pUE->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_LTEWiFiUEList.insert(make_pair(pUE->GetID(), pUE));	//加入新UE
		shared_ptr<TimeClient> pTimeClient = dynamic_pointer_cast<TimeClient>( pUE );
		ClockManager::Instance().RegisterForAllClock( pTimeClient );	//注册新UE的时钟
		ClockManager::Instance().RegisterForWiFiClock( pTimeClient );	//注册到WiFi时钟
		pUE->Attach(shared_from_this());	//将System传给UE
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

	//获取UE
	System::UEPTR System::GetUE( int ID )
	{
		if (m_uelist.count(ID) == 1)
		{
			return m_uelist[ID];
		}
		USER_MESSAGE("Can't find UE " << ID);
		return ILTEUE::PTR();	//返回空指针
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

	//获取UE列表
	const System::UELIST& System::GetUEList() const
	{
		return m_uelist;
	}

	const System::HSPAUELIST& System::GetHSPAUEList() const
	{
		return m_HSPAUEList;
	}

	//删除UE
	System::UEPTR System::RemoveUE( int ID )
	{
		ILTEUE::PTR pUE = GetUE(ID);
		m_nblist.erase(ID);
		return pUE;
	}

	//清除UEList
	void System::ClearUE()
	{
		m_uelist.clear();
	}

#pragma endregion 终端管理

#pragma region NodeB管理

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

	//加入NodeB
	bool System::AddNodeB( const System::NBPTR pNodeB )
	{
		using std::make_pair;
		if (m_nblist.count(pNodeB->GetID()))	//判断该NodeB是否存在
		{
			std::stringstream ss;
			ss << "There is already a NodeB with ID " << pNodeB->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_nblist.insert(make_pair(pNodeB->GetID(), pNodeB));	//加入新NodeB
		pNodeB->Attach(shared_from_this());	//使NodeB与System连接
		ClockManager::Instance().RegisterForAllClock( pNodeB );	//注册新NodeB的时钟
		return true;
	}

	bool System::AddHSPANodeB(const LTESim::System::HSPANBPTR pNodeB)
	{
		using std::make_pair;
		if (m_HSPANBList.count(pNodeB->GetID()))								//判断该NodeB是否存在
		{
			std::stringstream ss;
			ss << "There is already a NodeB with ID " << pNodeB->GetID() << endl;
			std::string s = ss.str();
			throw std::logic_error(s);
		}
		m_HSPANBList.insert(make_pair(pNodeB->GetID(), pNodeB));	//加入新NodeB
		pNodeB->Attach(shared_from_this());											//使NodeB与System连接
		ClockManager::Instance().RegisterForAllClock( pNodeB );				//注册新NodeB的时钟
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
	//获得NodeB列表
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

	//获取NodeB
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
	//删除NodeB
	System::NBPTR System::RemoveNodeB( int ID )
	{
		NodeB::PTR pNodeB = GetNodeB(ID);
		m_nblist.erase(ID);
		return pNodeB;
	}

	//清除nodeBList
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

#pragma region 系统参数

	int System::GetLTESubFramesPerFrame() const
	{
		//1个帧含有10个子帧
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

	//当前配置参数
	const RunParameters& System::GetConfig() const
	{
		return *m_pParameters;
	}
	//MCS表
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
	//AMC表
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

	//快衰落
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
	//慢衰
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
	//路损
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
	//系统的坐标信息
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
	//系统判决器
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
	//底噪密度
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

#pragma region 静态的全局访问点

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

#pragma endregion 静态的全局访问点

}



