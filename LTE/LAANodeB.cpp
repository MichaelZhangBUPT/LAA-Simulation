#include "commondef.h"
#include "LAANodeB.h"
#include "LAAUE.h"
#include "Command.h"
#include "TimedCommand.h"
#include "ClockManager.h"
#include "Clock.h"
#include "Configuration.h"
#include "RunParameters.h"
#include "Service.h"
#include "LAASINRCalculator.h"
#include "ChannelMaker.h"
#include "LAAChannelRxCommand.h"
#include "ARQ.h"
#include "LAAChannel.h"
#include "SimLog.h"
#include "LAACCA.h"
#include "LAADownLinkScheduler.h"

#include "RandomManager.h"
#include <math.h>

namespace LTESim
{
	LAANodeB::LAANodeB(int id, const POS& pos, double maxtxpower, shared_ptr<Antenna> pAntenna)
		: SystemElement(id, pos), m_dMaxTxPower(maxtxpower), m_pTxAntenna(pAntenna),
		m_bIsChannelAvailable(false),m_ChannelState(Idle),m_iChannelIdleTime(0),
		m_iChannelBusyTime(0),m_iChannelScheduleTime(0),m_iLBTBackoffTime(0),m_iLBTDeferTime(0),m_bFirstTrans(true),m_iSumCollisionNum(0),m_iWaitTime(0),
		m_dCellThroughput(0.0),m_iTransmitDataTime(0),m_bCCA(1)
	{
		m_dDataMaxLength = System::GetCurrentSystem()->GetConfig().GetInt( "WiFi_Data_Max_Length(byte)" ) * 8;
		m_TXOPLimit = System::GetCurrentSystem()->GetConfig().GetInt( "LAA_TXOPLimit(us)" );
		m_iCW = System::GetCurrentSystem()->GetConfig().GetInt( "LAANodeB_CW" );
	}

	LAANodeB::~LAANodeB()
	{

	}

	void LAANodeB::OnClock(const Clock& clock)
	{
		if ( !m_LAAUEList.empty() )
		{
			switch (clock.GetID())
			{
			case LAACLOCK:
				LAAClockProcess(clock);
				break;
			default:
				throw std::invalid_argument("Get Illegal Clock ID ");
				break;
			}
			CMDEngine::Run(clock);
		}
	}

	void LAANodeB::LAAClockProcess(const Clock& LAAclock)
	{
		GetCCAResult();
		switch(m_ChannelState)
		{
		case Idle:
			ChannelIdleProcess();
			break;
		case Backoff:
			ChannelBackoffProcess();
			break;
		case Busy:
			ChannelBusyProcess();
			break;
		case Collision:
			ChannelCollisionProcess();
			break;
		default:
			throw std::invalid_argument("Get Illegal LAA Channel State");
			break;
		}
	}


	void LAANodeB::ChannelIdleProcess()
	{
		if ( m_iChannelIdleTime >= LAA_ICCA_US )
		{

			if ( m_bFirstTrans && !m_LAAUEList.empty() )
			{
				GetFirstTransBackoffTime();
				if(m_iChannelIdleTime >= LAA_ICCA_US + m_iLBTBackoffTime )
				{
					GetCandidateUEList();
					m_bFirstTrans = false;
				}
				else
				{
					if (m_bCCA)//空闲则累积DIFS
					{
						m_iChannelIdleTime += LAA_ECCA_SLOT_US;
					} 
					else
					{
						UpdateLBTBackoffTime();//HFR
						ChannelBackoffProcess();//HFR是否注释？
						m_iChannelIdleTime = 0;
						m_ChannelState = Backoff;
					}
				}
			}
			else
				GetCandidateUEList();

			if ( m_CandidateUEList.size() == 0 ) 
			{
				m_ChannelState = Idle;

			}
			if ( m_CandidateUEList.size() >= 1 ) 
			{
				m_ChannelState = Busy;
				m_iChannelIdleTime = 0;
				ChannelBusyProcess();
			}
		}
		else 
		{
			if (m_bCCA)//空闲则累积DIFS
			{
				m_iChannelIdleTime += LAA_ECCA_SLOT_US;
			} 
			else
			{
				m_ChannelState = Backoff;
				m_iChannelIdleTime = 0;
				UpdateLBTBackoffTime();//HFR
				ChannelBackoffProcess();//HFR
			}
		}

	}

	void LAANodeB::ChannelBackoffProcess()
	{
		if ( m_iLBTDeferTime >= LAA_ECCA_US )
		{
			if ( m_iLBTBackoffTime <= 0 )
			{
				if ( m_bFirstTrans && !m_LAAUEList.empty() )
				{
					GetCandidateUEList();
					m_bFirstTrans = false;
				}
				else
					GetCandidateUEList();

				m_bIsChannelAvailable = false;

				if ( m_CandidateUEList.size() == 0 ) 
				{
					m_ChannelState = Idle;
					ChannelIdleProcess();//hfr324
					//SimLog::Instance()->LogWiFi( ClockManager::Instance().GetWiFiClock().GetClock(), m_ChannelState );
				}
				if ( m_CandidateUEList.size() >= 1 ) 
				{
					m_ChannelState = Busy;
					m_iChannelIdleTime = 0;
					m_iLBTDeferTime = 0;
					ChannelBusyProcess();
				}
			} 
			else
			{
				LBTBackoffTimeTick();
			}
		} 
		else 
		{
			LBTDeferTimeTick();
		}
	}

	void LAANodeB::ChannelBusyProcess()
	{  

		if ( ! m_bIsChannelAvailable )
		{	
			InitChannel();
			m_iChannelBusyTime += m_iChannelScheduleTime;
		}

		if ( m_iChannelBusyTime >= m_TXOPLimit*4 )
		{
			GetCandidateUEList();
			if (m_CandidateUEList.empty())
			{
				m_ChannelState = Idle;
				ChannelIdleProcess();
			} 
			else
			{
				m_ChannelState = Backoff;
				UpdateLBTBackoffTime();
				ChannelBackoffProcess();
			}
			
		}
		else
		{

			if(m_iChannelScheduleTime<=0)
			{
				GetCandidateUEList();//重新调度
				m_bIsChannelAvailable = false;
				InitChannel();
				m_iChannelBusyTime += m_iChannelScheduleTime;//323同上
			}
			else
			{
				m_iChannelScheduleTime -= LAA_ECCA_SLOT_US;
			}
		}

	}

	void LAANodeB::ChannelCollisionProcess()
	{
		//SimLog::Instance()->LogWiFi( ClockManager::Instance().GetWiFiClock().GetClock(), m_ChannelState );
		{
			LAAUELIST::const_iterator mapIter = m_CandidateUEList.begin();
			for ( ; mapIter != m_CandidateUEList.end(); ++mapIter )
			{
				shared_ptr<ILAAUE> pUE = mapIter -> second.lock();
				//pUE -> UpdateCollisionNum();
				//pUE -> UpdateBackoffTime();

			}
			m_ChannelState = Idle;
		}
	}

	void LAANodeB::GetCandidateUEList() 
	{
		//LAAUELIST LAAUEBackoffTimeList;
		m_CandidateUEList.clear();
		LAADownLinkScheduler laaDownLinkScheduler;
		shared_ptr<ILAAUE> pUE =laaDownLinkScheduler.ScheduleProcess(shared_from_this());
		if (pUE)
		{
			m_CandidateUEList.insert(std::make_pair(pUE->GetID(), pUE));
		}
		else
		{
			return;
		}
	}


	void LAANodeB::InitChannel()
	{
		//MAKE_SURE( m_CandidateUEList.size() == 1, "WiFiUE::ChannelBusyProcess()" );
		ILAAUE::PTR pUE = m_CandidateUEList.begin() -> second.lock();
		shared_ptr<Service> pService = pUE -> GetService( SYSTEM_LAA );

		pService -> SetIsScheduled(true);
		MCS mcs( 0, SYSTEM_LAA );		
		ChannelMaker maker;  //需要改
		int TxAntennaNumber = GetTxAntenna() -> GetAntennaPortNum();
		//生成信道时，发射功率需要除以2吗？？？
		shared_ptr<LAAChannel> pChannel = maker.CreateLAADLSCH( pUE, mcs, GetMaxTxPower() / TxAntennaNumber );//有问题，和LTE不一致

		shared_ptr<ARQ> pARQ = pUE -> GetARQ();
		if ( pARQ->GetNewPacketIndicator() )//有新数据需要传
		{
			LAASINRCalculator * pSINRCalc = LAASINRCalculator::Instance();
			double dSINR = pSINRCalc -> CalculateSINROfChannel(pChannel);//没有计算cell间interference，wifi也要改
			double q = RealTodB(dSINR);
			mcs = pUE -> GetLAAAMCEntity() -> SelectMCS( DB( RealTodB(dSINR) ) );
		} 
		else
		{
			mcs = pARQ -> GetMCS();
		}
		//MCS等级的获得也有问题

		pARQ->SetService(pService);
		pARQ->UpdateTransStatus(false);
		pARQ -> SetMCS( mcs );	
		pChannel -> SetMCS( mcs );	//更新MCS等级

		ServiceType stype = pService -> GetServiceType();//得到数据
		double dServiceData = pService->GetData();
		//dServiceData = std::min( dServiceData, 10e10 );	//还有数据大小的限制？？
		int iTdata_us = ceil( dServiceData / mcs.GetLAARate() );//传数据占用的时间，向上取整，单位us

		if ( stype == VoIP )
		{
			m_iTransmitDataTime += iTdata_us;//传输总时间
			iTdata_us += (34+40)*8 / mcs.GetLAARate() + 20;	//加入物理层开销----这里不用走
			pChannel -> SetData( dServiceData );	//传输的数据
		} 
		else
		{
			iTdata_us = std::min( iTdata_us + LAA_TX_ConfigTime , m_TXOPLimit);	//一次传输最大信道占用时间限制
			m_iTransmitDataTime += iTdata_us;
			pChannel -> SetData( iTdata_us*mcs.GetLAARate() );	//传输的数据
		}
		m_ChannelState = Busy;
		m_bIsChannelAvailable = true;//信道初始化完成

		//if ( m_CandidateUEList.size() == 1 )
		//{
			m_iChannelScheduleTime = iTdata_us ;//配置要改
			TransmitChannel( pChannel, ceil(double(m_iChannelBusyTime)/WIFI_STANDARD_US) );
		//} 
		//else
		//{
		//	m_iChannelBusyTime = iTdata_us + WIFI_ACK_TIMEOUT_US;
		//}

		////设定本次传输时间点，存储传输间隔
		//pService -> SetThisTransTime( ClockManager::Instance().GetWiFiClock().GetClock() * WIFI_STANDARD_US );
		//pService -> UpdateTransInterval();

		//std::cout<<endl<<"UEID:"<<pUE->GetID()<<" ChannelTime:"<<m_iChannelBusyTime<<" MCS:"<<mcs.ToIndex()<<" WiFiTime:"<<ClockManager::Instance().GetWiFiClock().GetClock();
	}

	void LAANodeB::TransmitChannel( const shared_ptr<LAAChannel> pChannel, int iTimeDelay )
	{
		ClockManager& clockManager = ClockManager::Instance();
		Clock&	refLAAClock = clockManager.GetLAAClock();
		Command::PTR pCommand( new LAAChannelRxCommand( pChannel ) );
		ILAAUE::PTR pUE = pChannel -> GetUE();
		CMDEngine::PTR pCmdEngine = dynamic_pointer_cast<CMDEngine>( pUE );
		TimedCommand::PTR pTimedCommand( new TimedCommand( refLAAClock, pCmdEngine, pCommand ) );
		pTimedCommand->SetTimer( iTimeDelay );
		pCmdEngine->AddCommand( pTimedCommand );

	}

	void LAANodeB::AddUE( const shared_ptr<ILAAUE> pUE )
	{
		m_LAAUEList.insert( std::make_pair( pUE->GetID(), pUE ) );
	}

	shared_ptr<Antenna> LAANodeB::GetTxAntenna() const
	{
		return m_pTxAntenna;
	}

	double LAANodeB::GetMaxTxPower() const
	{
		return m_dMaxTxPower;
	}

	const LAANodeB::LAAUELIST& LAANodeB::GetUEList() const
	{
		return m_LAAUEList;
	}

	const int LAANodeB::GetSumCollisionNum() const
	{
		return m_iSumCollisionNum;
	}

	void LAANodeB::UpdateCellThroughput( double thr )
	{
		m_dCellThroughput += thr;
	}

	double LAANodeB::GetCellThroughput()
	{
		return m_dCellThroughput;
	}

	int LAANodeB::GetTransmitDataTime()
	{
		return m_iTransmitDataTime;
	}

	const int LAANodeB::GetLBTBackoffTime() const
	{
		return m_iLBTBackoffTime;
	}

	void LAANodeB::LBTBackoffTimeTick()
	{
		//m_iLBTBackoffTime -= WIFI_STANDARD_US;
		if (m_bCCA)
		{
			m_iLBTBackoffTime -= LAA_ECCA_SLOT_US;
		}
		else 
		{
			m_iLBTDeferTime = 0;
			//LBTDeferTimeTick();
		}

	}

	void LAANodeB::UpdateLBTBackoffTime()
	{
		UNIFORM_INT_GEN BackoffTimeGen = RandomManager::GetRandomManager().GetUniformIntGen(0,m_iCW);
		m_iLBTBackoffTime = BackoffTimeGen() * LAA_ECCA_SLOT_US;
	}
	void LAANodeB::LBTDeferTimeTick()
	{
		if (m_bCCA)
		{
			m_iLBTDeferTime += LAA_ECCA_SLOT_US;
		}
		else
		{
			m_iLBTDeferTime = 0;
		}
	}

	const int LAANodeB::GetLBTDeferTime()
	{
		return m_iLBTDeferTime;
	}

	ChannelState LAANodeB::GetChannelState()
	{
		return m_ChannelState;
	}

	void LAANodeB::GetCCAResult()
	{
		LAACCACalculator * pCCACalc = LAACCACalculator::Instance();
		m_bCCA = pCCACalc -> CalculateCCA(shared_from_this());
	}
	void LAANodeB::GetFirstTransBackoffTime()
	{
		if (m_iLBTBackoffTime <= 0)
		{
			UpdateLBTBackoffTime();
		} 
		else
		{
			m_iLBTBackoffTime = 0;
		}
	}
}