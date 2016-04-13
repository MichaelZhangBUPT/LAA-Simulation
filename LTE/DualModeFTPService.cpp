#include "commondef.h"
#include "AuxiliaryService.h"
#include "DualModeFTPService.h"
#include "DataDistributeScheme.h"
#include "RandomManager.h"
#include "Clock.h"
#include "ClockManager.h"
#include "Command.h"
#include "FTPUpdateCommand.h"
#include "TimedCommand.h"
#include "SimLog.h"
#include "ServiceStatistic.h"
#include "RunParameters.h"

#include <limits>
#include <boost\enable_shared_from_this.hpp>

namespace LTESim
{
	DualModeFTPService::DualModeFTPService(shared_ptr<IUE> pUE, double dFileSize, double dReadingTimeMean)
		:	ServiceComposite(pUE),
			m_bIsDataDistributed( false ),
			m_dFileSize( dFileSize * 8 ), 									//To Be Modified，Buffer中的数据以Bit为单位
			m_dReadingTimeMean( dReadingTimeMean )
	{
		//初始化Buffer中的数据量
		//m_dFileSize = std::numeric_limits<double>::max();

		m_dBufferData = m_dFileSize;
		m_dUpdateBufferTime = 0.0;
		m_bIsRatioSure = false;
		m_dLTERatio = System::GetCurrentSystem()->GetConfig().GetDouble("DualMode_UE_LTERatio");
		m_ServiceType = DualModeFTP;

	}

	DualModeFTPService::~DualModeFTPService(void)
	{

	}

	shared_ptr<DualModeFTPService> DualModeFTPService::CreateInstance(shared_ptr<IUE> pUE, double dFileSize, double dReadingTimeMean )
	{
		shared_ptr<DualModeFTPService> pInstance( new DualModeFTPService(pUE, dFileSize, dReadingTimeMean ) );
		pInstance->m_pLTEService.reset( new AuxiliaryService( pUE, pInstance ) );
		pInstance->m_pHSPAService.reset( new AuxiliaryService( pUE, pInstance ) );
		pInstance->m_pWiFiService.reset( new AuxiliaryService( pUE, pInstance ) );
		return pInstance;
	}

	//更新吞吐量
	void DualModeFTPService::UpdateThroughput( DATASIZE throughput ) 
	{
		//Service Composite整体的吞吐量，包括两个Service Element之和
		if ( m_dBufferData <= throughput )
		{
			//当Buffer中没有数据的时候，将该Service置为End
			m_SumThroughput += m_dBufferData;
			m_dBufferData = 0.0;
		}
		else
		{
			m_dBufferData -= throughput;
			m_SumThroughput += throughput;
		}

		//if ( 0 && !m_bIsRatioSure )	//在一个系统传输完毕时，取两个系统已传输的数据之比作为下个包的比例
		//{		//Dynamic1
		//	if ( m_pLTEService->GetIsEnd() || m_pHSPAService->GetIsEnd() )
		//	{
		//		double LTEData = dynamic_pointer_cast<AuxiliaryService>(m_pLTEService) -> GetThrInAPacket();
		//		double HSPAData = dynamic_pointer_cast<AuxiliaryService>(m_pHSPAService) -> GetThrInAPacket();
		//		m_dLTERatio = LTEData / (HSPAData+LTEData);
		//		m_bIsRatioSure = true;
		//	}
		//}

		if ( m_pLTEService->GetIsEnd() && m_pHSPAService->GetIsEnd() && m_pWiFiService->GetIsEnd() )
		{
			//B
			SetIsEnd(true);
			m_dPacketEndTime = ClockManager::Instance().GetCurrentTime();			//统计包传输时间
			m_vecPacketTransTime.push_back( m_dPacketEndTime - m_dPacketStartTime );

			//根据这个数据包在两个系统中的数据传输速率比，作为下个包的分割比例
			//int iDynamic = System::GetCurrentSystem()->GetConfig().GetInt("DualMode_UE_LTERatio_Dynamic");
			//if ( iDynamic ==2 || iDynamic == 3 )
			//{
			//	double rate_L = m_dFileSize * m_dLTERatio / ( m_pLTEService->GetPacketEndTime() - m_pLTEService->GetPacketStartTime() );
			//	double rate_H = m_dFileSize * ( 1 - m_dLTERatio) / ( m_pHSPAService->GetPacketEndTime() - m_pHSPAService->GetPacketStartTime() );
			//	double dLTERateRatio = rate_L / ( rate_H + rate_L );
			//	if ( iDynamic == 2 )
			//		m_dLTERatio = dLTERateRatio;
			//	if ( iDynamic == 3 )
			//		m_dLTERatio = sqrt( m_dLTERatio * dLTERateRatio );
			//}
			//if ( iDynamic == 4 )
			//{
			//	double time_L = m_pLTEService->GetPacketEndTime() - m_pLTEService->GetPacketStartTime();
			//	double time_H = m_pHSPAService->GetPacketEndTime() - m_pHSPAService->GetPacketStartTime();
			//	double Pe_L = (double)m_pLTEService->GetPacketFailedTransTimes() / (double)m_pLTEService->GetPacketTransTimes();
			//	double Pe_H = (double)m_pHSPAService->GetPacketFailedTransTimes() / (double)m_pHSPAService->GetPacketTransTimes();
			//	//std::cout<<endl<<m_pLTEService->GetPacketFailedTransTimes()<<"  "<<m_pLTEService->GetPacketTransTimes()<<"  "<<Pe_L<<"  "<<m_pHSPAService->GetPacketFailedTransTimes()<<"  "<<m_pHSPAService->GetPacketTransTimes()<<"  "<<Pe_H<<"  "<<endl;
			//	double A_L = ( 1 - Pe_L ) / time_L;
			//	double A_H = ( 1 - Pe_H ) / time_H;
			//	m_dLTERatio = A_L / ( A_L + A_H );
			//}
			SimLog::Instance()->LogDualTimeDifference(m_pUE.lock()->GetID(), m_dPacketStartTime, 
				m_pLTEService->GetPacketEndTime(), m_pWiFiService->GetPacketEndTime(), m_pWiFiService->GetPacketEndTime() - m_pLTEService->GetPacketEndTime() );

			//产生新的数据
			//首先更新Reading Time，服从指数分布
			UpdateGenerateInterval();
			shared_ptr<IUE> pIUE = m_pUE.lock();
			Clock& rFrameClock = ClockManager::Instance().GetFrameClock();
			Command::PTR pFTPUpdateCommand( new FTPUpdateCommand( pIUE ) );
			CMDEngine::PTR pCmdEngine = dynamic_pointer_cast<CMDEngine>( pIUE );
			TimedCommand::PTR pTimedCommand( new TimedCommand( rFrameClock, pCmdEngine, pFTPUpdateCommand ) );
			pTimedCommand->SetTimer( m_iGenInterval );
			//pTimedCommand->SetTimer(1);
			pCmdEngine->AddCommand( pTimedCommand );

			SimLog::Instance()->LogServiceBufferUpdate( pIUE->GetID(), rFrameClock.GetClock(), m_iGenInterval, pIUE->GetType() );
		}
	}

	void DualModeFTPService::UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/)
	{
		//B'	
		m_dUpdateBufferTime = ClockManager::Instance().GetCurrentTime();		//统计数据包的读取时间
		m_vecPacketReadingTime.push_back( m_dUpdateBufferTime - m_dPacketEndTime );
		m_dBufferData = m_dFileSize;
		//Buffer中的数据更新后，需要将分发标志位置为False，从而允许下次进行数据分配
		m_bIsDataDistributed = false;
		//当Buffer中有数据的时候，将该Service置为未结束，这样可以使得NodeB调度
		SetIsEnd(false);
		//数据包的传输次数置零				
		m_pLTEService->ResetPacketTransTimes();
		m_pLTEService->ResetPacketFailedTransTimes();
		m_pHSPAService->ResetPacketTransTimes();
		m_pHSPAService->ResetPacketFailedTransTimes();
		//如果业务分流方案是PacketDynamic，则去除高层控制器带来的时延，立即分配业务
		const std::string& sDistributeMode = System::GetCurrentSystem()->GetConfig().GetString("DualMode_UE_Service_DistributeMode");
		if ( sDistributeMode == "Packet" )
		{
			if( m_dLTERatio == 1.0 )
			{
				m_pLTEService->SetData( m_dFileSize );
				m_pLTEService->SetIsEnd( false );
			}
			else
			{
				m_pWiFiService->SetData( m_dFileSize );
				m_pWiFiService->SetIsEnd( false );
			}
			m_bIsDataDistributed = true;
			SimLog::Instance()->LogLTERatio( m_pUE.lock()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), m_dLTERatio );
		}
	}

	//更新丢弃数据包的吞吐量
	void DualModeFTPService::UpdateDiscardThroughput( DATASIZE throughput )		
	{
		//To Be Modified
		//Service Composite整体的丢弃包的吞吐量，包括两个Service Element丢弃包数据量之和
		m_SumDiscardThroughput += throughput;
	}

	//数据统计访问接口
	void DualModeFTPService::Accept( ServiceStatistic& visit )										
	{
		//访问Composite Service，需要遍历其Service Element
		visit.Visit( *this );
	}

	//更新MCS等级
	void DualModeFTPService::UpdateMCSIndex( int mcsindex )									
	{
		//To Be Modified
		//throw UnFinishedFunc();
	}

	void DualModeFTPService::UpdateGenerateInterval( void )
	{
		const double dBeta = 1.0 / m_dReadingTimeMean;
		EXPONENTIAL_DOUBLE_GEN ExpnentialDoubleGen = RandomManager::GetRandomManager().GetExponenialDoubleGen(dBeta);
		//以Frame的时间周期10ms为单位，在进行Buffer中数据量更新的时候都是使用的FrameClock
		m_iGenInterval = static_cast<int>(ExpnentialDoubleGen()/0.01 + 1.0);
		//m_iGenInterval = 250;	//固定读取时间为2.5s
		if (m_iGenInterval < 0.0)
		{
			throw std::runtime_error( "Invalid Buffer Data Generate Interval" );
		}
	}

	shared_ptr<Service> DualModeFTPService::GetServiceElement( SystemType systemType )
	{
		if ( systemType == SYSTEM_LTE )
			return m_pLTEService;
		else if ( systemType == SYSTEM_HSPA )
			return m_pHSPAService;
		else if ( systemType == SYSTEM_WIFI )
			return m_pWiFiService;
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	void DualModeFTPService::DistributeBufferData( const DataDistributeScheme& rScheme )
	{
		assert( m_bIsDataDistributed == false );
		//只有在未Distributed的状态下才能够进行分发数据
		if( !m_bIsDataDistributed )
		{
			double currenttime = ClockManager::Instance().GetCurrentTime();		
			//为LTE Service分配Buffer数据
			double dLTEBufferDataVol = rScheme.GetBufferDataVolume( SYSTEM_LTE );
			m_pLTEService->SetData( dLTEBufferDataVol );
			//分配了数据给Auxiliary Service的Buffer中，并且启动该Service，使其可调度
			if ( dLTEBufferDataVol > 0.0 )
			{
				m_pLTEService->SetIsEnd( false );
				//m_pLTEService->SetPacketStartTime( currenttime );
			}
			//为HSPA Service分配Buffer数据
			double dHSPABufferDataVol = rScheme.GetBufferDataVolume( SYSTEM_HSPA );
			m_pHSPAService->SetData( dHSPABufferDataVol );
			//分配了数据给Auxiliary Service的Buffer中，并且启动该Service，使其可调度
			if ( dHSPABufferDataVol > 0.0 )
			{
				m_pHSPAService->SetIsEnd( false );
				//m_pHSPAService->SetPacketStartTime( currenttime );
			}
			//为WiFi Service分配Buffer数据
			double dWiFiBufferDataVol = rScheme.GetBufferDataVolume( SYSTEM_WIFI );
			m_pWiFiService->SetData( dWiFiBufferDataVol );
			//分配了数据给Auxiliary Service的Buffer中，并且启动该Service，使其可调度
			if ( dWiFiBufferDataVol > 0.0 )
			{
				m_pWiFiService->SetIsEnd( false );
				//m_pWiFiService->SetPacketStartTime( currenttime );
			}
			//在将数据进行分配之后，将分配的标志置为True
			m_bIsDataDistributed = true;

			//A 统计由于高层更新周期引入的延时
			m_dPacketStartTime = ClockManager::Instance().GetCurrentTime();
			if( m_dPacketStartTime > 0.0 )
				m_vecHighLayerDelay.push_back( m_dPacketStartTime - m_dUpdateBufferTime );

			//SimLog::Instance()->LogCompositeServiceDistributeBuffer(m_pUE.lock()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), dLTEBufferDataVol, dHSPABufferDataVol);
		}
	}

	bool DualModeFTPService::IsDataDistributed()
	{
		return m_bIsDataDistributed;
	}

	std::vector<double> DualModeFTPService::GetHighLayerDelay()		//获得由于高层更新周期引入的延时
	{
		return m_vecHighLayerDelay;
	}

	double DualModeFTPService::GetLTERatio()
	{
		return m_dLTERatio;
	}

	void DualModeFTPService::SetLTERatio(double ratio)
	{
		m_dLTERatio = ratio;
	}

	double DualModeFTPService::GetFileSize()
	{
		return m_dFileSize;
	}

	//shared_ptr<NodeB> DualModeFTPService::GetServingNodeB()
	//{
	//	m_pLTEService->GetUE()->;
	//}

	//shared_ptr<WiFiAP> DualModeFTPService::GetServingAP()
	//{
	//	m_pWiFiService->GetUE()->AddService;
	//}

}