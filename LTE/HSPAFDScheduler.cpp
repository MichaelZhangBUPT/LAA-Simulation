#include "HSPAFDScheduler.h"
#include "commondef.h"
#include "Service.h"
#include "System.h"
#include "Configuration.h"
#include "RunParameters.h"
#include "HSPANodeB.h"
#include "HSPAUE.h"
#include "Command.h"
#include "ChannelMaker.h"
#include "ClockManager.h"
#include "Clock.h"

#include <vector>
#include <map>
#include <algorithm>
#include <functional>

namespace LTESim
{
	HSPAFDDownlinkScheduler::HSPAFDDownlinkScheduler(shared_ptr<HSPANodeB> pNodeB)
		: m_pMasterNodeB( pNodeB )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_iCodeNumPerUE = pSystem->GetConfig().GetInt("HSPA_CodeNumPerUE");
		m_iTotalCodeNum = pSystem->GetConfig().GetInt("HSPA_TotalCodeNum");
	}

	HSPAFDDownlinkScheduler::~HSPAFDDownlinkScheduler()
	{

	}

	void HSPAFDDownlinkScheduler::Process( SERVICELIST& refServiceList )
	{
		Clear();

		GetCandidateServiceList( refServiceList );

		AllocateCodeChannelToService();

		InitChannels();

		AddScheduledChannelsToTransmitter();

		Clear();
	}

	void HSPAFDDownlinkScheduler::Clear()
	{
		m_CandServiceList.clear();
		m_ScheduledChannelMap.clear();
		m_ServiceCodeMap.clear();
	}

	void HSPAFDDownlinkScheduler::GetCandidateServiceList( const SERVICELIST& refServiceList )
	{
		//���Ƚ�ʱ����ȵ�CandidateServiceList��ֵ��Ƶ����ȵ�CandidateServiceList��
		m_CandServiceList.clear();
		for ( SERVICELIST::const_iterator vecIter = refServiceList.begin(); vecIter != refServiceList.end(); ++vecIter )
			m_CandServiceList.push_back( *vecIter );
	}

	void HSPAFDDownlinkScheduler::AllocateCodeChannelToService()
	{
		if ( m_CandServiceList.empty() )
			return;
		//������HSDPA��ֻ���ǵ���һ��UE���������е�������������UE�����Դ����m_CandServiceList���ֻ��һ��Service
		//MAKE_SURE( m_CandServiceList.size() == 1, "HSPAFDDownlinkScheduler::AllocateCodeChannelToService()" );

		//ʵ�ֵ��ǽ����е����������������ϵ�UE
		//ֻѡ��һ��UE�����ҽ����е�CodeChannel�������UE
		//shared_ptr<Service> pService = m_CandServiceList[0];
		//std::vector<int>	codeChannelVec;
		//for ( int iCodeIndex = 0; iCodeIndex < m_iTotalCodeNum; ++iCodeIndex )
		//	codeChannelVec.push_back( iCodeIndex );

		//m_ServiceCodeMap.insert( std::make_pair( pService, codeChannelVec ) );

		SERVICELIST::const_iterator vecIter = m_CandServiceList.begin();
		for ( int i = 0; vecIter != m_CandServiceList.end(); ++vecIter, ++i )
		{
			shared_ptr<Service> pService = *vecIter;
			std::vector<int> vecCodeChannel;
			for ( int j = 0; j < m_iCodeNumPerUE; ++j )
			{
				vecCodeChannel.push_back( i*m_iCodeNumPerUE + j );
			}
			m_ServiceCodeMap.insert( std::make_pair( pService, vecCodeChannel ) );
		}
	}

	void HSPAFDDownlinkScheduler::InitChannels()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		ClockManager& refClkManager = ClockManager::Instance();
		int iFrame = refClkManager.GetFrameClock().GetClock();
		int iSubFrame = refClkManager.GetHSPASubFrameClock().GetClock();

		ChannelMaker channelMaker;

		if ( shared_ptr<HSPANodeB> pNodeB = m_pMasterNodeB.lock() )
		{
			int iTransAntennaPortNum = pNodeB->GetAntenna()->GetAntennaPortNum();
			double dNBTxPower = pNodeB->GetTransmitPower();
			const HSPANodeB::UELIST& rUEList = pNodeB->GetUEList();
			//��Ҫ���������HSPANodeB������UE�������������������Channel�����з���
			//���δ�����ϣ������趨Ϊδ���ȣ����Ҹ���ƽ��������
			for ( HSPANodeB::UELIST::const_iterator uelistIter = rUEList.begin(); uelistIter != rUEList.end(); ++uelistIter )
			{
				if(shared_ptr<IHSPAUE> pUE = uelistIter->second.lock())
				{
					shared_ptr<Service> pService = pUE->GetService( SYSTEM_HSPA );
					SERVICECODELISTMAP::iterator mapIter = m_ServiceCodeMap.find( pService );
					if ( mapIter == m_ServiceCodeMap.end() )
					{
						//û�е����ϵ�UE����ƽ���������ĸ��£�����PF�㷨
						pUE->UpdateAverageThroughput( shared_ptr<HSPAChannel>() );
						pService->SetIsScheduled( false );
					}
					else
					{
						//�������˸�UE����ô������Ҫ���¸�UE��HARQ����
						pUE->UpdateDLHARQProcess( SYSTEM_HSPA );
						shared_ptr<HARQ> pHARQ = pUE->GetCurrentDLHARQ( SYSTEM_HSPA );
						shared_ptr<IHSPAReporter> pReporter = pUE->GetHSPAReporter();

						//�õ���Channelʹ�õ�MCS
						MCS mcs(0, SYSTEM_HSPA);
						if ( pHARQ->GetNewPacketIndicator() )
						{
							//������µ����ݣ���ô��ʹ���µ��Ƽ���MCS
							mcs = pReporter->GetRecommendMCS();
						}
						else
						{
							//������ش�����ô��ʹ��ԭ��HARQ��MCS
							mcs = pHARQ->GetMCS();
						}
						//��ȷ����MCS֮�󣬸�HARQ��Ҫ�Ѵ˴δ����õ�MCS�ȼ�����
						pHARQ->SetMCS( mcs );

						//׼�����з��͵�Ȩ������
						const Matrix& refRecDLWeightMatrix = pReporter->GetRecommendDLTransWeightMatrix();

						//׼�����͵���
						//����������ʹ�õ��Ƿּ�������һ��Channel����ʹ��2���������������ͬ������
						std::vector<int> streamVec;
						for( int iStreamIndex = 0; iStreamIndex < iTransAntennaPortNum; ++iStreamIndex )
							streamVec.push_back( iStreamIndex );

						//����Channel��Ȼ����뵽m_ScheduledChannelVec��
						//To Be Modified Add MCS Parameter
						shared_ptr<HSPAChannel> pChannel = channelMaker.CreateHSPADLSCH( pUE, mcs, dNBTxPower, iFrame, iSubFrame, streamVec, mapIter->second, refRecDLWeightMatrix );
						//����ÿ��Channel������Ҫ�з����ʱ���ӳ�
						//��HSPAϵͳ�У���ʱ�Ļ�����λΪһ��HSPAϵͳSlot��ʱ������LTEϵͳ�У���ΪLTE Slot��ʱ��
						//To Be Modified
						int iTimeDelay = 0;
						m_ScheduledChannelMap.insert(std::make_pair( pChannel, iTimeDelay ) );

						//������Channel��ʱ��ͬʱ�Ե����ϵ�UE����״̬���趨
						pService->SetIsScheduled( true );
					}
				}
				else
				{
					throw std::runtime_error("Get UE Pointer Error");
				}
			}
		}
		else
			throw std::runtime_error("Get Master NodeB Error");
	}

	void HSPAFDDownlinkScheduler::AddScheduledChannelsToTransmitter()
	{
		for(	std::map<shared_ptr<HSPAChannel>, int>::iterator mapIter = m_ScheduledChannelMap.begin(); 
				mapIter != m_ScheduledChannelMap.end(); ++mapIter )
		{
			shared_ptr<HSPAChannel> pChannel = mapIter->first;
			shared_ptr<HSPANodeB> pNodeB = pChannel->GetNodeB();
			if ( pChannel->GetLinkDirection() == DOWNLINK )
			{
				pNodeB->TransmitChannel( pChannel, mapIter->second );
				pNodeB->TransmitChannel( pChannel, mapIter->second+1 );
				pNodeB->TransmitChannel( pChannel, mapIter->second+2 );
			}
			else
				throw UnFinishedFunc();
		}
	}
}