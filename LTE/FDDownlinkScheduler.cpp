#include "commondef.h"
#include <algorithm>
#include <boost/bind.hpp>
#include "FDDownlinkScheduler.h"
#include "System.h"
#include "NodeB.h"
#include "UE.h"
#include "Service.h"
#include "ChannelRxCommand.h"
#include "TimedCommand.h"
#include "ClockManager.h"
#include "Channel.h"
#include "ChannelMaker.h"
#include "UpdateUEForRBCommand.h"
#include "TimedCommand.h"
#include "DownlinkScheduler.h"
#include "RunParameters.h"
#include <map>
namespace LTESim
{
	FDDownlinkScheduler::FDDownlinkScheduler(shared_ptr<NodeB>& pNodeB)
		:DownlinkScheduler(pNodeB)
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_iRBNum = pSystem->GetConfig().GetInt("RB_Number");
		m_iRBNumPerUE = pSystem->GetConfig().GetInt("RBNumPerUser");
		m_iRBNumPerVoIPUE = pSystem->GetConfig().GetInt("RBNum_Per_VoIPUser");
	}

	FDDownlinkScheduler::~FDDownlinkScheduler(void)
	{

	}

	void FDDownlinkScheduler::Process(SERVICELIST CandidateServices)
	{
		//��ÿ�ε���֮ǰ�������������
		Clear();

		//�������ҵ����б���Ϊ��ƽ������������
		GetCadidateServiceList();

		//���û��ҵ��Ҫ����ֱ�ӷ���
		if (CandidateServices.empty())
			return;

		//���ȷ����ش���ҵ��
		//ReserveRBForReTranService(CandidateServices);

		//��ÿ��RBѡ�����ȼ���ߵ�ҵ��
		/*for (int RBIndex = 0; RBIndex < RBNUM && !CandidateServices.empty(); RBIndex++)
		{
			CalculateServicePriority(CandidateServices, RBIndex);
			SortServiceAccordingtoPriority(CandidateServices, RBIndex);
		}*/

		ReserveRBList.clear();
		for (int i=0; i < m_iRBNum; i++)
		  ReserveRBList.push_back(i);
		
		SERVICELIST::iterator iter = CandidateServices.begin();
		m_servicePriorityList.clear();
		for ( ; iter!=CandidateServices.end(); ++iter)
		{  
			std::list<int>::iterator it = ReserveRBList.begin();
			for ( ; it!=ReserveRBList.end(); ++it)
				CalculateServicePriority(*iter,*it);
		}
			
		SortServiceAccordingtoPriority();

		//�������ȼ�����RB
		//AssignRBIndicator();

		//�Ե����ϵ�ҵ���ʼ���ŵ�
		InitChannels();

		//���ݸ�NodeB����
		AddScheduledChannelsToNodeB();

		//��ÿ���û����ȵ�RBindex���£������������߸��ż���
		UpdateScheduledUEForRB();

		Clear();
	}

	void FDDownlinkScheduler::GetCadidateServiceList()
	{
		shared_ptr<NodeB> pNodeB = m_pNodeB.lock();
		const NodeB::UELIST& CandidateUEList = pNodeB->GetUEList();
		NodeB::UELIST::const_iterator pServicePtr = CandidateUEList.begin();

		if ( pNodeB->GetID() == 0 && CandidateUEList.empty() )
		{
			USER_MESSAGE("Find Case")
		}

		//����ÿ����ҵ���UE��ҵ�����
		for ( ; pServicePtr != CandidateUEList.end(); ++pServicePtr)
		{
			shared_ptr<ILTEUE> pUE(pServicePtr->second);
			if ( pUE )
			{
				shared_ptr<Service> pService = pUE->GetService( SYSTEM_LTE );
				m_CandidateServices.push_back(pService);
			}
		}
	}

	void FDDownlinkScheduler::CalculateServicePriority( shared_ptr<Service> pService, int RBindex )
	{
		throw UnFinishedFunc();
	}

	//////////////////////////////////////////////////////////////////////////�ڱ��в��ҷ��������ķ���
	void FDDownlinkScheduler::SortServiceAccordingtoPriority()
	{
		vector<int> RBtemp;	//��ŷ����RBindex
		std::sort(m_servicePriorityList.begin(),m_servicePriorityList.end());
		//map<int,int> ParticularService = m_pNodeB.lock()->GetLastCandidateService();
		//int ParticularUEID = -1;
		//int ParticularRBNum = 0;
		//if ( !ParticularService.empty() )
		//{
		//	ParticularUEID = ParticularService.begin() -> first;
		//	ParticularRBNum = ParticularService.begin() -> second;
		//}
		std::vector<ServicePriority>::iterator temp = m_servicePriorityList.begin();
		for ( ; temp!= m_servicePriorityList.end(); ++temp)
		{ 
			int RBNumthisUE;
			ServiceType servicetype = (*temp).pService->GetServiceType();
			if ( servicetype == VoIP )	
				RBNumthisUE = m_iRBNumPerVoIPUE;
			else	
			{
				RBNumthisUE = m_iRBNumPerUE;
				//int thisUEID = (*temp).pService -> GetUE() ->GetID();
				//if( thisUEID == ParticularUEID )
				//	RBNumthisUE = ParticularRBNum;
			}

			size_t rb = m_serviceRBList[(*temp).pService].size();
			if ( rb < (size_t)(RBNumthisUE) )   //�����RB��Ŀ������
			{
				vector<int>::iterator RBiter = RBtemp.begin();
				for ( ; RBiter != RBtemp.end() ; ++RBiter)
				{	//�����RB�ѱ����䣬������
					if ((*temp).RBindex == *RBiter)
					break;
				}
				if ( RBiter == RBtemp.end())
				{	//��RB���У������
					m_serviceRBList[(*temp).pService].push_back((*temp).RBindex);
					RBtemp.push_back((*temp).RBindex);
				}
			}
			//m_serviceRBList[(*temp).pService].push_back((*temp).RBindex);
			//ReserveRBList.erase(find(ReserveRBList.begin(),ReserveRBList.end(),(*temp).RBindex));
		}

		int UsedRBNum = RBtemp.size();
		m_pNodeB.lock()->UpdateUsedRBNum(UsedRBNum);
	}

	void FDDownlinkScheduler::AddScheduledChannelsToNodeB()
	{
		if (shared_ptr<NodeB> pNodeB = m_pNodeB.lock())
		{
			std::vector<shared_ptr<Channel> >::iterator iter = m_ScheduledChannels.begin();
			for (;iter != m_ScheduledChannels.end(); ++iter)
			{
				//Ҫ���͵������ŵ�
				shared_ptr<Channel> pDataChannel = *iter;
				shared_ptr<Service> pService = pDataChannel->GetActiveService();

				//Ҫ��NodeB���书��
				pNodeB->AllocateResource(pDataChannel->GetPhyResourceList());

				pNodeB->AllocatePower(pDataChannel->GetPhyResourceList());

				//�����ŵ��Ķ�ʱ
				int DataChanellTiming = GetDataChannelTiming(pDataChannel);
				//Ҫ��NodeB��ʱ���ͣ�ռ����ʱ϶
				pNodeB->TransmittChannel( pDataChannel, DataChanellTiming);
				pNodeB->TransmittChannel( pDataChannel, DataChanellTiming + 1);
			};
		}
	}

	void FDDownlinkScheduler::UpdateScheduledUEForRB()
	{
		shared_ptr<NodeB> pNodeB = m_pNodeB.lock();
		int txantennaportnum = pNodeB->GetTxAntenna()->GetAntennaPortNum();
		vector<shared_ptr<ILTEUE> > ueatrblist(m_iRBNum*txantennaportnum);
		if (pNodeB)
		{
			//�����ŵ��Ķ�ʱ
			int DataChanellTiming = 0;
			std::vector<shared_ptr<Channel> >::iterator iter = m_ScheduledChannels.begin();
			for (;iter != m_ScheduledChannels.end(); ++iter)
			{
				//Ҫ���͵������ŵ�
				shared_ptr<Channel> pDataChannel = *iter;
				shared_ptr<IUE> pIUE = pDataChannel->GetActiveService()->GetUE();
				shared_ptr<ILTEUE> pue = dynamic_pointer_cast<ILTEUE>( pIUE );
				if ( pue == 0 )
					throw std::runtime_error( "��ƥ���ҵ������" );

				const Channel::PhyResourceList &phylist = pDataChannel->GetPhyResourceList();

				Channel::PhyResourceList::const_iterator phyiter = phylist.begin();
				for ( ; phyiter != phylist.end(); ++phyiter)
				{
					int index = (*phyiter)->GetRB()*txantennaportnum + (*phyiter)->GetAntennaPort();
					ueatrblist[index] = pue;
				}
				DataChanellTiming = GetDataChannelTiming(pDataChannel);
			};
			//����command������µ���LTEϵͳ�е�RB������Ϣ������֡Ϊ��λ���з��������Բο�ʱ��ΪLTE SubFrame Clock
			Clock & clock = ClockManager::Instance().GetLTESubFrameClock();
			shared_ptr<Command> ueatrbcommand(new UpdateUEForRBCommand(pNodeB, ueatrblist));
			shared_ptr<CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(pNodeB);
			shared_ptr<TimedCommand> pTimecommand(new TimedCommand(clock,pCmdEngine,ueatrbcommand) );
			pTimecommand->SetTimer(DataChanellTiming);
			pCmdEngine->AddCommand(pTimecommand);
		}
	}

	//�õ�Ԥ�Ʒ������ݵ�ʱ�̣����ص��Ǿ൱ǰʱ�̵�slot��
	int FDDownlinkScheduler::GetDataChannelTiming( shared_ptr<Channel> pDataChannel ) const
	{
		size_t pos = std::distance(m_ScheduledChannels.begin(),
			std::find(m_ScheduledChannels.begin(), m_ScheduledChannels.end(), pDataChannel)
			);
		return m_DataTiming[pos];
	}

	void FDDownlinkScheduler::Clear()
	{
		m_ScheduledChannels.clear();
		m_DataTiming.clear();
		m_CandidateServices.clear();
		m_servicePriorityList.clear();
		m_serviceRBList.clear();
	}

	size_t FDDownlinkScheduler::SelectMCS(shared_ptr<Service> pService)
	{
		vector<MCS> vmcs;
		std::list<int>& indicator = m_serviceRBList[pService];
		shared_ptr<HARQ> pHARQ=pService->GetUE()->GetNextDLHARQ( SYSTEM_LTE );
		int mcsindex;
		if (pHARQ->GetNewPacketIndicator())
		{
			std::list<int>::iterator iterin=indicator.begin();
			for ( ; iterin != indicator.end(); ++iterin)
			{
				shared_ptr<IUE> pIUE = pService->GetUE();
				shared_ptr<ILTEUE> pILTEUE = dynamic_pointer_cast<ILTEUE>( pIUE );
				if ( pILTEUE == 0 )
					throw std::runtime_error( "��ƥ���UE����" );
				MCS mcs= pILTEUE->GetReporter()->GetCQI((*iterin));
				vmcs.push_back(mcs);
			}
			mcsindex = vmcs[0].ToIndex();  //ѡ����С��MCS�ȼ�
			for (size_t i = 1; i < vmcs.size(); ++i)
			{
				if (vmcs[i].ToIndex() < mcsindex)
				{
					mcsindex = vmcs[i].ToIndex();
				}
			}
			pHARQ->InitTrans();
			pHARQ->SetMCS(MCS(mcsindex, SYSTEM_LTE));
			return  mcsindex;
		}
		else
		{
			mcsindex = pHARQ->GetMCS().ToIndex();
			return mcsindex;
		}
	}

	void FDDownlinkScheduler::InitChannels()
	{   
		//ע���ڴ˼�����PF�㷨��ƽ���������ĸ��£��ܷ���ڱ𴦣�
		const double Tc=1.7;
		const double CRCSIZE = 24.0;
		ClockManager& clkman = ClockManager::Instance();
		FDDownlinkScheduler::SERVICELIST::iterator pServicePtr=m_CandidateServices.begin();
		for ( ; pServicePtr != m_CandidateServices.end(); ++pServicePtr)
		{
			shared_ptr<ILTEUE> pUE = dynamic_pointer_cast<ILTEUE>((*pServicePtr)->GetUE());
			if ( pUE == 0 )
				throw std::runtime_error( "��ƥ����û�����" );
			if (m_serviceRBList[*pServicePtr].empty())
			{
				(*pServicePtr)->SetIsScheduled(false);
				pUE->UpdateAverageThroughput( shared_ptr<Channel>() );
				continue;
			}
			(*pServicePtr)->SetIsScheduled(true);
			size_t mcsindex = SelectMCS(*pServicePtr);
			MCS mcsSelected( mcsindex, SYSTEM_LTE );
			(*pServicePtr)->GetUE()->UpdateDLHARQProcess( SYSTEM_LTE );												//���½��̣�ֻ�Ե������û�������
			shared_ptr<HARQ> pHARQ = (*pServicePtr)->GetUE()->GetCurrentDLHARQ( SYSTEM_LTE );
			pHARQ->SetService(*pServicePtr);
			pHARQ->UpdateTransStatus(false);
			pHARQ->SetUsedRBNum(m_serviceRBList[(*pServicePtr)].size());
			ChannelMaker maker;

			shared_ptr<NodeB>	pNodeB = m_pNodeB.lock();
			int AntennaNumber = pNodeB->GetTxAntenna()->GetAntennaPortNum();

			//���ɵ���LTEϵͳ�е�Channel������ʹ�õ���LTE Frame Clock��LTE SubFrame Clock��ʱ��
			shared_ptr<Channel> pChannel = maker.CreateDLSCH(	pUE, mcsSelected, pNodeB->GetMaxTxPower() / pNodeB->GetRBPerSlot() / AntennaNumber, 
																									clkman.GetFrameClock().GetClock(),clkman.GetLTESubFrameClock().GetClock(),m_serviceRBList[(*pServicePtr)]
																								);
			assert(pHARQ->GetMCS().ToIndex() == pChannel->GetMCS().ToIndex());

			pUE->UpdateAverageThroughput( pChannel );
			//��ʱ�ڵ�ǰʱ϶����
			m_DataTiming.push_back(0);
			m_ScheduledChannels.push_back(pChannel);
		}
	}

	//shared_ptr<Channel> DownLinkScheduler::GetSheduledChannel( shared_ptr<Service> pService )	//��õ��ȵ��ŵ�
	//{
	//	std::vector<shared_ptr<Channel> >::iterator iter =
	//		std::find_if(m_ScheduledChannels.begin(), m_ScheduledChannels.end(),
	//		boost::bind(std::equal_to<shared_ptr<Service> >(),
	//		boost::bind(&Channel::GetActiveService, _1),
	//						pService
	//				)
	//		);
	//	if (iter != m_ScheduledChannels.end())
	//	{
	//		return *iter;
	//	}
	//	return shared_ptr<Channel>();
	//}

	//void FDDownLinkScheduler::SortServiceAccordingtoPriority()
	//{
	//	std::sort(m_servicePriorityList.begin(),m_servicePriorityList.end());
	//       m_servicePriorityList.resize(RBNUMPerUSER);//ֻҪǰ�����ļ���RB
	//	std::vector<ServicePriority>::iterator temp = m_servicePriorityList.begin();
	//	for ( ; temp!= m_servicePriorityList.end();++temp)
	//	{
	//		m_serviceRBList[(*temp).pService].push_back((*temp).RBindex);
	//		ReserveRBList.erase(find(ReserveRBList.begin(),ReserveRBList.end(),(*temp).RBindex));
	//	}
	//}
}