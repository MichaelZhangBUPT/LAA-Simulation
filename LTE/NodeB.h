#pragma once
#include <map>
#include <set>
#include <boost/enable_shared_from_this.hpp>
#include "Clock.h"
#include "CMDEngine.h"
#include "POS.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
#include "Antenna.h"
#include "PhyResource.h"
#include "Reporter.h"
#include "TDDownlinkScheduler.h"
#include "FDDownlinkScheduler.h"

namespace LTESim
{
class ILTEUE;
class Scheduler;
class DownlinkScheduler;
class TDDownLinkScheduler;
class FDDownLinkScheduler;
class Antenna;
class Channel;
class System;
class ResManager;
class PowerManager;
class ServiceStatistic;
class LTENBReport;

class NodeB : public CMDEngine, public ClockProcessor, public SystemElement, public boost::enable_shared_from_this<NodeB>
{
public:
    typedef weak_ptr<ILTEUE> UEPTR;
	typedef std::list< shared_ptr<Channel> > CHANNELLIST;
	typedef std::map<int, UEPTR> UELIST;
	typedef std::set< shared_ptr<PhyResource> > PhyResourceList;	
public:
    typedef shared_ptr<NodeB> PTR;
	NodeB(		int id/*��վ��ʾ*/, 
					const POS& pos/*��վλ��*/, 
					int RBPERSLOT/*ÿʱ϶��������Դ*/, 
					double maxtxpower/*����书��*/ ,
					shared_ptr<Antenna> pAntenna);
public:
    virtual ~NodeB(void);
    
    //�õ���վ�Ը���Դ���͹���
	double  GetTXPower(const PhyResource & phyresource) const;    
    
    //�̳���TimeClient�Ľӿ�
    void OnClock(const Clock& clock);

    //����UE����
    virtual bool RequireAccess(const shared_ptr<ILTEUE>& pUE);
public:
	shared_ptr<TDDownlinkScheduler> SetTDDLScheduler( shared_ptr<TDDownlinkScheduler> pTDScheduler );
	shared_ptr<FDDownlinkScheduler> SetFDDLScheduler( shared_ptr<FDDownlinkScheduler> pFDScheduler );
#pragma region UE����
protected:
    //����UE����Ҫ�����UE�б�
    void AddUE(const shared_ptr<ILTEUE>& pUE);
	//�Ƴ��ض���UE
    void RemoveUE(int ID);

public:
	const UELIST& GetUEList() const;
#pragma endregion
	
#pragma region ���ͽ���
public:
	//�������
	/*��ָ��ʱ�̷���pChannel,ָ��ʱ��Ϊ�뵱ǰʱ�������TX Clock����*/
	virtual void TransmittChannel( shared_ptr<Channel> pChannel, int TxTiming );

	//��ָ��ʱ�̸���ÿ��RB����Ӧ��UE
    void UpdateUEForRB(vector <shared_ptr<ILTEUE> >& UEToRBList);

	//����RB Index�õ���Ӧ��UE,note��RB indexӦ�����������߷���˿ڵ�����������RBNUM=50���������߶˿�Ϊ2����ôRBindex��ΧӦΪ1-100��
    shared_ptr<ILTEUE> GetUEToRB(int RB, int antennaPort) const;

#pragma endregion

#pragma region ʱ����Ӧ
protected:
    //����ÿʱ϶��ʼʱ��Ҫ����Ĳ����������������TxProcess����RxProcess
    void SlotProcess(const Clock& slotclock);
    //����ÿ��֡��ʼʱ��Ҫ����Ĳ���
    void SubFrameProcess(const Clock& subframeclock);
    //����ÿ֡��ʼʱ��Ҫ����Ĳ���
    void FrameProcess(const Clock& frameclock);
    //������
    void TxProcess(const Clock& clock);
    //�������
    void RxProcess(const Clock& clock);

#pragma endregion

#pragma region ��Դ����
public:
	int GetRBPerSlot() const;
	
	//��λmw
	double GetMaxTxPower() const;

	double GetMaxTxPowerPerRBPerPort() const;

	//�����Դ�����ԣ�����ʹ�÷���true
	virtual bool CheckResource( const PhyResourceList & ResList ) const;
	
	//������Դ���ɹ�����true
	virtual bool AllocateResource(const PhyResourceList & ResList );

	//�����Դ�����ԣ�����ʹ�÷���true
	virtual bool CheckPower( const PhyResourceList & ResList );

	//������Դ���ɹ�����true
	virtual bool AllocatePower( const PhyResourceList & ResList );

	virtual void UpdateCellThroughput( double thr );
	virtual double GetCellThroughput();

	virtual void UpdateUsedRBNum( int num );
	virtual int GetUsedRBNum();

#pragma endregion

#pragma region ��������
public:
    shared_ptr<Antenna> GetTxAntenna() const;
    shared_ptr<Antenna> GetRxAntenna() const;
#pragma endregion

#pragma region	�߲���Ϣ�㱨

	void UpdateReportToHighLayer( );

#pragma endregion

#pragma region  �������洢����Ϣ
	public:
		void SetLastCandidateService( int id, int rbnum );
		map<int,int> GetLastCandidateService();
		void SetIsUpdateCSI(bool flag);
		bool GetIsUpdateCSI();
#pragma endregion 

#pragma region ��Ա����

protected:
    UELIST m_uelist;//�ɴ�NodeB�����UE

    //������ָ��(��Ϊʱ���Ƶ��
    shared_ptr<TDDownlinkScheduler> m_pTDDLScheduler;
    shared_ptr<FDDownlinkScheduler> m_pFDDLScheduler;
	//ҵ���ŵ���������
	shared_ptr<Antenna>						m_pTxAntenna;

	//��Դ����
	const int											m_RBPERSLOT;			//�������߶˿ڵ�RB��
	const double									m_MAXTxPower;
	shared_ptr<ResManager>				m_pResManager;
	shared_ptr<PowerManager>			m_pPowerManager;
	//����ͳ��
	shared_ptr<ServiceStatistic>			m_pServiceStatisticManager;
	vector<shared_ptr<ILTEUE> >			m_UEToRBList;
	map<int,int>						m_LastCandidateService;	//�����ȵ�����UE��ID���������RB��,�����洢����RB��������RB���û������ֻ��һ��
	double								m_dCellThroughput;
	int									m_iUsedRBNum;	//�ܹ�ռ�õ���Դ�������������ŵ�������
	bool								m_bUpdateCSI;	//UE�Ƿ�ִ��UpdateCSI�����ı�ǣ������û��ҵ��Ͳ�ִ��
#pragma endregion 
};

}

