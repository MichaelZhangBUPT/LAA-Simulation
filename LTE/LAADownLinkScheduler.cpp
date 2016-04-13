/*用于LAA的调度，每TTI全频带调度一个UE，所以只在时间域上采用RR或者PF调度算法，频域方面不做分配
返回优先级最高的UE，到LAA的GetCandidateUEList()；2016.3.17HFR*/
#include "LAADownLinkScheduler.h"
#include "UE.h"
#include "Service.h"
#include "Configuration.h"
#include "System.h"
#include "RunParameters.h"
#include "ClockManager.h"
#include "Clock.h"
#include "SimLog.h"
#include "RandomManager.h"

namespace LTESim
{
	LAADownLinkScheduler::LAADownLinkScheduler( void )
	{
		
	}

	LAADownLinkScheduler::~LAADownLinkScheduler( void )
	{

	}

	shared_ptr<ILAAUE> LAADownLinkScheduler::ScheduleProcess(shared_ptr <LAANodeB> pLAANodeB)
	{
		clear();//清除上次标识
		GetCadidateServiceList(pLAANodeB);//获得所有待调度的业务

		if (m_CandidateServices.empty())
		{  
			//return m_CandidateServices;
			shared_ptr <ILAAUE> pUE;
			return pUE;
		}

		return GetScheduledLAAUE();
	}

	void LAADownLinkScheduler::GetCadidateServiceList(shared_ptr <LAANodeB> pLAANodeB)
	{
		const LAANodeB::LAAUELIST& CandidateLAAUEList = pLAANodeB->GetUEList();
		LAANodeB::LAAUELIST::const_iterator pServicePtr = CandidateLAAUEList.begin();

		//加入每个有业务的UE到业务队列
		for ( ; pServicePtr != CandidateLAAUEList.end(); ++pServicePtr)
		{
			shared_ptr<ILAAUE> pUE = pServicePtr->second.lock();
			if ( pUE )
			{
				shared_ptr<Service> pService = pUE->GetService( SYSTEM_LAA );
				if ( pService )
				{
					pService -> UpdateService();
					if( pService->GetIsEnd() == false )
						m_CandidateServices.push_back( pService );	
				}
				else
					throw std::runtime_error( "Get LAA Service Error" );
			}
		}
	}



	void LAADownLinkScheduler::CalculateServicePriorityForRR( SERVICELIST & CandidateServices )
	{
		SERVICELIST::iterator iter=CandidateServices.begin();
		UNIFORM_REAL_GEN UEPriorityGen = RandomManager::GetRandomManager().GetUniformRealGen(0.0, 1.0);	
		for ( ; iter != CandidateServices.end(); ++iter)
		{
			double priority=UEPriorityGen();
			(*iter)->SetQoS(priority);
		}
	}

	void LAADownLinkScheduler::CalculateServicePriorityForPF( SERVICELIST & CandidateServices )
	{

	}

	void LAADownLinkScheduler::SortServiceAccordingtoPriority( SERVICELIST & CandidateServices )
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

	shared_ptr<ILAAUE> LAADownLinkScheduler::GetScheduledLAAUE()
	{
		shared_ptr <System> pSystem = System::GetCurrentSystem();
		const std::string & ScheduleAlgorithm = pSystem->GetConfig().GetString("LAA_DL_TDSchedule_Algorithm");
		if (ScheduleAlgorithm == "RR")
		{
			CalculateServicePriorityForRR(m_CandidateServices);
		} 
		if (ScheduleAlgorithm == "PF")
		{
			CalculateServicePriorityForPF(m_CandidateServices);
		}
		
		SortServiceAccordingtoPriority(m_CandidateServices);
		std::vector<ServicePriority>::iterator iter = m_servicePriorityList.begin();
		//m_SelectedService.push_back((*iter).pService);
		shared_ptr<ILAAUE> plaaue = dynamic_pointer_cast<LAAUE>((*iter).pService->GetUE());
		return plaaue;
		
	}


	void LAADownLinkScheduler::clear()
	{
		m_CandidateServices.clear();
	}	
}



