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
			m_dFileSize( dFileSize * 8 ), 									//To Be Modified��Buffer�е�������BitΪ��λ
			m_dReadingTimeMean( dReadingTimeMean )
	{
		//��ʼ��Buffer�е�������
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

	//����������
	void DualModeFTPService::UpdateThroughput( DATASIZE throughput ) 
	{
		//Service Composite���������������������Service Element֮��
		if ( m_dBufferData <= throughput )
		{
			//��Buffer��û�����ݵ�ʱ�򣬽���Service��ΪEnd
			m_SumThroughput += m_dBufferData;
			m_dBufferData = 0.0;
		}
		else
		{
			m_dBufferData -= throughput;
			m_SumThroughput += throughput;
		}

		//if ( 0 && !m_bIsRatioSure )	//��һ��ϵͳ�������ʱ��ȡ����ϵͳ�Ѵ��������֮����Ϊ�¸����ı���
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
			m_dPacketEndTime = ClockManager::Instance().GetCurrentTime();			//ͳ�ư�����ʱ��
			m_vecPacketTransTime.push_back( m_dPacketEndTime - m_dPacketStartTime );

			//����������ݰ�������ϵͳ�е����ݴ������ʱȣ���Ϊ�¸����ķָ����
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

			//�����µ�����
			//���ȸ���Reading Time������ָ���ֲ�
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

	void DualModeFTPService::UpdateBuffer(unsigned int timespan/*������ʱ����ms*/)
	{
		//B'	
		m_dUpdateBufferTime = ClockManager::Instance().GetCurrentTime();		//ͳ�����ݰ��Ķ�ȡʱ��
		m_vecPacketReadingTime.push_back( m_dUpdateBufferTime - m_dPacketEndTime );
		m_dBufferData = m_dFileSize;
		//Buffer�е����ݸ��º���Ҫ���ַ���־λ��ΪFalse���Ӷ������´ν������ݷ���
		m_bIsDataDistributed = false;
		//��Buffer�������ݵ�ʱ�򣬽���Service��Ϊδ��������������ʹ��NodeB����
		SetIsEnd(false);
		//���ݰ��Ĵ����������				
		m_pLTEService->ResetPacketTransTimes();
		m_pLTEService->ResetPacketFailedTransTimes();
		m_pHSPAService->ResetPacketTransTimes();
		m_pHSPAService->ResetPacketFailedTransTimes();
		//���ҵ�����������PacketDynamic����ȥ���߲������������ʱ�ӣ���������ҵ��
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

	//���¶������ݰ���������
	void DualModeFTPService::UpdateDiscardThroughput( DATASIZE throughput )		
	{
		//To Be Modified
		//Service Composite����Ķ�����������������������Service Element������������֮��
		m_SumDiscardThroughput += throughput;
	}

	//����ͳ�Ʒ��ʽӿ�
	void DualModeFTPService::Accept( ServiceStatistic& visit )										
	{
		//����Composite Service����Ҫ������Service Element
		visit.Visit( *this );
	}

	//����MCS�ȼ�
	void DualModeFTPService::UpdateMCSIndex( int mcsindex )									
	{
		//To Be Modified
		//throw UnFinishedFunc();
	}

	void DualModeFTPService::UpdateGenerateInterval( void )
	{
		const double dBeta = 1.0 / m_dReadingTimeMean;
		EXPONENTIAL_DOUBLE_GEN ExpnentialDoubleGen = RandomManager::GetRandomManager().GetExponenialDoubleGen(dBeta);
		//��Frame��ʱ������10msΪ��λ���ڽ���Buffer�����������µ�ʱ����ʹ�õ�FrameClock
		m_iGenInterval = static_cast<int>(ExpnentialDoubleGen()/0.01 + 1.0);
		//m_iGenInterval = 250;	//�̶���ȡʱ��Ϊ2.5s
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
		//ֻ����δDistributed��״̬�²��ܹ����зַ�����
		if( !m_bIsDataDistributed )
		{
			double currenttime = ClockManager::Instance().GetCurrentTime();		
			//ΪLTE Service����Buffer����
			double dLTEBufferDataVol = rScheme.GetBufferDataVolume( SYSTEM_LTE );
			m_pLTEService->SetData( dLTEBufferDataVol );
			//���������ݸ�Auxiliary Service��Buffer�У�����������Service��ʹ��ɵ���
			if ( dLTEBufferDataVol > 0.0 )
			{
				m_pLTEService->SetIsEnd( false );
				//m_pLTEService->SetPacketStartTime( currenttime );
			}
			//ΪHSPA Service����Buffer����
			double dHSPABufferDataVol = rScheme.GetBufferDataVolume( SYSTEM_HSPA );
			m_pHSPAService->SetData( dHSPABufferDataVol );
			//���������ݸ�Auxiliary Service��Buffer�У�����������Service��ʹ��ɵ���
			if ( dHSPABufferDataVol > 0.0 )
			{
				m_pHSPAService->SetIsEnd( false );
				//m_pHSPAService->SetPacketStartTime( currenttime );
			}
			//ΪWiFi Service����Buffer����
			double dWiFiBufferDataVol = rScheme.GetBufferDataVolume( SYSTEM_WIFI );
			m_pWiFiService->SetData( dWiFiBufferDataVol );
			//���������ݸ�Auxiliary Service��Buffer�У�����������Service��ʹ��ɵ���
			if ( dWiFiBufferDataVol > 0.0 )
			{
				m_pWiFiService->SetIsEnd( false );
				//m_pWiFiService->SetPacketStartTime( currenttime );
			}
			//�ڽ����ݽ��з���֮�󣬽�����ı�־��ΪTrue
			m_bIsDataDistributed = true;

			//A ͳ�����ڸ߲���������������ʱ
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

	std::vector<double> DualModeFTPService::GetHighLayerDelay()		//������ڸ߲���������������ʱ
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