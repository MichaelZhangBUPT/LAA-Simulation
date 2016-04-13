#include "HighLayerSchedulerScheme.h"
#include "HighLayerController.h"
#include "BufferDistributeCommand.h"
#include "CMDEngine.h"
#include "IUE.h"
#include "UE.h"
#include "HSPAUE.h"
#include "NodeB.h"
#include "HSPANodeB.h"
#include "ClockManager.h"
#include <algorithm>

#include <map>


namespace LTESim
{
	HighLayerSchedulerScheme::HighLayerSchedulerScheme(shared_ptr<HighLayerController> pController )
		:HighLayerScheduler(pController)
	{
		m_DualUEID.clear();
		m_ServiceThrVec.clear();
	}

	HighLayerSchedulerScheme::~HighLayerSchedulerScheme()
	{

	}

	void HighLayerSchedulerScheme::MakeScheme()
	{
		if ( shared_ptr<HighLayerController> pController = m_pMasterController.lock() )
		{
			m_ServiceThrVec.clear();
			for ( SERVICESET::iterator setIter = m_ServiceSet.begin(); setIter != m_ServiceSet.end(); ++setIter )
			{
				ServiceThr ServiceThrItem;
				ServiceThrItem.m_pService = *setIter;
				ServiceThrItem.m_dThr = ( (*setIter) -> GetThroughput() );
				ServiceThrItem.m_LTERatio = 0.0;
				m_ServiceThrVec.push_back(ServiceThrItem);
			}

			if(ClockManager::Instance().GetFrameClock().GetClock() == 2000)
			{
				//按吞吐量从低到高排序,选出低的用户
				std::sort(m_ServiceThrVec.begin(), m_ServiceThrVec.end());
				std::vector<ServiceThr>::iterator Iter = m_ServiceThrVec.begin();
				for( int i = 0; (Iter != m_ServiceThrVec.end()) && (i < 15); ++Iter, ++i )
				{
					//(*Iter).m_LTERatio = 2.0 / 3.0;
					int iUEID = (*Iter).m_pService->GetUE()->GetID();
					m_DualUEID.push_back(iUEID);
				}
			}
			//2000帧以后，把吞吐量低的15个UE改成双模UE，LTE业务比例为2/3
			if( ClockManager::Instance().GetFrameClock().GetClock() >= 2000 )
			{
				std::vector<ServiceThr>::iterator Iter = m_ServiceThrVec.begin();
				for( int i = 0; (Iter != m_ServiceThrVec.end()); ++Iter, ++i )
				{
					int iUEID = (*Iter).m_pService->GetUE()->GetID();
					if( find(m_DualUEID.begin(),m_DualUEID.end(),iUEID) != m_DualUEID.end() )
						(*Iter).m_LTERatio = 1.0; //2.0 / 3.0;
				}
			}

			for ( std::vector<ServiceThr>::iterator vecIter = m_ServiceThrVec.begin(); vecIter != m_ServiceThrVec.end(); ++vecIter )
			{
				shared_ptr<ServiceComposite> pCompositeService = (*vecIter).m_pService;
				if( pCompositeService->IsDataDistributed() == false )
				{
					double dLTERatio = (*vecIter).m_LTERatio;
					double dTotalDataVolume = pCompositeService->GetData();
					DataDistributeScheme dataDistScheme;
					dataDistScheme.AddSchemeNode( DataDistributeSchemeNode( SYSTEM_LTE, dTotalDataVolume * dLTERatio ) );
					dataDistScheme.AddSchemeNode( DataDistributeSchemeNode( SYSTEM_HSPA, dTotalDataVolume * (1.0 - dLTERatio ) ) );
					m_ServiceDataDistSchemeMap.insert( std::make_pair( pCompositeService, dataDistScheme ) );
				}
			}
		}
		else
		{
			throw std::runtime_error( "Get Master High Layer Controller Error" );
		}

	}
}