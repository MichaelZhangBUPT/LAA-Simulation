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
		//sort������Ҫ�� 
		//struct ServicePriority
		//{
		//	shared_ptr<Service> servicePoint;
		//	double priority;
  //          int RBindex; //ռ��RB��
		//	bool operator<(const struct ServicePriority  &c)
		//	{ return (priority<=c.priority) ? false :true; };

		//}m_servicePriority;
		//vector<ServicePriority> m_vservicePriority; 
	public: 
		~FDPFDownlinkScheduler(void);
		void CalculateServicePriority(shared_ptr<Service>pService ,int RBindex);	//����Ȩֵ
		//void SortServiceAccordingtoPriority(SERVICELIST& CandidateServices,int rb);	//��������Ȩֵ����
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

