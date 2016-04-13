#pragma once
#include <map>
#include "FDDownlinkScheduler.h"

namespace LTESim
{

	class FDPFDownlinkScheduler :
		public FDDownlinkScheduler
	{
	public:
		FDPFDownlinkScheduler(shared_ptr<NodeB>& pNodeB);
		//sort函数需要的 
		//struct ServicePriority
		//{
		//	shared_ptr<Service> servicePoint;
		//	double priority;
  //          int RBindex; //占的RB号
		//	bool operator<(const struct ServicePriority  &c)
		//	{ return (priority<=c.priority) ? false :true; };

		//}m_servicePriority;
		//vector<ServicePriority> m_vservicePriority; 
	public: 
		~FDPFDownlinkScheduler(void);
		void CalculateServicePriority(shared_ptr<Service>pService ,int RBindex);	//计算权值
		//void SortServiceAccordingtoPriority(SERVICELIST& CandidateServices,int rb);	//将服务按照权值排序
		//void Process();
		//void ClearScheduleIndicator(SERVICELIST& CandidateServices);
		//void SelectReTranService(SERVICELIST& CandidateServices);
		//void AssignRBIndicator();
		//size_t  SelectMCS(shared_ptr<Service> pService);
		//
	protected:
		std::map<shared_ptr<Service>,double> AverageThr;
	};

}

