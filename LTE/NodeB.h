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
	NodeB(		int id/*基站标示*/, 
					const POS& pos/*基站位置*/, 
					int RBPERSLOT/*每时隙的物理资源*/, 
					double maxtxpower/*最大发射功率*/ ,
					shared_ptr<Antenna> pAntenna);
public:
    virtual ~NodeB(void);
    
    //得到基站对改资源发送功率
	double  GetTXPower(const PhyResource & phyresource) const;    
    
    //继承自TimeClient的接口
    void OnClock(const Clock& clock);

    //处理UE接纳
    virtual bool RequireAccess(const shared_ptr<ILTEUE>& pUE);
public:
	shared_ptr<TDDownlinkScheduler> SetTDDLScheduler( shared_ptr<TDDownlinkScheduler> pTDScheduler );
	shared_ptr<FDDownlinkScheduler> SetFDDLScheduler( shared_ptr<FDDownlinkScheduler> pFDScheduler );
#pragma region UE管理
protected:
    //加入UE到需要管理的UE列表
    void AddUE(const shared_ptr<ILTEUE>& pUE);
	//移除特定的UE
    void RemoveUE(int ID);

public:
	const UELIST& GetUEList() const;
#pragma endregion
	
#pragma region 发送接收
public:
	//发送相关
	/*在指定时刻发送pChannel,指定时刻为与当前时刻相隔的TX Clock记数*/
	virtual void TransmittChannel( shared_ptr<Channel> pChannel, int TxTiming );

	//在指定时刻更新每个RB所对应的UE
    void UpdateUEForRB(vector <shared_ptr<ILTEUE> >& UEToRBList);

	//根据RB Index得到对应的UE,note：RB index应该是算上天线发射端口的总数（例如RBNUM=50，发射天线端口为2，那么RBindex范围应为1-100）
    shared_ptr<ILTEUE> GetUEToRB(int RB, int antennaPort) const;

#pragma endregion

#pragma region 时钟响应
protected:
    //处理每时隙开始时需要处理的操作，并视情况调用TxProcess或者RxProcess
    void SlotProcess(const Clock& slotclock);
    //处理每子帧开始时需要处理的操作
    void SubFrameProcess(const Clock& subframeclock);
    //处理每帧开始时需要处理的操作
    void FrameProcess(const Clock& frameclock);
    //处理发送
    void TxProcess(const Clock& clock);
    //处理接收
    void RxProcess(const Clock& clock);

#pragma endregion

#pragma region 资源管理
public:
	int GetRBPerSlot() const;
	
	//单位mw
	double GetMaxTxPower() const;

	double GetMaxTxPowerPerRBPerPort() const;

	//检测资源可用性，可以使用返回true
	virtual bool CheckResource( const PhyResourceList & ResList ) const;
	
	//分配资源，成功返回true
	virtual bool AllocateResource(const PhyResourceList & ResList );

	//检测资源可用性，可以使用返回true
	virtual bool CheckPower( const PhyResourceList & ResList );

	//分配资源，成功返回true
	virtual bool AllocatePower( const PhyResourceList & ResList );

	virtual void UpdateCellThroughput( double thr );
	virtual double GetCellThroughput();

	virtual void UpdateUsedRBNum( int num );
	virtual int GetUsedRBNum();

#pragma endregion

#pragma region 天线配置
public:
    shared_ptr<Antenna> GetTxAntenna() const;
    shared_ptr<Antenna> GetRxAntenna() const;
#pragma endregion

#pragma region	高层信息汇报

	void UpdateReportToHighLayer( );

#pragma endregion

#pragma region  调度器存储的信息
	public:
		void SetLastCandidateService( int id, int rbnum );
		map<int,int> GetLastCandidateService();
		void SetIsUpdateCSI(bool flag);
		bool GetIsUpdateCSI();
#pragma endregion 

#pragma region 成员变量

protected:
    UELIST m_uelist;//由此NodeB管理的UE

    //调度器指针(分为时域和频域）
    shared_ptr<TDDownlinkScheduler> m_pTDDLScheduler;
    shared_ptr<FDDownlinkScheduler> m_pFDDLScheduler;
	//业务信道发送天线
	shared_ptr<Antenna>						m_pTxAntenna;

	//资源配置
	const int											m_RBPERSLOT;			//不算天线端口的RB数
	const double									m_MAXTxPower;
	shared_ptr<ResManager>				m_pResManager;
	shared_ptr<PowerManager>			m_pPowerManager;
	//数据统计
	shared_ptr<ServiceStatistic>			m_pServiceStatisticManager;
	vector<shared_ptr<ILTEUE> >			m_UEToRBList;
	map<int,int>						m_LastCandidateService;	//被调度的最后的UE的ID与所分配的RB数,用来存储分配RB不够所需RB的用户，最多只有一个
	double								m_dCellThroughput;
	int									m_iUsedRBNum;	//总共占用的资源数，用来计算信道利用率
	bool								m_bUpdateCSI;	//UE是否执行UpdateCSI函数的标记，如果都没有业务就不执行
#pragma endregion 
};

}

