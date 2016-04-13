#pragma once
#include <map>
#include <iostream>
#include <fstream>
#include <boost/enable_shared_from_this.hpp>

using std::map;
namespace LTESim
{
class UEMaker;
class NodeBMaker;
class HighLayerControllerMaker;
class SystemResult;
class RunParameters;
class AMCTable;
class MCSTable;
class NodeB;
class IUE;
class ILTEUE;
class HSPANodeB;
class IHSPAUE;
class HSPAUE;
class IDualModeLTEHSPAUE;
class HighLayerController;
//class ServiceComposite;
class DB;
class Fading;
class SlowFading;
class Pathloss;
class MCS;
class Decisioner;
class Map;
class ServiceStatistic;
class HSPAFading;
class WiFiAP;
class IWiFiUE;
class WiFiUE;
class WiFiFading;
class ILTEWiFiUE;
class LTEWiFiUE;
class LAANodeB;
class ILAAUE;
class LAAUE;
class LAAFading;

//每个Drop初始化一个System并运行
class System : public boost::enable_shared_from_this<System>
{
public:
    typedef shared_ptr<System> PTR;
	typedef shared_ptr<IUE> IUEPTR;
	typedef shared_ptr<NodeB> NBPTR;
	typedef shared_ptr<ILTEUE> UEPTR;
	typedef shared_ptr<HSPANodeB> HSPANBPTR;
	typedef shared_ptr<IHSPAUE> HSPAUEPTR;
	typedef shared_ptr<IDualModeLTEHSPAUE> DUALMODELTEHSPAUEPTR;
	typedef shared_ptr<HighLayerController> HIGHLAYERCONTROLLERPTR;
	typedef shared_ptr<WiFiAP> WiFiAPPTR;
	typedef shared_ptr<IWiFiUE> WiFiUEPTR;

    typedef shared_ptr<LAANodeB> LAANBPTR;
	typedef shared_ptr<ILAAUE> LAAUEPTR;

	typedef shared_ptr<ILTEWiFiUE> LTEWiFiUEPTR;
	typedef std::map<int, NBPTR > NBLIST;
	typedef std::map<int, UEPTR > UELIST;
	typedef std::map<int, HSPANBPTR> HSPANBLIST;
	typedef std::map<int, HSPAUEPTR> HSPAUELIST;
	typedef std::map<int, DUALMODELTEHSPAUEPTR> DUALMODELTEHSPAUELIST;
	typedef std::map<int, HIGHLAYERCONTROLLERPTR> HIGHLAYERCONTROLLERLIST;
	typedef std::map<int, WiFiAPPTR > WiFiAPLIST;
	typedef std::map<int, WiFiUEPTR > WiFiUELIST;
	typedef std::map<int, LTEWiFiUEPTR > LTEWiFiUELIST;

	typedef std::map<int, LAANBPTR > LAANBLIST;
	typedef std::map<int, LAAUEPTR > LAAUELIST;

	typedef std::map<DB, MCS> AMCTABLE;

public:
	System(const shared_ptr<const RunParameters>& parameters);
    virtual ~System(void);
	//获得当前有效的SYSTEM
	static shared_ptr<System> GetCurrentSystem();
	shared_ptr<System> UpdateCurrentSystem();
#pragma region 主要流程
    //初始化，生成服务区环境
    bool Init();
	void InitMap();
    void InitSINRCalculator();
    void InitArray();
    void InitUE();
	void InitNodeB();
	void InitHighLayerController();
	void InitTables();
	void InitPathLoss();
    void InitFastFading();
	void InitSlowFading();
	void InitDecisioner();

	//运行，可以反复调用，在时间上连续运行多次
    void Run(int timespan);

    //结束，清理，调用End后不得再Run
    void End();

    //输出报告
	const shared_ptr<SystemResult> GetResult() const;
#pragma endregion

#pragma region 终端管理函数
    //加入UE
	bool AddLTEUE(const UEPTR pUE);
	bool AddHSPAUE(const HSPAUEPTR pUE);
	bool AddDualModeLTEHSPAUE(const DUALMODELTEHSPAUEPTR pUE);
	bool AddWiFiUE(const WiFiUEPTR pUE);
	bool AddLTEWiFiUE(const LTEWiFiUEPTR pUE);
	bool AddLAAUE(const LAAUEPTR pUE);

	IUEPTR GetBaseUE( int iID );

    //通过ID得到UE指针
	UEPTR GetUE(int ID);
	UEPTR GetUEFromOffsetID(int iID);
	UEPTR RemoveUE(int ID);
    void ClearUE();
	const UELIST& GetUEList() const;

	HSPAUEPTR GetHSPAUE( int iID );
	HSPAUEPTR GetHSPAUEFromOffsetID( int iID );
	const HSPAUELIST& GetHSPAUEList() const;

	DUALMODELTEHSPAUEPTR GetDualModeLTEHSPAUE( int iID );
	DUALMODELTEHSPAUEPTR GetDualModeLTEHSPAUEFromOffsetID( int iID );

	LTEWiFiUEPTR GetLTEWiFiUE( int iID );
    WiFiUEPTR GetWiFiUE( int iID );

#pragma endregion 终端管理函数

#pragma  region 基站管理函数
	int ConvertNBOffsetIDToAbsoluteID( int iID, SystemType systemType );
	int ConvertNBAbsoluteIDToOffsetID( int iID, SystemType systemType );
	int ConvertUEOffsetIDToAbsoluteID( int iID, SystemType systemType );
	int ConvertUEAbsoluteIDToOffsetID( int iID, SystemType systemType );

	//加入NodeB
	bool AddNodeB(const NBPTR pNodeB);
	bool AddHSPANodeB(const HSPANBPTR pNodeB);
	bool AddWiFiAP(const WiFiAPPTR pAP);
	bool AddLAANB(const LAANBPTR pNoeB);

    //通过ID得到NodeB指针
	NBPTR GetNodeB(int ID);
	NBPTR GetNodeBFromOffsetID( int iID );
	NBPTR RemoveNodeB(int ID);
    void ClearNodeB();

	HSPANBPTR GetHSPANodeB(int iID);
	HSPANBPTR GetHSPANodeBFromOffsetID( int iID );
	WiFiAPPTR GetWiFiAP( int iID );
	WiFiAPPTR GetWiFiAPFromOffsetID( int iID );
	LAANBPTR GetLAANB( int iID );
	LAANBPTR GetLAANBFromOffsetID( int iID );

    const NBLIST& GetNBList() const;
	const HSPANBLIST& GetHSPANBList() const;
	const DUALMODELTEHSPAUELIST& GetDualModeLTEHSPAUEList() const;
	const WiFiAPLIST& GetWiFiAPList() const;
	const LAANBLIST& GetLAANBList() const;
	const LTEWiFiUELIST& GetLTEWiFiUEList() const;
	const WiFiUELIST& GetWiFiUEList() const;
	const LAAUELIST& GetLAAUEList() const;

	bool AddHighLayerController( const HIGHLAYERCONTROLLERPTR pHighLayerController );
	HIGHLAYERCONTROLLERPTR GetAssociateHighLayerController( NBPTR pLTENodeB );
	HIGHLAYERCONTROLLERPTR GetAssociateHighLayerController( HSPANBPTR pHSPANodeB );

protected:
	HIGHLAYERCONTROLLERPTR GetHighLayerController(int iID);
#pragma endregion 基站管理函数

#pragma region 获得系统的配置参数
public:
	//std::ofstream m_fout;	//调试
	//获得参数配置类
	const RunParameters& GetConfig() const;
    //获得子帧数
    int GetLTESubFramesPerFrame() const;
	int GetHSPASubFramePerFrame() const;
    //获得每子帧的时隙数
    int GetSlotsPerSubFrame() const;
	int GetStandardTimeSlicesPerHSPASubFrame() const;
	int GetFramesPerHighLayerClockUpdate() const;

	//得到MCS表
	const MCSTable& GetMCSTables() const;
	const MCSTable& GetHSPAMCSTables() const;
	const MCSTable& GetWiFiMCSTables() const;
	const MCSTable& GetLAAMCSTables() const;

	//得到AMC表
	const AMCTABLE& GetAMCTable() const;
	const AMCTABLE& GetHSPAAMCTable() const;
	const AMCTABLE& GetWiFiAMCTable() const;
	const AMCTABLE& GetLAAAMCTable() const;
	
	//得到fading的生成器
	const Fading& GetFading() const;
    const Fading& GetSlowFading() const;
	const HSPAFading& GetHSPAFading() const;
	const WiFiFading& GetWiFiFading() const;
	const WiFiFading& GetWiFiShadowFading() const;
	const LAAFading& GetLAAFading() const;
	const LAAFading& GetLAAShadowFading() const;

	//得到路损计算器
	const Pathloss& GetPathLoss(SystemType systemType) const;

	//经研究可以全局使用一个判决器
	const Decisioner& GetDecisioner( SystemType sysType) const;

	//得到小区拓扑生成器
	const Map& GetMap() const;
	//const Map& GetIndoorMap() const;
	const Map& GetWiFiMap() const;
	const Map& GetLAAMap() const;

	//底噪密度，还需要加上收端噪声系数
	double GetThermalDensity() const;
	double GetWiFiThermalDensity() const;
	double GetLAAThermalDensity() const;

	//得到RB的带宽
	double GetRBBandWidth() const;
	double GetWiFiBandWidth() const;
	double GetLAABandWidth() const;

	//本轮仿真的总时间，以帧为单位
	int GetTotalTime() const;

	//收敛时间，以帧为单位
	int GetConvergeTime() const;

	//统计时长，以帧为单位
	int GetStatisticTime() const;

	//判断是否进行统计，超出收敛时间后为true
	bool StatisticEnabled() const;
#pragma endregion


protected:
    NBLIST m_nblist;
    UELIST m_uelist;
	HSPANBLIST m_HSPANBList;
	HSPAUELIST m_HSPAUEList;
	DUALMODELTEHSPAUELIST m_DualModeLTEHSPAUEList;
	HIGHLAYERCONTROLLERLIST m_HighLayerControllerList;
	//WiFiAPPTR m_WiFiAP;
	WiFiAPLIST m_WiFiAPList;
	WiFiUELIST m_WiFiUEList;
	LTEWiFiUELIST m_LTEWiFiUEList;

	LAANBLIST m_LAANBList;
	LAAUELIST m_LAAUEList;

	//NodeB生成
    shared_ptr<NodeBMaker> m_nbmaker;
	//UE生成
    shared_ptr<UEMaker> m_uemaker;

	shared_ptr<HighLayerControllerMaker> m_pControllerMaker;
	//读入的参数
	const shared_ptr<const RunParameters> m_pParameters;

#pragma region 统计
	//吞吐量统计
	shared_ptr<ServiceStatistic> m_pThrStatistic;
	shared_ptr<ServiceStatistic> m_pHSPAThrStatistic;
	shared_ptr<ServiceStatistic> m_pDualModeThrStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pDualModeThrStatisticHSPAPart;
	shared_ptr<ServiceStatistic> m_pWiFiThrStatistic;
	shared_ptr<ServiceStatistic> m_pLAAThrStatistic;
	shared_ptr<ServiceStatistic> m_pLTEWiFiThrStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pLTEWiFiThrStatisticWiFiPart;

	//业务包传输时间统计
	shared_ptr<ServiceStatistic> m_pLTEPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pHSPAPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pWiFiPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pLAAPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pDualPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pLTEWiFiPacketTimeStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pLTEWiFiPacketTimeStatisticWiFiPart;

	//FullBuffer时，传输间隔统计
	shared_ptr<ServiceStatistic> m_pLTETransIntervalStatistic;
	shared_ptr<ServiceStatistic> m_pWiFiTransIntervalStatistic;
	shared_ptr<ServiceStatistic> m_pLAATransIntervalStatistic;

	//丢包率统计
	shared_ptr<ServiceStatistic> m_pLTEPacketLossStatistic;
	shared_ptr<ServiceStatistic> m_pWiFiPacketLossStatistic;
	shared_ptr<ServiceStatistic> m_pLAAPacketLossStatistic;
	shared_ptr<ServiceStatistic> m_pLTEWiFiPacketLossStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pLTEWiFiPacketLossStatisticWiFiPart;

	//信道利用率统计
	shared_ptr<ServiceStatistic> m_pLTEChannelUtilizationStatistic;
	shared_ptr<ServiceStatistic> m_pWiFiChannelUtilizationStatistic;
	shared_ptr<ServiceStatistic> m_pLAAChannelUtilizationStatistic;
	shared_ptr<ServiceStatistic> m_pLTEWiFiChannelUtilizationStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pLTEWiFiChannelUtilizationStatisticWiFiPart;

	//误包率统计
	shared_ptr<ServiceStatistic> m_pPERStatistic;

	//平均重传次数统计
	shared_ptr<ServiceStatistic> m_pRetranStatistic;

	//时延统计
	shared_ptr<ServiceStatistic> m_pDelayStatistic;

	//输出的结果
	shared_ptr<SystemResult>	 m_pResult;
#pragma endregion 统计

	//服务区拓扑结构
	shared_ptr<Map> m_pMap;
	shared_ptr<Map> m_pWiFiMap;
	shared_ptr<Map> m_pLAAMap;
	
	//MCS对应的BLER曲线表
	shared_ptr<MCSTable> m_pMCSTables;
	shared_ptr<MCSTable> m_pHSPAMCSTables;
	shared_ptr<MCSTable> m_pWiFiMCSTables;
	shared_ptr<MCSTable> m_pLAAMCSTables;

	//AMC对应门限表
	shared_ptr<AMCTable> m_pAMCTable;
	shared_ptr<AMCTable> m_pHSPAAMCTable;
	shared_ptr<AMCTable> m_pWiFiAMCTable;
	shared_ptr<AMCTable> m_pLAAAMCTable;

	//快衰生成器
	shared_ptr<Fading> m_pFastFading;
	shared_ptr<HSPAFading> m_pHSPAFastFading;
	shared_ptr<WiFiFading> m_pWiFiFastFading;
	shared_ptr<LAAFading> m_pLAAFastFading;
	
	//慢衰生成器
	shared_ptr<SlowFading> m_pSlowFading;
	shared_ptr<WiFiFading> m_pWiFiShadowFading;
	shared_ptr<LAAFading> m_pLAAShadowFading;

	//路损计算器
	shared_ptr<Pathloss> m_pLTEPathLoss;
	shared_ptr<Pathloss> m_pHSPAPathLoss;
	shared_ptr<Pathloss> m_pWiFiPathLoss;
	shared_ptr<Pathloss> m_pLAAPathloss;

	//判决器
	shared_ptr<Decisioner>	m_pLTEDecisioner;
	shared_ptr<Decisioner>	m_pHSPADecisioner;
	shared_ptr<Decisioner>	m_pWiFiDecisioner;
	shared_ptr<Decisioner>	m_pLAADecisioner;

	double m_ThermalDensity;//底噪密度
	double m_RBBandWidth;//RB的带宽，Hz
	double m_dWiFiBandWidth;//WiFi的带宽，Hz
	double m_dWiFiThermalDensity;//WiFi的底噪密度，实值

	double m_dLAABandWidth;//LAA的带宽，Hz
	double m_dLAAThermalDensity;//LAA的底噪密度，实值


	//保存当前有效的系统的静态成员变量
	static  shared_ptr<System> ms_pCurrentSystem;
	
	//总运行时间,单位 帧
	int m_time;

	
};
}

