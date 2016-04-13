#include "HSPATDScheduler.h"
#include "commondef.h"
#include "Service.h"
#include "System.h"
#include "Configuration.h"
#include "RunParameters.h"
#include "HSPANodeB.h"
#include "HSPAUE.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <algorithm>
#include "SimLog.h"
#include "ClockManager.h"

namespace LTESim
{
	/*std::ofstream fout("../Output/C2I.txt");*/
	HSPATDDownlinkScheduler::HSPATDDownlinkScheduler( shared_ptr<HSPANodeB> pNodeB )
		: m_pMasterNodeB( pNodeB )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_iMaxSelectedUENum = pSystem->GetConfig().GetInt("HSPA_TotalCodeNum") / pSystem->GetConfig().GetInt("HSPA_CodeNumPerUE");
	}

	HSPATDDownlinkScheduler::~HSPATDDownlinkScheduler()
	{

	}

	HSPATDDownlinkScheduler::SERVICELIST HSPATDDownlinkScheduler::Process()
	{
		Clear();

		GetCandidateServiceList();

		PrepareServicePriorityList();

		SelectRetransUsers();

		SelectUsersAccordingToPriorityAfterReservation();

		//MAKE_SURE( m_SelectedServiceList.size() <= 1, "HSPATDDownlinkScheduler::Process" )


		return m_SelectedServiceList;
	}

	void HSPATDDownlinkScheduler::Clear()
	{
		m_SelectedServiceList.clear();
		m_CandServiceList.clear();
		m_ServicePriorityVec.clear();
	}

	void HSPATDDownlinkScheduler::GetCandidateServiceList()
	{
		//�õ���ѡ��Service�����У����뵽m_CandServiceVec��
		if ( shared_ptr<HSPANodeB> pNodeB = m_pMasterNodeB.lock() )
		{
			//��ѡService���ǽ��뵽��HSPANodeB������UE������HSPAUE��DualModeUE����HSPA Service
			const HSPANodeB::UELIST& refUEList = pNodeB->GetUEList();
			for ( HSPANodeB::UELIST::const_iterator mapIter = refUEList.begin(); mapIter != refUEList.end(); ++mapIter )
			{
				shared_ptr<IHSPAUE> pUE = mapIter->second.lock();
				if ( pUE )
				{
					shared_ptr<Service> pService = pUE->GetService( SYSTEM_HSPA );
					if ( pService )
					{
						if( pService->GetIsEnd() == false )
							m_CandServiceList.push_back( pService );
					}
					else
						throw std::runtime_error( "Get HSPA Service Error" );
				}
				else
				{
					throw std::runtime_error("��ȡCandidate UEʧ��");
				}
			}
		}
		else
		{
			throw std::runtime_error( "Master HSPANodeB ��ȡʧ��" );
		}
	}

	void HSPATDDownlinkScheduler::PrepareServicePriorityList()
	{
		for ( std::vector<shared_ptr<Service>>::iterator vecIter = m_CandServiceList.begin(); vecIter != m_CandServiceList.end(); ++vecIter )
		{
			ServicePriority servicePriorityItem;
			servicePriorityItem.m_pService = *vecIter;
			servicePriorityItem.m_dPriority = CalculatePriority( *vecIter );
			m_ServicePriorityVec.push_back( servicePriorityItem );
			//SimLog::Instance()->LogC2I( (*vecIter)->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), (*vecIter)->GetUE()->GetType(), servicePriorityItem.m_dPriority  );

		}
		//�������ȼ��Ӵ�С����
		std::sort( m_ServicePriorityVec.begin(), m_ServicePriorityVec.end(), std::greater<ServicePriority>() );
	}

	void HSPATDDownlinkScheduler::SelectRetransUsers()
	{
		//ѡ���ش����û�,��ѡ���ϵ��û����뵽m_SelectedServiceList��
		for( std::vector<ServicePriority>::iterator vecIter = m_ServicePriorityVec.begin(); vecIter != m_ServicePriorityVec.end(); )
		{
			shared_ptr<Service> pService = vecIter->m_pService;
			shared_ptr<IUE> pIUE = pService->GetUE();
			//�ڻ�δ����UE֮ǰ��UE��HARQ���̲�û�и��£�������������̽�õ���һ��HARQ����
			//����һ�������Ƿ�Ϊ�µ����ݣ����Ϊ�ش��Ļ���������ѡ���UE
			shared_ptr<HARQ> pHARQ = pIUE->GetNextDLHARQ( SYSTEM_HSPA );
			if ( pHARQ->GetNewPacketIndicator() == false )
			{
				m_SelectedServiceList.push_back( pService );
				vecIter = m_ServicePriorityVec.erase( vecIter );
			}
			else
			{
				vecIter++;
			}
		}
	}

	void HSPATDDownlinkScheduler::SelectUsersAccordingToPriorityAfterReservation()
	{
		//���ش��û�ѡ������ڽ��������û��а������ȼ��Ӹߵ��ͽ���ѡ��һֱ������û���Ŀ����������UE��ѡ��Ϊֹ
		for ( std::vector<ServicePriority>::iterator vecIter = m_ServicePriorityVec.begin(); vecIter != m_ServicePriorityVec.end(); ++vecIter )
		{
			if ( m_SelectedServiceList.size() >= (size_t)( m_iMaxSelectedUENum ) )
				break;
			m_SelectedServiceList.push_back( vecIter->m_pService );
			//SimLog::Instance()->LogC2I( vecIter->m_pService->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), vecIter->m_pService->GetUE()->GetType()  );

		}
	}
}