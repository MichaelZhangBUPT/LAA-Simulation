/*用于LAA的调度，每TTI全频带调度一个UE，所以只在时间域上采用RR或者PF调度算法，频域方面不做分配
返回优先级最高的UE，到LAA的GetCandidateUEList()；2016.3.17HFR*/
#pragma once
#include "commondef.h"
#include "Service.h"
#include "LAANodeB.h"
#include "LAAUE.h"
#include <vector>

namespace LTESim
{
	class LAANodeB;
	class Service;
	class LAADownLinkScheduler
	{
	public:
		//typedef shared_ptr<LAAUE> LAAUEPTR;
		typedef std::vector< shared_ptr<Service> > SERVICELIST;
	public:
		LAADownLinkScheduler(void);

	public: 
		~LAADownLinkScheduler(void);
	public:
		shared_ptr<ILAAUE>  ScheduleProcess(shared_ptr <LAANodeB> pLAANodeB);//调度总函数
		void  GetCadidateServiceList(shared_ptr <LAANodeB> pLAANodeB);//获取所有待传业务
		void  CalculateServicePriorityForRR(SERVICELIST & CandidateServices);
		void  CalculateServicePriorityForPF(SERVICELIST & CandidateServices);
		void  SortServiceAccordingtoPriority(SERVICELIST & CandidateServices);//根据优先级高低进行排序
		shared_ptr<ILAAUE>  GetScheduledLAAUE();//返回优先级最高的一个UE
		void  clear();

	protected:
        
		
		SERVICELIST m_SelectedService;
        SERVICELIST m_CandidateServices;

		struct ServicePriority
		{
			shared_ptr<Service> pService;
			double priority;
			bool operator<(const struct ServicePriority  &c) const
			{ return (priority<=c.priority) ? false :true; };

		};
		vector<ServicePriority> m_servicePriorityList;

	};
}


