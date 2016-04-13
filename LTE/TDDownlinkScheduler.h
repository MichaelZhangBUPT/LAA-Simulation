#pragma once
#include "DownlinkScheduler.h"
namespace LTESim
{
	class DownlinkScheduler;
	class Service;
	class TDDownlinkScheduler :public LTESim::DownlinkScheduler
	{
	public:
		TDDownlinkScheduler( shared_ptr<NodeB>& pNodeB );
	public:
		virtual ~TDDownlinkScheduler(void);
		virtual SERVICELIST Process(void );
		virtual void CalculateServicePriority(SERVICELIST & candidateServices);
		void  SortServiceAccordingtoPriority(SERVICELIST & candidateServices);
		SERVICELIST  SelectUser4FDScheduling(void);
	    void  GetCadidateServiceList();
		void  SelectReRranUsers();
		void  SelectVoIPUsers();
		void clear();
	protected:
	    int  m_iMaxSelectedUENum;  //时域调度挑选最大用户数限制--不再使用
		int	m_iRBNum;
		int m_iRBNumPerUE;
		int m_iRBNumPerVoIPUE;
		int m_iVoIPUENum;
		struct ServicePriority
		{
			shared_ptr<Service> pService;
			double priority;
			bool operator<(const struct ServicePriority  &c) const
			{ return (priority<=c.priority) ? false :true; };

		};
		vector<ServicePriority> m_servicePriorityList; 
		int m_ReTranUENum;
		SERVICELIST m_SelectedService;
	};
}