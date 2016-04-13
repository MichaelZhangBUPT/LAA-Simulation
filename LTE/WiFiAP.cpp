#include "commondef.h"
#include "WiFiAP.h"
#include "WiFiUE.h"
#include "Command.h"
#include "TimedCommand.h"
#include "ClockManager.h"
#include "Clock.h"
#include "Configuration.h"
#include "RunParameters.h"
#include "Service.h"
#include "WiFiSINRCalculator.h"
#include "ChannelMaker.h"
#include "WiFiChannelRxCommand.h"
#include "ARQ.h"
#include "WiFiChannel.h"
#include "SimLog.h"
#include "WiFiCCA.h"

namespace LTESim
{
	WiFiAP::WiFiAP(int id, const POS& pos, double maxtxpower, shared_ptr<Antenna> pAntenna)
		: SystemElement(id, pos), m_dMaxTxPower(maxtxpower), m_pTxAntenna(pAntenna),
		m_bIsChannelAvailable(false),m_ChannelState(Idle),m_iChannelIdleTime(0),
		m_iChannelBusyTime(0),m_bFirstTrans(true),m_iSumCollisionNum(0),m_iWaitTime(0),
		m_dCellThroughput(0.0),m_iTransmitDataTime(0)
	{
		m_dDataMaxLength = System::GetCurrentSystem()->GetConfig().GetInt( "WiFi_Data_Max_Length(byte)" ) * 8;
		m_TXOPLimit = System::GetCurrentSystem()->GetConfig().GetInt( "WiFi_TXOPLimit(us)" );
	}

	WiFiAP::~WiFiAP()
	{

	}

	void WiFiAP::OnClock(const Clock& clock)
	{
		if ( !m_WiFiUEList.empty() )
		{
			switch (clock.GetID())
			{
			case WIFICLOCK:
				WiFiClockProcess(clock);
				break;
			default:
				throw std::invalid_argument("Get Illegal Clock ID ");
				break;
			}
			CMDEngine::Run(clock);
		}
	}

	void WiFiAP::WiFiClockProcess(const Clock& wificlock)
	{
		
		switch(m_ChannelState)
		{
		case Idle:
			ChannelIdleProcess();
			break;
		case Busy:
			ChannelBusyProcess();
			break;
		case Collision:
			ChannelCollisionProcess();
			break;
		default:
			throw std::invalid_argument("Get Illegal WiFi Channel State");
			break;
		}
		WiFiCCACalculator * pCCACalc = WiFiCCACalculator::Instance();
		bool cs = pCCACalc -> CalculateCCA(shared_from_this());
	}

	void WiFiAP::ChannelIdleProcess()
	{
		if ( m_iChannelIdleTime >= WIFI_DIFS_US )
		{
			//SimLog::Instance()->LogWiFi( ClockManager::Instance().GetWiFiClock().GetClock(), m_ChannelState );
			//std::cout<<endl<<ClockManager::Instance().GetWiFiClock().GetClock()<<"  Ready  APID:"<<GetID();
			if ( m_bFirstTrans && !m_WiFiUEList.empty() )
			{
				m_CandidateUEList.insert( *m_WiFiUEList.begin() );
				m_WiFiUEList.begin()->second.lock()->GetService(SYSTEM_WIFI)->UpdateService();
				m_bFirstTrans = false;
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
		    }

			
		}
		else
		{
			if(cs = 0){m_iChannelIdleTime=WIFI_STANDARD_US;}
			else m_iChannelIdleTime += WIFI_STANDARD_US;
		}
			
	}

	void WiFiAP::ChannelBusyProcess()
	{  
			//MAKE_SURE( m_CandidateUEList.size() == 1, "WiFiUE::ChannelBusyProcess()" );
			if ( !m_bIsChannelAvailable )
			{	
				if(cs=0)//检测到信道忙，停止Backoff，重新累计信道空闲时间。
			    { 
					m_ChannelState = Idle;
				    m_iChannelIdleTime = WIFI_STANDARD_US;	
				    m_bIsChannelAvailable = false;
					ChannelCollisionProcess();
					m_iSumCollisionNum++;
				}
				InitChannel();
			}

			if ( m_iChannelBusyTime <= 0 )
			{
				if ( m_CandidateUEList.size() > 1 )
				{
					ChannelCollisionProcess();
					m_iSumCollisionNum++;
				} 
				m_ChannelState = Idle;
				m_iChannelIdleTime = WIFI_STANDARD_US;	
				m_bIsChannelAvailable = false;
			}
			else
				m_iChannelBusyTime -= WIFI_STANDARD_US;
	}

	void WiFiAP::ChannelCollisionProcess()
	{
		//SimLog::Instance()->LogWiFi( ClockManager::Instance().GetWiFiClock().GetClock(), m_ChannelState );
			WiFiUELIST::const_iterator mapIter = m_CandidateUEList.begin();
			for ( ; mapIter != m_CandidateUEList.end(); ++mapIter )
			{
				shared_ptr<IWiFiUE> pUE = mapIter -> second.lock();
				pUE -> UpdateCollisionNum();
				pUE -> UpdateBackoffTime();

			}
			m_ChannelState = Idle;
	}

	void WiFiAP::GetCandidateUEList() 
	{
		m_CandidateUEList.clear();
		WiFiUELIST::const_iterator mapIter = m_WiFiUEList.begin();
		for ( ; mapIter != m_WiFiUEList.end(); ++mapIter )
		{
			shared_ptr<IWiFiUE> pUE = mapIter -> second.lock();
			if ( pUE )
			{
				shared_ptr<Service> pService = pUE -> GetService( SYSTEM_WIFI );
				if ( pService )
				{
					pService -> UpdateService();
					if( pService -> GetIsEnd() == false )//如果业务没有结束，即Buffer中还有数据
					{
						if(cs=0)//检测到信道忙，停止Backoff，重新累计信道空闲时间。
					    { 
							m_ChannelState = Idle;
				            m_iChannelIdleTime = WIFI_STANDARD_US;	
				            m_bIsChannelAvailable = false;
							//if ( pUE -> GetBackoffTime() <= 0 )//同时计数器减到0（实际中是无法检测到的，AP
							//	                               //发送的数据发生了碰撞），认为此时发生碰撞。
						 //   {
							//	ChannelCollisionProcess();
					  //          m_iSumCollisionNum++;
							//}
				        }
						else
						{
							if ( pUE -> GetBackoffTime() <= 4 )
						    {
								m_CandidateUEList.insert(std::make_pair(pUE->GetID(), pUE));
						    }
							pUE -> BackoffTimeTick();//
						}	
					}
				}
				else
					throw std::runtime_error( "Get WiFi Service Error" );
			}
			else
				throw std::runtime_error("Get WiFi UE Error");
		}

	}

	void WiFiAP::InitChannel()
	{
		//MAKE_SURE( m_CandidateUEList.size() == 1, "WiFiUE::ChannelBusyProcess()" );
		IWiFiUE::PTR pUE = m_CandidateUEList.begin() -> second.lock();
		shared_ptr<Service> pService = pUE -> GetService( SYSTEM_WIFI );

		pService -> SetIsScheduled(true);
		MCS mcs( 0, SYSTEM_WIFI );		
		ChannelMaker maker;
		int TxAntennaNumber = GetTxAntenna() -> GetAntennaPortNum();
		//生成信道时，发射功率需要除以2吗？？？
		shared_ptr<WiFiChannel> pChannel = maker.CreateWiFiDLSCH( pUE, mcs, GetMaxTxPower() / TxAntennaNumber );
		
		shared_ptr<ARQ> pARQ = pUE -> GetARQ();
		if ( pARQ->GetNewPacketIndicator() )
		{
			WiFiSINRCalculator * pSINRCalc = WiFiSINRCalculator::Instance();
			double dSINR = pSINRCalc -> CalculateSINROfChannel(pChannel);
			double q = RealTodB(dSINR);
			mcs = pUE -> GetWiFiAMCEntity() -> SelectMCS( DB( RealTodB(dSINR) ) );
		} 
		else
		{
			mcs = pARQ -> GetMCS();
		}

		pARQ->SetService(pService);
		pARQ->UpdateTransStatus(false);
		pARQ -> SetMCS( mcs );	
		pChannel -> SetMCS( mcs );	

		ServiceType stype = pService -> GetServiceType();
		double dServiceData = pService->GetData();
		dServiceData = std::min( dServiceData, 10e10 );	
		int iTdata_us = ceil( dServiceData / mcs.GetWiFiRate() );//传数据占用的时间，向上取整，单位us

		if ( stype == VoIP )
		{
			m_iTransmitDataTime += iTdata_us;
			iTdata_us += (34+40)*8 / mcs.GetWiFiRate() + 20;	//加入物理层开销
			pChannel -> SetData( dServiceData );	//传输的数据
		} 
		else
		{
			iTdata_us = std::min( iTdata_us, m_TXOPLimit );	//一次传输最大信道占用时间限制
			m_iTransmitDataTime += iTdata_us;
			pChannel -> SetData( iTdata_us*mcs.GetWiFiRate() );	//传输的数据
		}
		m_ChannelState = Busy;
		m_bIsChannelAvailable = true;
	
		if ( m_CandidateUEList.size() == 1 )
		{
			m_iChannelBusyTime = iTdata_us + WIFI_SIFS_US + WIFI_ACK_US;
			TransmitChannel( pChannel, ceil(double(m_iChannelBusyTime)/WIFI_STANDARD_US) );
		} 
		else
		{
			m_iChannelBusyTime = iTdata_us + WIFI_ACK_TIMEOUT_US;
		}

		////设定本次传输时间点，存储传输间隔
		//pService -> SetThisTransTime( ClockManager::Instance().GetWiFiClock().GetClock() * WIFI_STANDARD_US );
		//pService -> UpdateTransInterval();

		//std::cout<<endl<<"UEID:"<<pUE->GetID()<<" ChannelTime:"<<m_iChannelBusyTime<<" MCS:"<<mcs.ToIndex()<<" WiFiTime:"<<ClockManager::Instance().GetWiFiClock().GetClock();
	}

	void WiFiAP::TransmitChannel( const shared_ptr<WiFiChannel> pChannel, int iTimeDelay )
	{
		ClockManager& clockManager = ClockManager::Instance();
		Clock&	refWiFiClock = clockManager.GetWiFiClock();
		Command::PTR pCommand( new WiFiChannelRxCommand( pChannel ) );
		IWiFiUE::PTR pUE = pChannel -> GetUE();
		CMDEngine::PTR pCmdEngine = dynamic_pointer_cast<CMDEngine>( pUE );
		TimedCommand::PTR pTimedCommand( new TimedCommand( refWiFiClock, pCmdEngine, pCommand ) );
		pTimedCommand->SetTimer( iTimeDelay );
		pCmdEngine->AddCommand( pTimedCommand );

	}

	void WiFiAP::AddUE( const shared_ptr<IWiFiUE> pUE )
	{
		m_WiFiUEList.insert( std::make_pair( pUE->GetID(), pUE ) );
	}

	shared_ptr<Antenna> WiFiAP::GetTxAntenna() const
	{
		return m_pTxAntenna;
	}

	double WiFiAP::GetMaxTxPower() const
	{
		return m_dMaxTxPower;
	}

	const WiFiAP::WiFiUELIST& WiFiAP::GetUEList() const
	{
		return m_WiFiUEList;
	}

	const int WiFiAP::GetSumCollisionNum() const
	{
		return m_iSumCollisionNum;
	}

	void WiFiAP::UpdateCellThroughput( double thr )
	{
		m_dCellThroughput += thr;
	}

	double WiFiAP::GetCellThroughput()
	{
		return m_dCellThroughput;
	}

	int WiFiAP::GetTransmitDataTime()
	{
		return m_iTransmitDataTime;
	}

	ChannelState WiFiAP::GetChannelState()
	{
		return m_ChannelState;
	}

}