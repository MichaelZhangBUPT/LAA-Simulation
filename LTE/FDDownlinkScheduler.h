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


		//ִ��һ�ε��ȹ���
		void Process(SERVICELIST service);

		void InitChannels();
		void GetCadidateServiceList();
		////��õ����ϵ�ҵ���б�
		//virtual void GetScheduledService();
	protected:
		virtual void CalculateServicePriority( shared_ptr<Service> pService, int RBindex );
		void SortServiceAccordingtoPriority();	//��������Ȩֵ����
		//void ClearScheduleIndicator(SERVICELIST& CandidateServices);
		void ReserveRBForReTranService(SERVICELIST& CandidateServices);
		void AssignRBIndicator();
		size_t  SelectMCS(shared_ptr<Service> pService);
		/*virtual void PreAllocateResources( SERVICELIST& CandidateServices );*/
		virtual void AddScheduledChannelsToNodeB();
		/*virtual shared_ptr<Channel> GetSheduledChannel( shared_ptr<Service> pService );*/
		//�õ�Ԥ�Ʒ������ݵ�ʱ�̣����ص��Ǿ൱ǰʱ�̵�slot��
		virtual int GetDataChannelTiming( shared_ptr<Channel> pDataChannel ) const;

		virtual void Clear();

		virtual void UpdateScheduledUEForRB();
	protected:
		//sort������Ҫ�Ľṹ��
		struct ServicePriority
		{
			shared_ptr<Service> pService;
			double priority;
			int RBindex; //ռ��RB��
			bool operator<(const struct ServicePriority  &c) const
			{ return (priority<=c.priority) ? false :true; };

		};

		std::vector<shared_ptr<Channel> > m_ScheduledChannels;

		//���ݷ���ʱ����������ڵ�SLOTʱ����������ǰʱ϶��
		std::vector<int> m_DataTiming;

		std::map<shared_ptr<Service>, std::list<int> > m_serviceRBList;//ÿ��ҵ���Ӧ��ռ��RB�б�
		//vector<ServicePriority> m_serviceList;  //��ÿ��RB����ѡ������ҵ��ṹ����һ����
		//std::map<shared_ptr<Service>, size_t> m_ReTranServiceRBSize;//�洢Ҫ�ش���ҵ�����Ҫ��RB��Ŀ


		std::list<int> ReserveRBList;        //��¼��û�б�ѡ��RB�±�
		vector<ServicePriority> m_servicePriorityList; 
		int m_iRBNumPerVoIPUE;
		int m_iRBNumPerUE;
		int m_iRBNum;
	};

}
