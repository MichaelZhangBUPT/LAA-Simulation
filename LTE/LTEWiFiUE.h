#pragma once

#include "commondef.h"
#include "CMDEngine.h"
#include "WiFiUE.h"
#include "UE.h"
#include "Clock.h"
#include "SystemElement.h"
#include "ClockProcessor.h"

#include <boost/enable_shared_from_this.hpp>

namespace LTESim
{
	class ServiceComposite;

	class ILTEWiFiUE : 
		public ILTEUE,
		public IWiFiUE
	{
	public:
		typedef boost::shared_ptr<ILTEWiFiUE> PTR;
	public:
		ILTEWiFiUE(){};
		virtual ~ILTEWiFiUE(){};
	};

	class LTEWiFiUE : 
		public ClockProcessor,
		public CMDEngine,
		public ILTEWiFiUE,
		public boost::enable_shared_from_this<LTEWiFiUE>,
		private SystemElement
	{
	public:
		typedef ::boost::shared_ptr<LTEWiFiUE> PTR;							//用户指针
		typedef boost::weak_ptr<NodeB> LTENODEBPTR;
		typedef boost::weak_ptr<WiFiAP> WiFiAPPTR;
		typedef std::list<shared_ptr<Service> > SERVICELIST;									//服务列表
		typedef std::map<int, shared_ptr<NodeB> > NBLIST;										//基站列表
		typedef std::set<shared_ptr<PhyResource> > PhyResourceList;						//资源列表，一个UE会申请多个资源块	
		typedef std::vector<double> SINRLIST;															//信噪比列表
		typedef std::vector<MCS> CQILIST;																//MCS表
	public:
		LTEWiFiUE( int iUEid, double dPosX, double dPosY, shared_ptr<Antenna> pAntenna);
		virtual ~LTEWiFiUE();

#pragma region IUE Interface

	public:
		virtual const POS& GetPos() const;
		virtual int GetID() const;
		virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys );

		virtual SystemType GetType() const;

		//UE初始化
		void Initialization();
		//获取噪声系数
		double GetNoiseFigure() const;
		/*返回UE的天线，在LTE系统中使用*/
		double GetWiFiNoiseFigure() const;

		shared_ptr<Antenna> GetRxAntenna() const;
		void SetAntenna( shared_ptr<Antenna> Aptr);

		//加入一个Service
		bool AddService(shared_ptr<Service> pService);
		//返回业务
		shared_ptr<Service> GetService( SystemType systemType );

		shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType );
		shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType );
		void UpdateDLHARQProcess( SystemType systemType );
		shared_ptr<ARQ> GetARQ() const;

#pragma endregion

#pragma region ILTE Interface

	public:
		//NodeB候选集
		const NBLIST& GetCandidateNodeBSet() const;
		/*得到当前服务的NodeB*/
		shared_ptr<NodeB> GetServingNodeB() const;
		/*将NodeB加入激活集*/
		void AddToActiveSet(const shared_ptr<NodeB> pNodeB);

		//接收一个信道，只处理当前时隙的接收，最后在合并判决
		void ReceiveChannel( const shared_ptr<Channel> pRxChannel );
		void MakeDecision( const shared_ptr<Channel> pRxChannel, int iframeno, int isubframeno);

		//计算所有RB的SINR以及CQI并存在传进去的参数里面 
		void MeasureSINRAndCQIofAllTheRB(SINRLIST &sinrlist, CQILIST &cqilist);

		void UpdateAverageThroughput( const shared_ptr<Channel> pChannel );
		//得到该时隙改用户的RB列表
		void	GetRBListofSlot(const PhyResourceList &SourceRBlist, PhyResourceList& DesRBList, int frame, int slot);
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
		//返回din,室内距离
		double GetDin() const;
		void SetDin(double din);

		bool GetBoolNear() const;
		void SetBoolNear(bool b);

#pragma endregion

#pragma region IWiFiUE Interface

	public:
		virtual shared_ptr<AMCEntity> GetWiFiAMCEntity() const;
		virtual void AddToActiveAP(const shared_ptr<WiFiAP> pAP);
		virtual shared_ptr<WiFiAP> GetServingAP() const;
		virtual const int GetBackoffTime() const;//获取退避时间
		virtual void BackoffTimeTick();//退避时间计时
		virtual void UpdateBackoffTime();//发生碰撞后，更新退避时间
		virtual void UpdateCollisionNum();

		void ReceiveChannel( const shared_ptr<WiFiChannel> pChannel );
		void MakeDecision( const shared_ptr<WiFiChannel> pChannel );
		void UpdateThroughput( const shared_ptr<WiFiChannel> pChannel );

#pragma endregion

#pragma region 时钟响应函数族

	public:
		/*相应时钟定时的接口函数*/
		virtual void OnClock(const Clock& clock);
	protected:
		//处理每子帧开始时需要处理的操作
		virtual void SubFrameProcess(const Clock& subframeclock);
		//处理每帧开始时需要处理的操作
		virtual void FrameProcess(const Clock& frameclock);
		//处理发送
		virtual void TxProcess(const Clock& clock);
		//处理接收
		virtual void RxProcess(const Clock& clock);
		void SlotProcess(const Clock& slotclock);
		void WiFiClockProcess(const Clock& wificlock);

#pragma endregion


#pragma region 成员变量

		LTENODEBPTR						m_pServingLTENodeB;	//当前服务的LTE系统NodeB
		WiFiAPPTR						m_pServingAP;		//所属AP
		shared_ptr<ServiceComposite>	m_pService;			//当前业务
		shared_ptr<Antenna>				m_pRxAntenna;		//天线
		//To Be Modified, 双模的UE的AMC是否共用？
		shared_ptr<AMCEntity>			m_pLTEAMCEntity;	//AMC实体
		shared_ptr<AMCEntity>			m_pWiFiAMCEntity;	//AMC实体

		shared_ptr<HARQEntity>			m_pLTEDLHARQEntity;	//LTE系统中的HARQ实体
		shared_ptr<ARQ>					m_pARQ;				//WiFi的ARQ
		double							m_dLTENoiseFigure;		//LTEUE噪声系数,实值
		double							m_dWiFiNoiseFigure;		//WiFiUE噪声系数,实值
		shared_ptr<Reporter>			m_CSIReporter;		//信道质量的报告管理器

		int								m_iBackoffTime;		//退避时间，单位us
		int								m_iCollisionNum;	//已碰撞次数

		double							m_dDin;	//室内距离
		bool							m_bNear;	//是否离基站比较近，PacketDynamic方案中选为LTE用户

#pragma endregion
	};

}

