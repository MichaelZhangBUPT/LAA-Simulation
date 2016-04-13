#include "TDDownlinkScheduler.h"
#include "UE.h"
#include "Configuration.h"
#include "System.h"
#include "RunParameters.h"
#include "ClockManager.h"
#include "Clock.h"
#include "SimLog.h"

namespace LTESim
{
	TDDownlinkScheduler::TDDownlinkScheduler(shared_ptr<NodeB>& pNodeB )
		: DownlinkScheduler(pNodeB),m_ReTranUENum(0)
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_iRBNum = pSystem->GetConfig().GetInt("RB_Number");
		m_iRBNumPerUE = pSystem->GetConfig().GetInt("RBNumPerUser");
		m_iRBNumPerVoIPUE = pSystem->GetConfig().GetInt("RBNum_Per_VoIPUser");
		//m_iMaxSelectedUENum = m_iRBNum / pSystem->GetConfig().GetInt("RBNumPerUser");
	}

	TDDownlinkScheduler::~TDDownlinkScheduler(void)
	{

	}

	DownlinkScheduler::SERVICELIST TDDownlinkScheduler::Process(void)
	{
		clear();//清除上次标识
		//获得所有待调度的业务
		GetCadidateServiceList();
		SimLog::Instance()->LogLTECandidateServices(ClockManager::Instance().GetLTESubFrameClock().GetInnerClock(), m_pNodeB.lock()->GetID(), m_CandidateServices.size());

		//如果没有业务要调度直接返回
		if (m_CandidateServices.empty())
		{  
			return m_CandidateServices;
		}
		m_pNodeB.lock()->SetIsUpdateCSI(true);	//初始值是false，一旦有业务之后，都为true
		SelectVoIPUsers();
		SelectReRranUsers();
		return SelectUser4FDScheduling();
	}

	void  TDDownlinkScheduler::GetCadidateServiceList()
	{
		shared_ptr<NodeB> pNodeB = m_pNodeB.lock();
		const NodeB::UELIST& CandidateUEList = pNodeB->GetUEList();
		NodeB::UELIST::const_iterator pServicePtr = CandidateUEList.begin();

		//加入每个有业务的UE到业务队列
		for ( ; pServicePtr != CandidateUEList.end(); ++pServicePtr)
		{
			shared_ptr<ILTEUE> pUE(pServicePtr->second);
			if ( pUE )
			{
				shared_ptr<Service> pService = pUE->GetService( SYSTEM_LTE );
				if ( pService )
				{
					pService -> UpdateService();
					if( pService->GetIsEnd() == false )
						m_CandidateServices.push_back( pService );	
				}
				else
					throw std::runtime_error( "Get LTE Service Error" );
			}
		}
	}
   
	void TDDownlinkScheduler::SelectVoIPUsers()
	{
		m_iVoIPUENum = 0;

		for (TDDownlinkScheduler::SERVICELIST::iterator iter = m_CandidateServices.begin();
			iter != m_CandidateServices.end(); )  
		{
			ServiceType servicetype = (*iter)->GetUE()->GetService(SYSTEM_LTE)->GetServiceType();
			if ( servicetype == VoIP )
			{
				m_iVoIPUENum += 1;
				m_SelectedService.push_back(*iter);				//保存到TD选择用户列表
				iter = m_CandidateServices.erase(iter);			//去掉VoIP业务
			}
			else
			{
				++iter;
			}
		}
	}

	void TDDownlinkScheduler::SelectReRranUsers()
	{
		m_ReTranUENum = 0;
		
		for (TDDownlinkScheduler::SERVICELIST::iterator iter = m_CandidateServices.begin();
			iter != m_CandidateServices.end(); )  
		{
			shared_ptr<HARQ> pHARQ=(*iter)->GetUE()->GetNextDLHARQ( SYSTEM_LTE );
			if (!pHARQ->GetNewPacketIndicator())
			{
				m_ReTranUENum += 1;
				m_SelectedService.push_back(*iter);				//保存到TD选择用户列表
				iter = m_CandidateServices.erase(iter);			//去掉重传业务
			}
			else
			{
				++iter;
			}
		}
	}

	void TDDownlinkScheduler::CalculateServicePriority(SERVICELIST & CandidateServices)
	{
        throw UnFinishedFunc();
	}

	void TDDownlinkScheduler::SortServiceAccordingtoPriority(SERVICELIST & CandidateServices)
	{
		m_servicePriorityList.clear();
		ServicePriority servicePriority;
		for (SERVICELIST::iterator pServicePtr = CandidateServices.begin();
			pServicePtr != CandidateServices.end();
			++pServicePtr)
		{	
			servicePriority.pService = (*pServicePtr);
			servicePriority.priority = (*pServicePtr)->GetQoS();
			m_servicePriorityList.push_back(servicePriority);
		}
		std::sort(m_servicePriorityList.begin(),m_servicePriorityList.end());
	}

	DownlinkScheduler::SERVICELIST TDDownlinkScheduler::SelectUser4FDScheduling(void)
	{
		int availableRBnum = m_iRBNum - m_iVoIPUENum*m_iRBNumPerVoIPUE - m_ReTranUENum*m_iRBNumPerUE;
		while ( availableRBnum < 0 )
		{	//VoIP业务和重传业务所需资源超过总RB数目
			m_SelectedService.pop_back();	//删除最后一个业务
			if ( m_ReTranUENum > 0 )
			{
				availableRBnum += m_iRBNumPerUE;
				m_ReTranUENum--;
			} 
			else
			{
				availableRBnum += m_iRBNumPerVoIPUE;
				m_iVoIPUENum--;
			}
		}

		if ( availableRBnum >= m_iRBNumPerUE )
		{	
			CalculateServicePriority(m_CandidateServices);
			SortServiceAccordingtoPriority(m_CandidateServices);
			if( (int)(m_servicePriorityList.size()) > (availableRBnum/m_iRBNumPerUE))
			{
				m_servicePriorityList.resize( /*ceil*/( /*double*/(availableRBnum)/m_iRBNumPerUE ) );//把剩余的RB分给下一个UE，则向上取整
				//if ( availableRBnum%m_iRBNumPerUE )	
				//{	//如果有剩余的RB，则记下要分配给的UE和剩余的RB数
				//	int UEID = m_servicePriorityList.back().pService -> GetUE() -> GetID();
				//	m_pNodeB.lock()->SetLastCandidateService( UEID, availableRBnum%m_iRBNumPerUE );
				//}
			}
			//把时域调度选好的用户储存起来，传给频域调度
			std::vector<ServicePriority>::iterator iter = m_servicePriorityList.begin();
			for( ; iter != m_servicePriorityList.end(); ++iter)
			{
				m_SelectedService.push_back((*iter).pService);
				(*iter).pService -> SetThisTransTime( ClockManager::Instance().GetCurrentTime() );
				(*iter).pService -> UpdateTransInterval();
			}
		} 
		return m_SelectedService;
	}

	void TDDownlinkScheduler::clear()
	{
		m_CandidateServices.clear();
	    m_SelectedService.clear();
	}
}