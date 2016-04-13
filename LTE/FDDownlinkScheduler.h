#pragma once
#include <list>
#include "DownlinkScheduler.h"
#include <map>
namespace LTESim
{
	class DownlinkScheduler;
	class Service;
	class Channel;
	class FDDownlinkScheduler :public LTESim::DownlinkScheduler
	{
	public:
		FDDownlinkScheduler( shared_ptr<NodeB>& pNodeB );
	public:
		virtual ~FDDownlinkScheduler(void);


		//执行一次调度过程
		void Process(SERVICELIST service);

		void InitChannels();
		void GetCadidateServiceList();
		////获得调度上的业务列表
		//virtual void GetScheduledService();
	protected:
		virtual void CalculateServicePriority( shared_ptr<Service> pService, int RBindex );
		void SortServiceAccordingtoPriority();	//将服务按照权值排序
		//void ClearScheduleIndicator(SERVICELIST& CandidateServices);
		void ReserveRBForReTranService(SERVICELIST& CandidateServices);
		void AssignRBIndicator();
		size_t  SelectMCS(shared_ptr<Service> pService);
		/*virtual void PreAllocateResources( SERVICELIST& CandidateServices );*/
		virtual void AddScheduledChannelsToNodeB();
		/*virtual shared_ptr<Channel> GetSheduledChannel( shared_ptr<Service> pService );*/
		//得到预计发送数据的时刻，返回的是距当前时刻的slot数
		virtual int GetDataChannelTiming( shared_ptr<Channel> pDataChannel ) const;

		virtual void Clear();

		virtual void UpdateScheduledUEForRB();
	protected:
		//sort函数需要的结构体
		struct ServicePriority
		{
			shared_ptr<Service> pService;
			double priority;
			int RBindex; //占的RB号
			bool operator<(const struct ServicePriority  &c) const
			{ return (priority<=c.priority) ? false :true; };

		};

		std::vector<shared_ptr<Channel> > m_ScheduledChannels;

		//数据发送时刻相距于现在的SLOT时间间隔（含当前时隙）
		std::vector<int> m_DataTiming;

		std::map<shared_ptr<Service>, std::list<int> > m_serviceRBList;//每个业务对应的占用RB列表
		//vector<ServicePriority> m_serviceList;  //把每个RB上挑选出来的业务结构体存成一个表
		//std::map<shared_ptr<Service>, size_t> m_ReTranServiceRBSize;//存储要重传的业务和需要的RB数目


		std::list<int> ReserveRBList;        //记录还没有备选的RB下标
		vector<ServicePriority> m_servicePriorityList; 
		int m_iRBNumPerVoIPUE;
		int m_iRBNumPerUE;
		int m_iRBNum;
	};

}
