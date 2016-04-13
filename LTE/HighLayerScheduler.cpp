#include "HighLayerScheduler.h"
#include "HighLayerController.h"
#include "ServiceComposite.h"
#include "DualModeFTPService.h"
#include "DataDistributeScheme.h"
#include "BufferDistributeCommand.h"
#include "CMDEngine.h"
#include "IUE.h"
#include "UE.h"
#include "HSPAUE.h"
#include "WiFiUE.h"
#include "DualModeLTEHSPAUE.h"
#include "LTEWiFiUE.h"
#include "NodeB.h"
#include "HSPANodeB.h"
#include "WiFiAP.h"
#include "ClockManager.h"
#include "RunParameters.h"
#include "RandomManager.h"
#include "SimLog.h"
#include<algorithm>

#include <map>

namespace LTESim
{

	HighLayerScheduler::HighLayerScheduler( shared_ptr<HighLayerController> pController )
		: m_pMasterController( pController ),m_bSelectLTE(false)
	{
		
	}

	HighLayerScheduler::~HighLayerScheduler()
	{

	}

	void HighLayerScheduler::Process()
	{
		//��Process��ʼ�Լ�������ʱ�򶼽������
		Clear();

		double time = ClockManager::Instance().GetCurrentTime();	//ms
		int id = m_pMasterController.lock()->GetID();
		shared_ptr<NodeB> nodeb = System::GetCurrentSystem()->GetNodeBFromOffsetID(id);
		shared_ptr<WiFiAP> ap = System::GetCurrentSystem()->GetWiFiAPFromOffsetID(id);
		double LTECellThr = nodeb -> GetCellThroughput() / time;	//kBps
		double WiFiCellThr = ap -> GetCellThroughput() / time;
		double ltecu = nodeb->GetUsedRBNum() / time/50;
		double wificu = ap->GetTransmitDataTime() / ( time*1000.0 );
		SimLog::Instance()->LogThrAndCU(time,id,LTECellThr,WiFiCellThr,ltecu,wificu);

		PrepareServiceVector();
		//std::cout<<"m_ServiceSet  "<<m_ServiceSet.size()<<endl;

		//��ͬ�ĵ����㷨��Ӧ�ĵ�������ʹ�ò�ͬ�Ĳ�������m_ServiceDataDistSchemeMap
		MakeScheme();

		//�������ݷ��䷽���������ݷ����Command�����뵽UE��Command�б���
		GenerateAndAddCommand();

		Clear();
	}

	void HighLayerScheduler::PrepareServiceVector()
	{
		//�ڵõ�ServiceList��ʱ���п���DualModeUE����ͬʱ����HSPA NodeB��LTE NodeB
		//���ȵõ���Cell��Ӧ��LTENodeB��HSPANodeB��Ȼ�����н����DualMode��UE���ҳ��������¼���б���
		if(shared_ptr<HighLayerController> pController = m_pMasterController.lock() )
		{
			shared_ptr<System> pSystem = System::GetCurrentSystem();
			shared_ptr<NodeB> pLTENodeB = pSystem->GetNodeBFromOffsetID( pController->GetID() );
			const NodeB::UELIST& rLTEUEList = pLTENodeB->GetUEList();
			for ( NodeB::UELIST::const_iterator mapIter = rLTEUEList.begin(); mapIter != rLTEUEList.end(); ++mapIter )
			{
				//����˫ģ��UE�̳���ILTE Interface��������LTE NodeB��UEList�д洢��ILTE���п���ָ�����˫ģ��UE
				shared_ptr<ILTEUE> pUE = mapIter->second.lock();
				if( pUE && pUE->GetType() == SYSTEM_DUAL )
				{
					shared_ptr<ServiceComposite> pCompositeService = dynamic_pointer_cast<ServiceComposite>( pUE->GetService( SYSTEM_DUAL ) );
					m_ServiceSet.insert( pCompositeService );
				}
			}

			shared_ptr<HSPANodeB> pHSPANodeB = pSystem->GetHSPANodeBFromOffsetID( pController->GetID() );
			const HSPANodeB::UELIST& rHSPAUEList = pHSPANodeB->GetUEList();
			for ( HSPANodeB::UELIST::const_iterator mapIter = rHSPAUEList.begin(); mapIter != rHSPAUEList.end(); ++mapIter )
			{
				//����˫ģ��UE�̳���IHSPA Interface��������IHSPA NodeB��UEList�д洢��IHSPA���п���ָ�����˫ģ��UE
				shared_ptr<IHSPAUE> pUE = mapIter->second.lock();
				if( pUE && pUE->GetType() == SYSTEM_DUAL )
				{
					if(shared_ptr<ServiceComposite> pCompositeService = dynamic_pointer_cast<ServiceComposite>( pUE->GetService( SYSTEM_DUAL ) ))
						m_ServiceSet.insert( pCompositeService );
					else
						throw std::runtime_error( "Convert to Composite Service Error" );
				}
			}

			shared_ptr<WiFiAP> pWiFiAP = pSystem->GetWiFiAPFromOffsetID( pController->GetID() );
			const WiFiAP::WiFiUELIST& rWiFiUEList = pWiFiAP->GetUEList();
			for ( WiFiAP::WiFiUELIST::const_iterator mapIter = rWiFiUEList.begin(); mapIter != rWiFiUEList.end(); ++mapIter )
			{
				shared_ptr<IWiFiUE> pUE = mapIter->second.lock();
				if( pUE && pUE->GetType() == SYSTEM_DUAL )
				{
					if(shared_ptr<ServiceComposite> pCompositeService = dynamic_pointer_cast<ServiceComposite>( pUE->GetService( SYSTEM_DUAL ) ))
						m_ServiceSet.insert( pCompositeService );
					else
						throw std::runtime_error( "Convert to Composite Service Error" );
				}
			}

		}
		else
		{
			throw std::invalid_argument( "Get Master HighLayer Controller Error" );
		}
	}

	void HighLayerScheduler::MakeScheme()
	{
		if ( shared_ptr<HighLayerController> pController = m_pMasterController.lock() )
		{
			//����ͨ��GetLTENBReport��GetHSPANBReport���õ�LTE��HSPA eNodB������С����Ϣ


			//��򵥵ķ�����
			//����ÿһ��Composite Service����Buffer�е����ݽ��о���

			for ( SERVICESET::iterator setIter = m_ServiceSet.begin(); setIter != m_ServiceSet.end(); ++setIter )
			{
				shared_ptr<ServiceComposite> pCompositeService = *setIter;
				if( pCompositeService->IsDataDistributed() == false )
				{
					double dLTERatio = GetLTERatio( dynamic_pointer_cast<DualModeFTPService>(pCompositeService) );
					SimLog::Instance()->LogLTERatio( pCompositeService->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), dLTERatio );
					double dTotalDataVolume = pCompositeService->GetData();
					DataDistributeScheme dataDistScheme;
					dataDistScheme.AddSchemeNode( DataDistributeSchemeNode( SYSTEM_LTE, dTotalDataVolume * dLTERatio ) );
					dataDistScheme.AddSchemeNode( DataDistributeSchemeNode( SYSTEM_HSPA, dTotalDataVolume * ( 0.0 ) ) );//���޸ģ�ѡ��ִ����һ��
					dataDistScheme.AddSchemeNode( DataDistributeSchemeNode( SYSTEM_WIFI, dTotalDataVolume * (1.0 - dLTERatio ) ) );
					m_ServiceDataDistSchemeMap.insert( std::make_pair( pCompositeService, dataDistScheme ) );
				}
			}
		}
		else
		{
			throw std::runtime_error( "Get Master High Layer Controller Error" );
		}
	}

	void HighLayerScheduler::GenerateAndAddCommand()
	{
		for (	SERVICEDATADISTSCHEMEMAP::iterator mapIter = m_ServiceDataDistSchemeMap.begin();
				mapIter != m_ServiceDataDistSchemeMap.end();
				++mapIter )
		{
			shared_ptr<ServiceComposite> pCompositeService = mapIter->first;
			shared_ptr<IUE> pIUE = pCompositeService->GetUE();
			//��Command��û�п�����ʱ������һ��ʱ϶�ͻ�ִ��
			shared_ptr<Command> pCommand( new DataDistributeCommand( pCompositeService, mapIter->second ) );
			shared_ptr<CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>( pIUE );
			pCmdEngine->AddCommand( pCommand );
		}
	}

	double HighLayerScheduler::GetLTERatio( shared_ptr<DualModeFTPService> pDualService )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		double dLTERatio = pSystem->GetConfig().GetDouble("DualMode_UE_LTERatio");
		const std::string& sDistributeMode = pSystem->GetConfig().GetString("DualMode_UE_Service_DistributeMode");
		int iDynamic = pSystem->GetConfig().GetInt("DualMode_UE_LTERatio_Dynamic");

		if ( sDistributeMode == "Data" )
		{
			if ( iDynamic )
			{	
				if ( ClockManager::Instance().GetFrameClock().GetClock() == 0 )
				{
					pDualService -> SetLTERatio(dLTERatio);
					return dLTERatio;
				}
				double rate_L = pDualService->GetFileSize() * pDualService->GetLTERatio() / ( pDualService->GetServiceElement(SYSTEM_LTE)->GetPacketEndTime() - pDualService->GetPacketStartTime() );
				double rate_W = pDualService->GetFileSize() * (1-pDualService->GetLTERatio()) / ( pDualService->GetServiceElement(SYSTEM_WIFI)->GetPacketEndTime() - pDualService->GetPacketStartTime() );
				dLTERatio = rate_L / ( rate_L+rate_W );
			}
		}

		else if ( sDistributeMode == "Packet" )
		{
			if ( iDynamic )
			{
				if ( ClockManager::Instance().GetFrameClock().GetClock() == 0 )
				{ 
					if ( !m_bSelectLTE )
					{
						shared_ptr<NodeB> pNodeB = pSystem -> GetNodeBFromOffsetID( m_pMasterController.lock()->GetID() );
						int lteue = pSystem->GetConfig().GetInt("Select_LTE_UE_Number");
						const NodeB::UELIST& CandidateUEList = pNodeB->GetUEList();
						vector<UEIDDistance> iddis;
						UEIDDistance one;
						NodeB::UELIST::const_iterator pServicePtr = CandidateUEList.begin();

						for ( ; pServicePtr != CandidateUEList.end(); ++pServicePtr )
						{
							shared_ptr<ILTEUE> pUE(pServicePtr->second);
							if( pUE->GetType() != SYSTEM_DUAL )	continue;
							double dis = ::Distance(pNodeB->GetPos(),pUE->GetPos());
							one.pUE = pUE;
							one.distance = dis;
							iddis.push_back(one);
						}
						std::sort(iddis.begin(),iddis.end());
						int length = iddis.size()<lteue?iddis.size():lteue;
						iddis.resize(length);
						vector<UEIDDistance>::iterator i = iddis.begin();
						for ( ; i != iddis.end(); ++i )
						{
							shared_ptr<LTEWiFiUE> pp = dynamic_pointer_cast<LTEWiFiUE>((*i).pUE);
							pp ->SetBoolNear(true);
						}
						m_bSelectLTE = true;
					}
					shared_ptr<LTEWiFiUE> p = dynamic_pointer_cast<LTEWiFiUE> (pDualService->GetUE());
					p->GetBoolNear()?dLTERatio=1.0:dLTERatio=0.0;
				}
				else
				{
					dLTERatio = pDualService->GetLTERatio(); 

					//int id = m_pMasterController.lock()->GetID();
					//shared_ptr<WiFiAP> ap = pSystem->GetWiFiAPFromOffsetID(id);
					//double wificu = ap->GetTransmitDataTime() / ( ClockManager::Instance().GetCurrentTime()*1000.0 );
					//if ( wificu<0.95 )
					//	dLTERatio = 0.0;
					//else
					//	dLTERatio = 1.0;
					//std::cout<<endl<<wificu;

					//double LTEMaxCellThr = System::GetCurrentSystem()->GetConfig().GetDouble("LTE_Max_Cell_Thr");
					//double WiFiMaxCellThr = System::GetCurrentSystem()->GetConfig().GetDouble("WiFi_Max_Cell_Thr");
					//shared_ptr<LTEWiFiUE> pUE = dynamic_pointer_cast<LTEWiFiUE>(pDualService->GetUE());
					//double LTECellThr = pUE -> GetServingNodeB() -> GetCellThroughput() / ClockManager::Instance().GetCurrentTime() /1000;
					//double WiFiCellThr = pUE -> GetServingAP() -> GetCellThroughput() / ClockManager::Instance().GetCurrentTime() /1000;
					//if ( WiFiCellThr <= WiFiMaxCellThr*0.95 )
					//	dLTERatio = 0.0;
					//else
					//	dLTERatio = 1.0;
				}
			} 
			else
			{
				UNIFORM_REAL_GEN ServiceModeGen = RandomManager::GetRandomManager().GetUniformRealGen(0.0, 1.0);	
				if ( ServiceModeGen() <= dLTERatio )
				{
					dLTERatio = 1.0;
				}
				else dLTERatio = 0.0;
			}
		}

		else if ( sDistributeMode == "User" )
		{
			if ( iDynamic )
			{

			} 
			else
			{
				if ( ClockManager::Instance().GetFrameClock().GetClock() == 0 )
				{
					UNIFORM_REAL_GEN ServiceModeGen = RandomManager::GetRandomManager().GetUniformRealGen(0.0, 1.0);	
					if ( ServiceModeGen() <= dLTERatio )
					{
						dLTERatio = 1.0;
					}
					else dLTERatio = 0.0;
				}
				else
				{
					dLTERatio = pDualService->GetLTERatio(); 
				}
			}
		}
		else
			throw std::invalid_argument( "δ֪��ҵ��ָʽ" );

		pDualService->SetLTERatio(dLTERatio);
		return dLTERatio;
	}

	void HighLayerScheduler::Clear()
	{
		m_ServiceSet.clear();
		m_ServiceDataDistSchemeMap.clear();
	}

}