#include "commondef.h"
#include "DLFTPService.h"
#include "ServiceStatistic.h"
#include "UE.h"
#include "WiFiUE.h"
#include "LAAUE.h"
#include "ClockManager.h"
#include "TimedCommand.h"
#include "FTPUpdateCommand.h"
#include "SimLog.h"

#include <limits>

namespace LTESim
{

    DLFTPService::DLFTPService(shared_ptr<IUE> pUE, double dFileSize, double dReadingTimeMean)
		:	Service( pUE ), 
			m_dFileSize( dFileSize * 8 ), 		//To Be Modified��Buffer�е�������BitΪ��λ
			m_dReadingTimeMean( dReadingTimeMean )
    {

		m_dBufferData = m_dFileSize;
		m_ServiceType = FTP;
		/*
		const double mean = 1800000;
		const double sigma = 1800000;
		double Buffertemp = 0.0;
		LOGNORMAL_DOUBLE_GEN LognormalGen = RandomManager::GetRandomManager().GetLognormalDoubleGen(mean, sigma);
		do 
		{
			Buffertemp = LognormalGen();
		} while (Buffertemp > 6000000);
		m_dBufferData = Buffertemp;
		*/
    }

    DLFTPService::~DLFTPService(void)
    {
    }

    void DLFTPService::UpdateThroughput( DATASIZE throughput ) //����������
    {
		if (!GetIsEnd())															//�жϵ�ǰ��buffer�Ƿ��ڿ�״̬�����Ƿ񵽴�reading time
		{
			m_SumIdealThroughput += throughput;
			m_dBufferData -= throughput;
			if ( (static_cast<int>(m_dBufferData) <= 0 ))			//�жϴ˴δ����Ƿ�buffer����
			{
				SetIsEnd(true);													//����bufferΪ��״̬
				m_SumThroughput += (m_dBufferData + throughput);
				m_dBufferData = 0.0;

				m_dPacketEndTime = ClockManager::Instance().GetCurrentTime();		//ͳ�����ݰ��Ĵ���ʱ��
				m_vecPacketTransTime.push_back( m_dPacketEndTime - m_dPacketStartTime );

				UpdateGenerateInterval();
				//������buffer���������ɶ�ʱ��������������
				//FTP���ݸ��µĵ�λΪFrameClock��ʱ�䳤��10ms
				if ( m_pUE.lock()->GetType() == SYSTEM_WIFI )
				{
					Clock & clock = ClockManager::Instance().GetWiFiClock();
					shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(m_pUE.lock());
					shared_ptr<Command> pFTPUpdateCommand(new FTPUpdateCommand(m_pUE.lock() ));
					shared_ptr<TimedCommand> pTimeCommand(new TimedCommand(clock,pCmdEngine,pFTPUpdateCommand) );				
					pTimeCommand->SetTimer( (int)(m_iGenInterval) );
					pCmdEngine->AddCommand(pTimeCommand);
					SimLog::Instance()->LogServiceBufferUpdate( m_pUE.lock()->GetID(), clock.GetClock(), m_iGenInterval, m_pUE.lock()->GetType() );
				}
				else if ( m_pUE.lock()->GetType() == SYSTEM_LAA )
				{
					Clock & clock = ClockManager::Instance().GetLAAClock();
					shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(m_pUE.lock());
					shared_ptr<Command> pFTPUpdateCommand(new FTPUpdateCommand(m_pUE.lock() ));
					shared_ptr<TimedCommand> pTimeCommand(new TimedCommand(clock,pCmdEngine,pFTPUpdateCommand) );				
					pTimeCommand->SetTimer( (int)(m_iGenInterval) );
					pCmdEngine->AddCommand(pTimeCommand);
					SimLog::Instance()->LogServiceBufferUpdate( m_pUE.lock()->GetID(), clock.GetClock(), m_iGenInterval, m_pUE.lock()->GetType() );
				}
				else
				{
					Clock & clock = ClockManager::Instance().GetFrameClock();				
					shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(m_pUE.lock());
					shared_ptr<Command> pFTPUpdateCommand(new FTPUpdateCommand(m_pUE.lock() ));
					shared_ptr<TimedCommand> pTimeCommand(new TimedCommand(clock,pCmdEngine,pFTPUpdateCommand) );				
					pTimeCommand->SetTimer( (int)(m_iGenInterval) );
					pCmdEngine->AddCommand(pTimeCommand);
					SimLog::Instance()->LogServiceBufferUpdate( m_pUE.lock()->GetID(), clock.GetClock(), m_iGenInterval, m_pUE.lock()->GetType() );
				}
			}
			else
			{
				m_SumThroughput += throughput;
			}
		}
		++m_iPacketNum;
    }

    void DLFTPService::UpdateBuffer(unsigned int timespan/*������ʱ����ms*/)
    {
		SetIsEnd(false);

		m_dBufferData = m_dFileSize;

		m_dPacketStartTime = ClockManager::Instance().GetCurrentTime();		//ͳ�����ݰ��Ķ�ȡʱ��
		m_vecPacketReadingTime.push_back( m_dPacketStartTime - m_dPacketEndTime );

		/*
		const double mean = 1800000;
		const double sigma = 1800000;
		LOGNORMAL_DOUBLE_GEN LognormalGen = RandomManager::GetRandomManager().GetLognormalDoubleGen(mean, sigma);
		double Buffertemp = 0.0;
		do 
		{
			Buffertemp = LognormalGen();
		} while (Buffertemp > 6000000);
		m_dBufferData = Buffertemp;
		*/
    }

    void DLFTPService::UpdateDiscardThroughput( DATASIZE throughput )		//���¶������ݰ���������
    {
        m_SumDiscardThroughput += throughput;
		++m_iPacketNum;
		++m_iDiscardPacketNum;
    }

    void DLFTPService::Accept( ServiceStatistic& visit )										//����ͳ�Ʒ��ʽӿ�
    {
        visit.Visit(*this);
    }

	void DLFTPService::UpdateMCSIndex( int mcsindex )									//����MCS�ȼ�
	{
		++m_MCSIndex[mcsindex];
	}

	void DLFTPService::UpdateGenerateInterval( void )
	{
		/*
		const double beta = 2.0;
		EXPONENTIAL_DOUBLE_GEN ExpnentialDoubleGen = RandomManager::GetRandomManager().GetExponenialDoubleGen(beta);
		*/

		const double dBeta = 1.0 / m_dReadingTimeMean;
		EXPONENTIAL_DOUBLE_GEN ExpnentialDoubleGen = RandomManager::GetRandomManager().GetExponenialDoubleGen(dBeta);
		
		if ( m_pUE.lock()->GetType() == SYSTEM_WIFI )
		{
			m_iGenInterval = static_cast<int>(ExpnentialDoubleGen()*1e6/WIFI_STANDARD_US + 1.0);
		}
		else
		{
			//��Frame��ʱ������10msΪ��λ���ڽ���Buffer�����������µ�ʱ����ʹ�õ�FrameClock
			m_iGenInterval = static_cast<int>(ExpnentialDoubleGen()/0.01 + 1.0);
		}

		if (m_iGenInterval < 0.0)
		{
			throw std::runtime_error( "Invalid Buffer Data Generate Interval" );
		}
	}


}

