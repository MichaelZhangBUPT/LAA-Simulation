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
			PacketsSize( PacketSize * 8 )		//Buffer�е�������BitΪ��λ
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

    void StreamService::UpdateThroughput( DATASIZE throughput )						//����������
    {
		if (!GetIsEnd())															//�жϵ�ǰ��buffer�Ƿ��ڿ�״̬�����Ƿ񵽴�reading time
		{
			double tmp = m_dBufferData - throughput;
			if ( (static_cast<int>(tmp) < 0 ))										//�жϴ˴δ����Ƿ�buffer����
			{
				PacketsNum++;
				SetIsEnd(true);														//����bufferΪ��״̬
				
				m_dPacketEndTime = ClockManager::Instance().GetCurrentTime();		//ͳ�����ݰ��Ĵ���ʱ��
				m_vecPacketTransTime.push_back( m_dPacketEndTime - m_dPacketStartTime );

				m_PacketTimePerFarme.push_back( m_dPacketEndTime - m_dPacketStartTime );
				m_PacketSize.push_back(m_dBufferData);								//��ÿ�������������ݴ�

				m_SumThroughputPerFrame = accumulate ( m_PacketSize.begin() , m_PacketSize.end(),0.0);
				m_SumPacketPerFrame = accumulate ( m_PacketTimePerFarme.begin() , m_PacketTimePerFarme.end(),0.0);
				m_SumPacketDelayPerFrame = accumulate( m_PacketDelayPerFarme.begin() , m_PacketDelayPerFarme.end() , 0.0 );
				/* ÿ�θ��������ж��Ƿ񳬳���ʱT=100ms��δ�������������ֱ��PackNum=8 */
				if (  m_SumPacketDelayPerFrame + m_SumPacketPerFrame < VideoFrameLength)
				{
					if ( PacketsNum < PacketsPerVideoFrame )
					{
						/*���붨ʱ�������һ���°�*/
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
						/*����8����������һ��ʱ��ʼ����*/
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
				/* ������ʱT����ǰһ������������һ��ʱ��ʼ���°� */
				else
				{
					m_SumThroughput += (m_SumThroughputPerFrame+m_tempThroughput);
					m_SumDiscardThroughput += m_PacketSize.back();
					m_SumThroughput -= m_PacketSize.back();				//�������һ�����������С������������ȥ��
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

    void StreamService::UpdateBuffer(unsigned int timespan/*������ʱ����ms*/)
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

    void StreamService::UpdateDiscardThroughput( DATASIZE throughput )		//���¶������ݰ���������
    {
    }

    void StreamService::Accept( ServiceStatistic& visit )										//����ͳ�Ʒ��ʽӿ�
    {
        visit.Visit(*this);
    }

	void StreamService::UpdateMCSIndex( int mcsindex )									//����MCS�ȼ�
	{
		++m_MCSIndex[mcsindex];
	}

	void StreamService::UpdateGenerateInterval( void )
	{
		//��LTESubFrame��ʱ������1msΪ��λ���ڽ���Buffer�����������µ�ʱ����ʹ�õ�LTESubFrameClock
		const double low = 2.5;
		const double high = 12.5;
		const double alfa = 1.2;
		UNIFORM_REAL_GEN UniformGen = RandomManager::GetRandomManager().GetUniformRealGen(0, 1);
		double U = UniformGen();
		m_iGenInterval = low / pow((1-U*(1-pow((low/high),alfa))),1.0/alfa);			//���ȷֲ�ת��Ϊ�ضϷֲ�
		m_iGenInterval = static_cast<int>(m_iGenInterval)+1;							
		if (m_iGenInterval < 0.0)
		{
			throw std::runtime_error( "Invalid Buffer Data Generate Interval" );
		}
		m_PacketDelayPerFarme.push_back(m_iGenInterval);
	}
}

