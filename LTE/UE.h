#pragma once
#include <set>
#include <list>
#include <boost/enable_shared_from_this.hpp>
#include "commondef.h"
#include "Clock.h"
#include "CMDEngine.h"
#include "POS.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
#include "Antenna.h"
#include "HARQ.h"
#include "PhyResource.h"
#include "Channel.h"
#include "IUE.h"

namespace LTESim
{
	class System;
	class NodeB;
	class Channel;
	class Service;
	class ReporterImp;
	class Reporter;
	class HARQEntity;
	typedef std::list<shared_ptr<Service> > SERVICELIST;

	class ILTEUE : 
		virtual public IUE
	{
	public:
		typedef weak_ptr<NodeB> NODEBPTR;
		typedef ::boost::shared_ptr<ILTEUE> PTR;							//用户指针
		typedef std::map<int, shared_ptr<NodeB> > NBLIST;				//基站列表
		typedef std::vector<double> SINRLIST;									//信噪比列表
		typedef std::vector<MCS> CQILIST;										//MCS表
	public:
		ILTEUE(){};
		virtual ~ILTEUE(){};
	public:
		//NodeB候选集
		virtual const NBLIST& GetCandidateNodeBSet() const = 0;
		/*得到当前服务的NodeB*/
		virtual shared_ptr<NodeB> GetServingNodeB() const = 0;
		/*将NodeB加入激活集*/
		virtual void AddToActiveSet(const shared_ptr<NodeB> pNodeB) = 0;

		//Interface Reporter
		//获得SINR和CQI的报告
		virtual shared_ptr<Reporter> GetReporter() const = 0;
		//更新SINR和CQI的报告
		virtual void UpdateReport( const SINRLIST& sinrlist, const CQILIST& cqilist) = 0;

		virtual void ReceiveChannel( const shared_ptr<Channel> pRxChannel ) = 0;

		virtual void UpdateAverageThroughput( const shared_ptr<Channel> pChannel ) = 0;
		//获得din，室内距离
		virtual double GetDin() const = 0;
		virtual void SetDin( double din ) = 0;
	};

	class UE : 
		public CMDEngine, 
		public ClockProcessor,
		public ILTEUE,
		public boost::enable_shared_from_this<UE>,
		private SystemElement
	{

	public:
		typedef std::list<shared_ptr<Service> > SERVICELIST;				//服务列表
		typedef weak_ptr<NodeB> NODEBPTR;
		typedef boost::shared_ptr<UE> PTR;										//用户指针
		typedef std::map<int, shared_ptr<NodeB> > NBLIST;					//基站列表
		typedef std::set<shared_ptr<PhyResource> > PhyResourceList;	//资源列表，一个UE会申请多个资源块	
		typedef std::vector<double> SINRLIST;										//信噪比列表
		typedef std::vector<MCS> CQILIST;											//MCS表

#pragma region 构造析构
	public:
		UE(int ueid/*UE标示*/, const POS& pos/*UE位置*/, shared_ptr<Antenna> pRxAntenna);
		virtual ~UE(void);
#pragma endregion 

		//Interface SystemElement
		virtual const POS& GetPos() const;
		virtual int GetID() const;
		virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys );

		virtual SystemType GetType() const;

		//UE初始化
		void Initialization();
		//获取噪声系数
		double GetNoiseFigure() const;
		/*返回UE的天线*/
		shared_ptr<Antenna> GetRxAntenna() const;

		//加入一个Service
		bool AddService(shared_ptr<Service> pService);
		//返回业务
		shared_ptr<Service> GetService( SystemType systemType );
		//返回din,室内距离
		double GetDin() const;
		void SetDin(double din);

#pragma region Relative eNodeB
		//NodeB候选集
		const NBLIST& GetCandidateNodeBSet() const;
		/*得到当前服务的NodeB*/
		shared_ptr<NodeB> GetServingNodeB() const;
		/*将NodeB加入激活集*/
		void AddToActiveSet(const shared_ptr<NodeB> pNodeB);
#pragma endregion

#pragma region 接收
		//接收一个信道，只处理当前时隙的接收，最后在合并判决
		void ReceiveChannel( const shared_ptr<Channel> pRxChannel );
		void MakeDecision( const shared_ptr<Channel> pRxChannel, int iframeno, int isubframeno);
		void UpdateAverageThroughput( const shared_ptr<Channel> pChannel );
#pragma endregion 

#pragma region 计算SINR
	public:
		void MeasureSINRAndCQIofAllTheRB(SINRLIST &sinrlist, CQILIST &cqilist);//计算所有RB的SINR以及CQI并存在传进去的参数里面
#pragma endregion 

#pragma region 报告
	public:
		//得到该时隙改用户的RB列表
		void GetRBListofSlot(const PhyResourceList &SourceRBlist, PhyResourceList& DesRBList, int frame, int slot);
		//统计吞吐量
		void UpdateThroughput(const shared_ptr<Channel> pchannel);
		//统计丢弃数据包的吞吐量
		void UpdateDiscardThroughput(const shared_ptr<Channel> pchannel);
		//更新UE的信道质量，用于NodeB端调度取得，参数表示信道信息是否具有时延，delayslotnum=0表示用本TTI信道信息，以此类推
		void UpdateCSI(int delayslotnum);
		//更新MCS等级  YRM
		void UpdateMCSIndex(const shared_ptr<Channel> pchannel);

		//更新SINR和CQI的报告
		void UpdateReport( const SINRLIST & sinrlist, const CQILIST& cqilist);
		//获得SINR和CQI的报告
		shared_ptr<Reporter> GetReporter() const;
#pragma endregion 

#pragma region HARQ
	public:
		shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType );
		shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType );
		void UpdateDLHARQProcess( SystemType systemType );
#pragma endregion 

#pragma region 时钟响应函数族
	public:
		/*相应时钟定时的接口函数*/
		void OnClock(const Clock& clock);
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

#pragma region 成员变量
	protected:
		NODEBPTR						m_pServingNodeB;				//当前服务小区
		shared_ptr<Service>				m_pService;							//当前业务
		shared_ptr<Antenna>			m_pRxAntenna;					//天线

		shared_ptr<Reporter>			m_CSIReporter;				    //信道质量的报告管理器
		shared_ptr<AMCEntity>		m_AMCEntity;						//AMC实体
		shared_ptr<HARQEntity>		m_DLHARQEntity;				//HARQ实体

		double									m_dNoiseFigure;						//UE噪声系数
		double						m_dDin;	//室内距离
#pragma endregion 
	};

}


