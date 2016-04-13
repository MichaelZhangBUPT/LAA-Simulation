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

//ÿ��Drop��ʼ��һ��System������
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
	//��õ�ǰ��Ч��SYSTEM
	static shared_ptr<System> GetCurrentSystem();
	shared_ptr<System> UpdateCurrentSystem();
#pragma region ��Ҫ����
    //��ʼ�������ɷ���������
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

	//���У����Է������ã���ʱ�����������ж��
    void Run(int timespan);

    //��������������End�󲻵���Run
    void End();

    //�������
	const shared_ptr<SystemResult> GetResult() const;
#pragma endregion

#pragma region �ն˹�����
    //����UE
	bool AddLTEUE(const UEPTR pUE);
	bool AddHSPAUE(const HSPAUEPTR pUE);
	bool AddDualModeLTEHSPAUE(const DUALMODELTEHSPAUEPTR pUE);
	bool AddWiFiUE(const WiFiUEPTR pUE);
	bool AddLTEWiFiUE(const LTEWiFiUEPTR pUE);
	bool AddLAAUE(const LAAUEPTR pUE);

	IUEPTR GetBaseUE( int iID );

    //ͨ��ID�õ�UEָ��
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

#pragma endregion �ն˹�����

#pragma  region ��վ������
	int ConvertNBOffsetIDToAbsoluteID( int iID, SystemType systemType );
	int ConvertNBAbsoluteIDToOffsetID( int iID, SystemType systemType );
	int ConvertUEOffsetIDToAbsoluteID( int iID, SystemType systemType );
	int ConvertUEAbsoluteIDToOffsetID( int iID, SystemType systemType );

	//����NodeB
	bool AddNodeB(const NBPTR pNodeB);
	bool AddHSPANodeB(const HSPANBPTR pNodeB);
	bool AddWiFiAP(const WiFiAPPTR pAP);
	bool AddLAANB(const LAANBPTR pNoeB);

    //ͨ��ID�õ�NodeBָ��
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
#pragma endregion ��վ������

#pragma region ���ϵͳ�����ò���
public:
	//std::ofstream m_fout;	//����
	//��ò���������
	const RunParameters& GetConfig() const;
    //�����֡��
    int GetLTESubFramesPerFrame() const;
	int GetHSPASubFramePerFrame() const;
    //���ÿ��֡��ʱ϶��
    int GetSlotsPerSubFrame() const;
	int GetStandardTimeSlicesPerHSPASubFrame() const;
	int GetFramesPerHighLayerClockUpdate() const;

	//�õ�MCS��
	const MCSTable& GetMCSTables() const;
	const MCSTable& GetHSPAMCSTables() const;
	const MCSTable& GetWiFiMCSTables() const;
	const MCSTable& GetLAAMCSTables() const;

	//�õ�AMC��
	const AMCTABLE& GetAMCTable() const;
	const AMCTABLE& GetHSPAAMCTable() const;
	const AMCTABLE& GetWiFiAMCTable() const;
	const AMCTABLE& GetLAAAMCTable() const;
	
	//�õ�fading��������
	const Fading& GetFading() const;
    const Fading& GetSlowFading() const;
	const HSPAFading& GetHSPAFading() const;
	const WiFiFading& GetWiFiFading() const;
	const WiFiFading& GetWiFiShadowFading() const;
	const LAAFading& GetLAAFading() const;
	const LAAFading& GetLAAShadowFading() const;

	//�õ�·�������
	const Pathloss& GetPathLoss(SystemType systemType) const;

	//���о�����ȫ��ʹ��һ���о���
	const Decisioner& GetDecisioner( SystemType sysType) const;

	//�õ�С������������
	const Map& GetMap() const;
	//const Map& GetIndoorMap() const;
	const Map& GetWiFiMap() const;
	const Map& GetLAAMap() const;

	//�����ܶȣ�����Ҫ�����ն�����ϵ��
	double GetThermalDensity() const;
	double GetWiFiThermalDensity() const;
	double GetLAAThermalDensity() const;

	//�õ�RB�Ĵ���
	double GetRBBandWidth() const;
	double GetWiFiBandWidth() const;
	double GetLAABandWidth() const;

	//���ַ������ʱ�䣬��֡Ϊ��λ
	int GetTotalTime() const;

	//����ʱ�䣬��֡Ϊ��λ
	int GetConvergeTime() const;

	//ͳ��ʱ������֡Ϊ��λ
	int GetStatisticTime() const;

	//�ж��Ƿ����ͳ�ƣ���������ʱ���Ϊtrue
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

	//NodeB����
    shared_ptr<NodeBMaker> m_nbmaker;
	//UE����
    shared_ptr<UEMaker> m_uemaker;

	shared_ptr<HighLayerControllerMaker> m_pControllerMaker;
	//����Ĳ���
	const shared_ptr<const RunParameters> m_pParameters;

#pragma region ͳ��
	//������ͳ��
	shared_ptr<ServiceStatistic> m_pThrStatistic;
	shared_ptr<ServiceStatistic> m_pHSPAThrStatistic;
	shared_ptr<ServiceStatistic> m_pDualModeThrStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pDualModeThrStatisticHSPAPart;
	shared_ptr<ServiceStatistic> m_pWiFiThrStatistic;
	shared_ptr<ServiceStatistic> m_pLAAThrStatistic;
	shared_ptr<ServiceStatistic> m_pLTEWiFiThrStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pLTEWiFiThrStatisticWiFiPart;

	//ҵ�������ʱ��ͳ��
	shared_ptr<ServiceStatistic> m_pLTEPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pHSPAPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pWiFiPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pLAAPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pDualPacketTimeStatistic;
	shared_ptr<ServiceStatistic> m_pLTEWiFiPacketTimeStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pLTEWiFiPacketTimeStatisticWiFiPart;

	//FullBufferʱ��������ͳ��
	shared_ptr<ServiceStatistic> m_pLTETransIntervalStatistic;
	shared_ptr<ServiceStatistic> m_pWiFiTransIntervalStatistic;
	shared_ptr<ServiceStatistic> m_pLAATransIntervalStatistic;

	//������ͳ��
	shared_ptr<ServiceStatistic> m_pLTEPacketLossStatistic;
	shared_ptr<ServiceStatistic> m_pWiFiPacketLossStatistic;
	shared_ptr<ServiceStatistic> m_pLAAPacketLossStatistic;
	shared_ptr<ServiceStatistic> m_pLTEWiFiPacketLossStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pLTEWiFiPacketLossStatisticWiFiPart;

	//�ŵ�������ͳ��
	shared_ptr<ServiceStatistic> m_pLTEChannelUtilizationStatistic;
	shared_ptr<ServiceStatistic> m_pWiFiChannelUtilizationStatistic;
	shared_ptr<ServiceStatistic> m_pLAAChannelUtilizationStatistic;
	shared_ptr<ServiceStatistic> m_pLTEWiFiChannelUtilizationStatisticLTEPart;
	shared_ptr<ServiceStatistic> m_pLTEWiFiChannelUtilizationStatisticWiFiPart;

	//�����ͳ��
	shared_ptr<ServiceStatistic> m_pPERStatistic;

	//ƽ���ش�����ͳ��
	shared_ptr<ServiceStatistic> m_pRetranStatistic;

	//ʱ��ͳ��
	shared_ptr<ServiceStatistic> m_pDelayStatistic;

	//����Ľ��
	shared_ptr<SystemResult>	 m_pResult;
#pragma endregion ͳ��

	//���������˽ṹ
	shared_ptr<Map> m_pMap;
	shared_ptr<Map> m_pWiFiMap;
	shared_ptr<Map> m_pLAAMap;
	
	//MCS��Ӧ��BLER���߱�
	shared_ptr<MCSTable> m_pMCSTables;
	shared_ptr<MCSTable> m_pHSPAMCSTables;
	shared_ptr<MCSTable> m_pWiFiMCSTables;
	shared_ptr<MCSTable> m_pLAAMCSTables;

	//AMC��Ӧ���ޱ�
	shared_ptr<AMCTable> m_pAMCTable;
	shared_ptr<AMCTable> m_pHSPAAMCTable;
	shared_ptr<AMCTable> m_pWiFiAMCTable;
	shared_ptr<AMCTable> m_pLAAAMCTable;

	//��˥������
	shared_ptr<Fading> m_pFastFading;
	shared_ptr<HSPAFading> m_pHSPAFastFading;
	shared_ptr<WiFiFading> m_pWiFiFastFading;
	shared_ptr<LAAFading> m_pLAAFastFading;
	
	//��˥������
	shared_ptr<SlowFading> m_pSlowFading;
	shared_ptr<WiFiFading> m_pWiFiShadowFading;
	shared_ptr<LAAFading> m_pLAAShadowFading;

	//·�������
	shared_ptr<Pathloss> m_pLTEPathLoss;
	shared_ptr<Pathloss> m_pHSPAPathLoss;
	shared_ptr<Pathloss> m_pWiFiPathLoss;
	shared_ptr<Pathloss> m_pLAAPathloss;

	//�о���
	shared_ptr<Decisioner>	m_pLTEDecisioner;
	shared_ptr<Decisioner>	m_pHSPADecisioner;
	shared_ptr<Decisioner>	m_pWiFiDecisioner;
	shared_ptr<Decisioner>	m_pLAADecisioner;

	double m_ThermalDensity;//�����ܶ�
	double m_RBBandWidth;//RB�Ĵ���Hz
	double m_dWiFiBandWidth;//WiFi�Ĵ���Hz
	double m_dWiFiThermalDensity;//WiFi�ĵ����ܶȣ�ʵֵ

	double m_dLAABandWidth;//LAA�Ĵ���Hz
	double m_dLAAThermalDensity;//LAA�ĵ����ܶȣ�ʵֵ


	//���浱ǰ��Ч��ϵͳ�ľ�̬��Ա����
	static  shared_ptr<System> ms_pCurrentSystem;
	
	//������ʱ��,��λ ֡
	int m_time;

	
};
}

