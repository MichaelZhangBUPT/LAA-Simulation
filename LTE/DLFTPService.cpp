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
			m_dFileSize( dFileSize * 8 ), 		//To Be Modified，Buffer中的数据以Bit为单位
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

    void DLFTPService::UpdateThroughput( DATASIZE throughput ) //更新吞吐量
    {
		if (!GetIsEnd())															//判断当前的buffer是否处于空状态，即是否到达reading time
		{
			m_SumIdealThroughput += throughput;
			m_dBufferData -= throughput;
			if ( (static_cast<int>(m_dBufferData) <= 0 ))			//判断此次传输是否将buffer传完
			{
				SetIsEnd(true);													//设置buffer为空状态
				m_SumThroughput += (m_dBufferData + throughput);
				m_dBufferData = 0.0;

				m_dPacketEndTime = ClockManager::Instance().GetCurrentTime();		//统计数据包的传输时间
				m_vecPacketTransTime.push_back( m_dPacketEndTime - m_dPacketStartTime );

				UpdateGenerateInterval();
				//将更新buffer的命令生成定时命令，加入命令队列
				//FTP数据更新的单位为FrameClock的时间长度10ms
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

    void DLFTPService::UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/)
    {
		SetIsEnd(false);

		m_dBufferData = m_dFileSize;

		m_dPacketStartTime = ClockManager::Instance().GetCurrentTime();		//统计数据包的读取时间
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

    void DLFTPService::UpdateDiscardThroughput( DATASIZE throughput )		//更新丢弃数据包的吞吐量
    {
        m_SumDiscardThroughput += throughput;
		++m_iPacketNum;
		++m_iDiscardPacketNum;
    }

    void DLFTPService::Accept( ServiceStatistic& visit )										//数据统计访问接口
    {
        visit.Visit(*this);
    }

	void DLFTPService::UpdateMCSIndex( int mcsindex )									//更新MCS等级
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
			//以Frame的时间周期10ms为单位，在进行Buffer中数据量更新的时候都是使用的FrameClock
			m_iGenInterval = static_cast<int>(ExpnentialDoubleGen()/0.01 + 1.0);
		}

		if (m_iGenInterval < 0.0)
		{
			throw std::runtime_error( "Invalid Buffer Data Generate Interval" );
		}
	}


}

