/*����LAA�ĵ��ȣ�ÿTTIȫƵ������һ��UE������ֻ��ʱ�����ϲ���RR����PF�����㷨��Ƶ���治������
�������ȼ���ߵ�UE����LAA��GetCandidateUEList()��2016.3.17HFR*/
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
		shared_ptr<ILAAUE>  ScheduleProcess(shared_ptr <LAANodeB> pLAANodeB);//�����ܺ���
		void  GetCadidateServiceList(shared_ptr <LAANodeB> pLAANodeB);//��ȡ���д���ҵ��
		void  CalculateServicePriorityForRR(SERVICELIST & CandidateServices);
		void  CalculateServicePriorityForPF(SERVICELIST & CandidateServices);
		void  SortServiceAccordingtoPriority(SERVICELIST & CandidateServices);//�������ȼ��ߵͽ�������
		shared_ptr<ILAAUE>  GetScheduledLAAUE();//�������ȼ���ߵ�һ��UE
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


