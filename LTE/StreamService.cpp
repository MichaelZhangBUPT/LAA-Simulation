#include "commondef.h"
#include "StreamService.h"
#include "ServiceStatistic.h"
#include "UE.h"
#include "ClockManager.h"
#include "TimedCommand.h"
#include "FTPUpdateCommand.h"
#include "SimLog.h"
#include <numeric>

#include <limits>
#include <cmath>

namespace LTESim
{
	const int StreamService::PacketsPerVideoFrame = 8;
	const double StreamService::VideoFrameLength = 100.0;
    StreamService::StreamService(shared_ptr<IUE> pUE, double PacketSize)
		:	Service( pUE ), 
			PacketsSize( PacketSize * 8 )		//Buffer中的数据以Bit为单位
    {
		m_dBufferData = PacketsSize;
		PacketsNum = 0;
		m_tempThroughput = 0;

		m_dPacketStartTime = ClockManager::Instance().GetCurrentTime();

		const double low = 40.0;
		const double high = 250.0;
		const double alfa = 1.2;
		UNIFORM_REAL_GEN UniformGen = RandomManager::GetRandomManager().GetUniformRealGen(0, 1);
		double U = UniformGen();
		m_dBufferData = low / pow((1-U*(1-pow((low/high),alfa))),1.0/alfa) * 8;
		m_ServiceType = Stream;

    }

    StreamService::~StreamService(void)
    {
		m_PacketSize.clear();
		m_PacketTimePerFarme.clear();
		m_PacketDelayPerFarme.clear();
    }

    void StreamService::UpdateThroughput( DATASIZE throughput )						//更新吞吐量
    {
		if (!GetIsEnd())															//判断当前的buffer是否处于空状态，即是否到达reading time
		{
			double tmp = m_dBufferData - throughput;
			if ( (static_cast<int>(tmp) < 0 ))										//判断此次传输是否将buffer传完
			{
				PacketsNum++;
				SetIsEnd(true);														//设置buffer为空状态
				
				m_dPacketEndTime = ClockManager::Instance().GetCurrentTime();		//统计数据包的传输时间
				m_vecPacketTransTime.push_back( m_dPacketEndTime - m_dPacketStartTime );

				m_PacketTimePerFarme.push_back( m_dPacketEndTime - m_dPacketStartTime );
				m_PacketSize.push_back(m_dBufferData);								//将每个包放入容器暂存

				m_SumThroughputPerFrame = accumulate ( m_PacketSize.begin() , m_PacketSize.end(),0.0);
				m_SumPacketPerFrame = accumulate ( m_PacketTimePerFarme.begin() , m_PacketTimePerFarme.end(),0.0);
				m_SumPacketDelayPerFrame = accumulate( m_PacketDelayPerFarme.begin() , m_PacketDelayPerFarme.end() , 0.0 );
				/* 每次更新吞吐判断是否超出定时T=100ms，未超出则继续发包直到PackNum=8 */
				if (  m_SumPacketDelayPerFrame + m_SumPacketPerFrame < VideoFrameLength)
				{
					if ( PacketsNum < PacketsPerVideoFrame )
					{
						/*加入定时命令，生成一个新包*/
						Clock & clock = ClockManager::Instance().GetLTESubFrameClock();				
						UpdateGenerateInterval();
						shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(m_pUE.lock());
						shared_ptr<Command> pFTPUpdateCommand(new FTPUpdateCommand(m_pUE.lock() ));
						shared_ptr<TimedCommand> pTimeCommand(new TimedCommand(clock,pCmdEngine,pFTPUpdateCommand) );
						pTimeCommand->SetTimer( (int)(m_iGenInterval) );
						pCmdEngine->AddCommand(pTimeCommand);

					} 
					else if( PacketsNum == PacketsPerVideoFrame )
					{
						/*发满8个包，从下一定时开始更新*/
						//SimLog::Instance()->LogPacketSize( m_pUE.lock()->GetID(), PacketsNum, m_SumThroughputPerFrame+m_tempThroughput, m_SumPacketPerFrame, m_SumPacketDelayPerFrame);
						m_SumThroughput += (m_SumThroughputPerFrame+m_tempThroughput);

						Clock & clock = ClockManager::Instance().GetLTESubFrameClock();	
						double NextSendTime = (static_cast<int>(m_dPacketEndTime / VideoFrameLength )+1)*100.0;
						m_iGenInterval = ( NextSendTime - m_dPacketEndTime) / LTE_SUBFRAME_LENGTH_MS + 1.0;
						shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(m_pUE.lock());
						shared_ptr<Command> pFTPUpdateCommand(new FTPUpdateCommand(m_pUE.lock() ));
						shared_ptr<TimedCommand> pTimeCommand(new TimedCommand(clock,pCmdEngine,pFTPUpdateCommand) );
						pTimeCommand->SetTimer( (int)(m_iGenInterval) );
						pCmdEngine->AddCommand(pTimeCommand);

						PacketsNum = 0;
						m_tempThroughput = 0;

						m_PacketSize.clear();
						m_PacketTimePerFarme.clear();
						m_PacketDelayPerFarme.clear();
					}

				} 
				/* 超出定时T则丢弃前一个包，并从下一定时开始更新包 */
				else
				{
					m_SumThroughput += (m_SumThroughputPerFrame+m_tempThroughput);
					m_SumDiscardThroughput += m_PacketSize.back();
					m_SumThroughput -= m_PacketSize.back();				//丢弃最后一个包，将其大小从总吞吐量中去除
					//SimLog::Instance()->LogPacketSize( m_pUE.lock()->GetID(), PacketsNum, m_SumThroughputPerFrame+m_tempThroughput, m_SumPacketPerFrame, m_SumPacketDelayPerFrame);

					Clock & clock = ClockManager::Instance().GetLTESubFrameClock();	
					double NextSendTime = (static_cast<int>(m_dPacketEndTime / VideoFrameLength )+1)*100.0;
					m_iGenInterval = ( NextSendTime - m_dPacketEndTime) / LTE_SUBFRAME_LENGTH_MS + 1.0;
					shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(m_pUE.lock());
					shared_ptr<Command> pFTPUpdateCommand(new FTPUpdateCommand(m_pUE.lock() ));
					shared_ptr<TimedCommand> pTimeCommand(new TimedCommand(clock,pCmdEngine,pFTPUpdateCommand) );
					pTimeCommand->SetTimer( (int)(m_iGenInterval) );
					pCmdEngine->AddCommand(pTimeCommand);

					PacketsNum = 0;
					m_tempThroughput = 0;

					m_PacketSize.clear();
					m_PacketTimePerFarme.clear();
					m_PacketDelayPerFarme.clear();
				}

			}

			else
			{
				m_dBufferData -= throughput;
				m_tempThroughput += throughput;
			}

		}

    }

    void StreamService::UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/)
    {
		SetIsEnd(false);
		m_dPacketStartTime = ClockManager::Instance().GetCurrentTime();

		const double low = 40.0;
		const double high = 250.0;
		const double alfa = 1.2;
		UNIFORM_REAL_GEN UniformGen = RandomManager::GetRandomManager().GetUniformRealGen(0, 1);
		double U = UniformGen();
		m_dBufferData = low / pow((1-U*(1-pow((low/high),alfa))),1.0/alfa) * 8;
    }

    void StreamService::UpdateDiscardThroughput( DATASIZE throughput )		//更新丢弃数据包的吞吐量
    {
    }

    void StreamService::Accept( ServiceStatistic& visit )										//数据统计访问接口
    {
        visit.Visit(*this);
    }

	void StreamService::UpdateMCSIndex( int mcsindex )									//更新MCS等级
	{
		++m_MCSIndex[mcsindex];
	}

	void StreamService::UpdateGenerateInterval( void )
	{
		//以LTESubFrame的时间周期1ms为单位，在进行Buffer中数据量更新的时候都是使用的LTESubFrameClock
		const double low = 2.5;
		const double high = 12.5;
		const double alfa = 1.2;
		UNIFORM_REAL_GEN UniformGen = RandomManager::GetRandomManager().GetUniformRealGen(0, 1);
		double U = UniformGen();
		m_iGenInterval = low / pow((1-U*(1-pow((low/high),alfa))),1.0/alfa);			//均匀分布转化为截断分布
		m_iGenInterval = static_cast<int>(m_iGenInterval)+1;							
		if (m_iGenInterval < 0.0)
		{
			throw std::runtime_error( "Invalid Buffer Data Generate Interval" );
		}
		m_PacketDelayPerFarme.push_back(m_iGenInterval);
	}
}

