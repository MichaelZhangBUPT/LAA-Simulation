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
		//在每次调度之前都进行清除操作
		Clear();

		//获得所有业务的列表，是为了平均吞吐量计算
		GetCadidateServiceList();

		//如果没有业务要调度直接返回
		if (CandidateServices.empty())
			return;

		//优先服务重传的业务
		//ReserveRBForReTranService(CandidateServices);

		//对每个RB选择优先级最高的业务
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

		//根据优先级分配RB
		//AssignRBIndicator();

		//对调度上的业务初始化信道
		InitChannels();

		//传递给NodeB发送
		AddScheduledChannelsToNodeB();

		//将每个用户调度的RBindex更新，用于智能天线干扰计算
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

		//加入每个有业务的UE到业务队列
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

	//////////////////////////////////////////////////////////////////////////在表中查找符合条件的分配
	void FDDownlinkScheduler::SortServiceAccordingtoPriority()
	{
		vector<int> RBtemp;	//存放分配的RBindex
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
			if ( rb < (size_t)(RBNumthisUE) )   //分配的RB数目还不够
			{
				vector<int>::iterator RBiter = RBtemp.begin();
				for ( ; RBiter != RBtemp.end() ; ++RBiter)
				{	//如果此RB已被分配，则跳出
					if ((*temp).RBindex == *RBiter)
					break;
				}
				if ( RBiter == RBtemp.end())
				{	//此RB空闲，则分配
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
				//要发送的数据信道
				shared_ptr<Channel> pDataChannel = *iter;
				shared_ptr<Service> pService = pDataChannel->GetActiveService();

				//要求NodeB分配功率
				pNodeB->AllocateResource(pDataChannel->GetPhyResourceList());

				pNodeB->AllocatePower(pDataChannel->GetPhyResourceList());

				//数据信道的定时
				int DataChanellTiming = GetDataChannelTiming(pDataChannel);
				//要求NodeB定时发送，占两个时隙
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
			//数据信道的定时
			int DataChanellTiming = 0;
			std::vector<shared_ptr<Channel> >::iterator iter = m_ScheduledChannels.begin();
			for (;iter != m_ScheduledChannels.end(); ++iter)
			{
				//要发送的数据信道
				shared_ptr<Channel> pDataChannel = *iter;
				shared_ptr<IUE> pIUE = pDataChannel->GetActiveService()->GetUE();
				shared_ptr<ILTEUE> pue = dynamic_pointer_cast<ILTEUE>( pIUE );
				if ( pue == 0 )
					throw std::runtime_error( "不匹配的业务类型" );

				const Channel::PhyResourceList &phylist = pDataChannel->GetPhyResourceList();

				Channel::PhyResourceList::const_iterator phyiter = phylist.begin();
				for ( ; phyiter != phylist.end(); ++phyiter)
				{
					int index = (*phyiter)->GetRB()*txantennaportnum + (*phyiter)->GetAntennaPort();
					ueatrblist[index] = pue;
				}
				DataChanellTiming = GetDataChannelTiming(pDataChannel);
			};
			//生成command命令，更新的是LTE系统中的RB分配信息，以子帧为单位进行反馈，所以参考时钟为LTE SubFrame Clock
			Clock & clock = ClockManager::Instance().GetLTESubFrameClock();
			shared_ptr<Command> ueatrbcommand(new UpdateUEForRBCommand(pNodeB, ueatrblist));
			shared_ptr<CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(pNodeB);
			shared_ptr<TimedCommand> pTimecommand(new TimedCommand(clock,pCmdEngine,ueatrbcommand) );
			pTimecommand->SetTimer(DataChanellTiming);
			pCmdEngine->AddCommand(pTimecommand);
		}
	}

	//得到预计发送数据的时刻，返回的是距当前时刻的slot数
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
					throw std::runtime_error( "不匹配的UE类型" );
				MCS mcs= pILTEUE->GetReporter()->GetCQI((*iterin));
				vmcs.push_back(mcs);
			}
			mcsindex = vmcs[0].ToIndex();  //选择最小的MCS等级
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
		//注：在此加入了PF算法中平均吞吐量的更新，能否加在别处？
		const double Tc=1.7;
		const double CRCSIZE = 24.0;
		ClockManager& clkman = ClockManager::Instance();
		FDDownlinkScheduler::SERVICELIST::iterator pServicePtr=m_CandidateServices.begin();
		for ( ; pServicePtr != m_CandidateServices.end(); ++pServicePtr)
		{
			shared_ptr<ILTEUE> pUE = dynamic_pointer_cast<ILTEUE>((*pServicePtr)->GetUE());
			if ( pUE == 0 )
				throw std::runtime_error( "不匹配的用户类型" );
			if (m_serviceRBList[*pServicePtr].empty())
			{
				(*pServicePtr)->SetIsScheduled(false);
				pUE->UpdateAverageThroughput( shared_ptr<Channel>() );
				continue;
			}
			(*pServicePtr)->SetIsScheduled(true);
			size_t mcsindex = SelectMCS(*pServicePtr);
			MCS mcsSelected( mcsindex, SYSTEM_LTE );
			(*pServicePtr)->GetUE()->UpdateDLHARQProcess( SYSTEM_LTE );												//更新进程，只对调度上用户做更新
			shared_ptr<HARQ> pHARQ = (*pServicePtr)->GetUE()->GetCurrentDLHARQ( SYSTEM_LTE );
			pHARQ->SetService(*pServicePtr);
			pHARQ->UpdateTransStatus(false);
			pHARQ->SetUsedRBNum(m_serviceRBList[(*pServicePtr)].size());
			ChannelMaker maker;

			shared_ptr<NodeB>	pNodeB = m_pNodeB.lock();
			int AntennaNumber = pNodeB->GetTxAntenna()->GetAntennaPortNum();

			//生成的是LTE系统中的Channel，所以使用的是LTE Frame Clock和LTE SubFrame Clock的时间
			shared_ptr<Channel> pChannel = maker.CreateDLSCH(	pUE, mcsSelected, pNodeB->GetMaxTxPower() / pNodeB->GetRBPerSlot() / AntennaNumber, 
																									clkman.GetFrameClock().GetClock(),clkman.GetLTESubFrameClock().GetClock(),m_serviceRBList[(*pServicePtr)]
																								);
			assert(pHARQ->GetMCS().ToIndex() == pChannel->GetMCS().ToIndex());

			pUE->UpdateAverageThroughput( pChannel );
			//定时在当前时隙发送
			m_DataTiming.push_back(0);
			m_ScheduledChannels.push_back(pChannel);
		}
	}

	//shared_ptr<Channel> DownLinkScheduler::GetSheduledChannel( shared_ptr<Service> pService )	//获得调度的信道
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
	//       m_servicePriorityList.resize(RBNUMPerUSER);//只要前面最大的几个RB
	//	std::vector<ServicePriority>::iterator temp = m_servicePriorityList.begin();
	//	for ( ; temp!= m_servicePriorityList.end();++temp)
	//	{
	//		m_serviceRBList[(*temp).pService].push_back((*temp).RBindex);
	//		ReserveRBList.erase(find(ReserveRBList.begin(),ReserveRBList.end(),(*temp).RBindex));
	//	}
	//}
}