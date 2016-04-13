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
		//首先将时域调度的CandidateServiceList赋值到频域调度的CandidateServiceList中
		m_CandServiceList.clear();
		for ( SERVICELIST::const_iterator vecIter = refServiceList.begin(); vecIter != refServiceList.end(); ++vecIter )
			m_CandServiceList.push_back( *vecIter );
	}

	void HSPAFDDownlinkScheduler::AllocateCodeChannelToService()
	{
		if ( m_CandServiceList.empty() )
			return;
		//由于在HSDPA中只考虑调度一个UE，并且所有的码道都分配各该UE，所以传入的m_CandServiceList最多只有一个Service
		//MAKE_SURE( m_CandServiceList.size() == 1, "HSPAFDDownlinkScheduler::AllocateCodeChannelToService()" );

		//实现的是将所有的码道都分配给调度上的UE
		//只选择一个UE，并且将所有的CodeChannel分配给该UE
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
			//需要遍历接入该HSPANodeB的所有UE，如果调度上了则生成Channel，进行发送
			//如果未调度上，则将其设定为未调度，并且更新平均吞吐量
			for ( HSPANodeB::UELIST::const_iterator uelistIter = rUEList.begin(); uelistIter != rUEList.end(); ++uelistIter )
			{
				if(shared_ptr<IHSPAUE> pUE = uelistIter->second.lock())
				{
					shared_ptr<Service> pService = pUE->GetService( SYSTEM_HSPA );
					SERVICECODELISTMAP::iterator mapIter = m_ServiceCodeMap.find( pService );
					if ( mapIter == m_ServiceCodeMap.end() )
					{
						//没有调度上的UE进行平均吞吐量的更新，用于PF算法
						pUE->UpdateAverageThroughput( shared_ptr<HSPAChannel>() );
						pService->SetIsScheduled( false );
					}
					else
					{
						//调度上了该UE，那么首先需要更新该UE的HARQ进程
						pUE->UpdateDLHARQProcess( SYSTEM_HSPA );
						shared_ptr<HARQ> pHARQ = pUE->GetCurrentDLHARQ( SYSTEM_HSPA );
						shared_ptr<IHSPAReporter> pReporter = pUE->GetHSPAReporter();

						//得到该Channel使用的MCS
						MCS mcs(0, SYSTEM_HSPA);
						if ( pHARQ->GetNewPacketIndicator() )
						{
							//如果是新的数据，那么就使用新的推荐的MCS
							mcs = pReporter->GetRecommendMCS();
						}
						else
						{
							//如果是重传，那么就使用原来HARQ的MCS
							mcs = pHARQ->GetMCS();
						}
						//在确定了MCS之后，该HARQ需要把此次传输用的MCS等级记下
						pHARQ->SetMCS( mcs );

						//准备下行发送的权重向量
						const Matrix& refRecDLWeightMatrix = pReporter->GetRecommendDLTransWeightMatrix();

						//准备发送的流
						//在这里由于使用的是分集，所以一个Channel中有使用2个流，传输的是相同的数据
						std::vector<int> streamVec;
						for( int iStreamIndex = 0; iStreamIndex < iTransAntennaPortNum; ++iStreamIndex )
							streamVec.push_back( iStreamIndex );

						//生成Channel，然后插入到m_ScheduledChannelVec中
						//To Be Modified Add MCS Parameter
						shared_ptr<HSPAChannel> pChannel = channelMaker.CreateHSPADLSCH( pUE, mcs, dNBTxPower, iFrame, iSubFrame, streamVec, mapIter->second, refRecDLWeightMatrix );
						//对于每个Channel，还需要有发射的时间延迟
						//在HSPA系统中，延时的基本单位为一个HSPA系统Slot的时长，在LTE系统中，则为LTE Slot的时长
						//To Be Modified
						int iTimeDelay = 0;
						m_ScheduledChannelMap.insert(std::make_pair( pChannel, iTimeDelay ) );

						//在生成Channel的时候，同时对调度上的UE进行状态的设定
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