#include "commondef.h"
#include "WiFiUE.h"
#include "System.h"
#include "WiFiAP.h"
#include "ClockManager.h"
#include "Channel.h"
#include "Service.h"
#include "DLFTPService.h"
#include "TimedCommand.h"
#include "RunParameters.h"
#include "CellSearchCommand.h"
#include "RandomManager.h"
#include <math.h>
#include "Decisioner.h"
#include "WiFiSINRCalculator.h"
#include "WiFiChannel.h"
#include "SimLog.h"
//#include <stdio.h>

namespace LTESim
{
	WiFiUE::WiFiUE(int ueid, const POS& pos, shared_ptr<Antenna> pRxAntenna)
		: SystemElement(ueid, pos),
		m_pRxAntenna(pRxAntenna),m_iCollisionNum(0),m_iCW(16)
	{

	}

	WiFiUE::~WiFiUE(void)
	{

	}

	const POS& WiFiUE::GetPos() const
	{
		return SystemElement::GetPos();
	}

	int WiFiUE::GetID() const
	{
		return SystemElement::GetID();
	}

	shared_ptr<System> WiFiUE::Attach( shared_ptr<System> pNewSys )
	{
		return SystemElement::Attach( pNewSys );
	}

	void WiFiUE::OnClock(const Clock& clock)
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


	void WiFiUE::Initialization()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_pARQ = shared_ptr<ARQ>( new ARQ( m_pService ) );
		m_pAMCEntity = shared_ptr<AMCEntity>(new AMCEntity(pSystem->GetWiFiAMCTable(), 
			DB(pSystem->GetConfig().GetDouble("WiFi_AMC_UPSTEP_DB")),
			DB(pSystem->GetConfig().GetDouble("WiFi_AMC_DOWNSTEP_DB"))));
		m_dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("WiFi_UE_Noise_Figure(dB)"));
		UpdateBackoffTime();
	}

	bool WiFiUE::AddService(shared_ptr<Service> pService)
	{
		m_pService = pService;
		return true;
	}

	shared_ptr<Service> WiFiUE::GetService( SystemType systemType )
	{
		if ( systemType == SYSTEM_WIFI )
			return m_pService;
		else
			throw std::invalid_argument( "Invalid System Type in WiFi UE" );
	}


	void WiFiUE::WiFiClockProcess(const Clock& wificlock)
	{
		//在每一时隙的开始进行检测
		//如果未接入WiFi AP，则生成WiFi小区接入命令放置到命令列表中
		if( !m_pServingAP.lock() )
		{
			Command::PTR pCmd( new WiFiCellSearchCommand( shared_from_this() ) );
			AddCommand( pCmd );
		}
	}

	void WiFiUE::ReceiveChannel( const shared_ptr<WiFiChannel> pChannel )
	{
		MakeDecision( pChannel );
	}

	void WiFiUE::MakeDecision( const shared_ptr<WiFiChannel> pChannel )
	{
		const Decisioner& refDecisioner = m_pSystem.lock()->GetDecisioner(SYSTEM_WIFI);
		shared_ptr<ARQ> pARQ = pChannel -> GetARQ(); 
		double dSINR = pChannel ->GetSINR();
		bool bIsTransSuccess = refDecisioner.CheckCRC( pChannel->GetMCS(), DB(RealTodB(dSINR)) );
		//更新AMC门限
		m_pAMCEntity->UpdateThreshold( bIsTransSuccess );
		//更新MCS等级
		//UpdateMCSIndex(pRxChannel);
		if (bIsTransSuccess)
		{
			pARQ->Reset();
			//判决成功更新吞吐量
			UpdateThroughput(pChannel);
		}
		else
		{
			std::cout<<endl<<"WiFi UE Make Decision Error"<<endl;
			if( pARQ->MaxTransTime() )
			{
				USER_MESSAGE("达到最大重传次数，中止此次传输");
				pARQ->Reset();
				//UpdateDiscardThroughput(pChannel);//更新丢弃数据大小
			}
			else
			{
				pARQ->UpdateTranTime();																					//更新传输次数
			}
		}
		m_iCollisionNum = 0;
		m_iCW = 16;
		UpdateBackoffTime();
		//设定上一次传输时间点
		m_pService->SetLastTransTime( ClockManager::Instance().GetWiFiClock().GetClock() * WIFI_STANDARD_US );
		//std::cout<<endl<<ClockManager::Instance().GetWiFiClock().GetClock();
	}

	void WiFiUE::UpdateThroughput( const shared_ptr<WiFiChannel> pChannel )
	{
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pChannel->GetData();
			if ( pSystem->StatisticEnabled() )
			{
				m_pService->UpdateThroughput( dTableBlock );
				m_pServingAP.lock()->UpdateCellThroughput(dTableBlock);

				if ( m_pService->GetIsEnd() )
					SimLog::Instance()->LogBufferEmpty( m_ID, ClockManager::Instance().GetWiFiClock().GetClock(), GetType(), SYSTEM_WIFI );
			}
			m_pService->SetLastPacketsize( dTableBlock );
		}
	}

	//获得接收天线
	shared_ptr<Antenna> WiFiUE::GetRxAntenna() const
	{
		return m_pRxAntenna;
	}

	//获得UE噪声系数
	double WiFiUE::GetNoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	double WiFiUE::GetWiFiNoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	SystemType WiFiUE::GetType() const
	{
		return SYSTEM_WIFI;
	}

	const int WiFiUE::GetBackoffTime() const
	{
		return m_iBackoffTime;
	}

	void WiFiUE::BackoffTimeTick()
	{
		m_iBackoffTime -= WIFI_STANDARD_US;
	}

	void WiFiUE::UpdateBackoffTime()
	{
		UNIFORM_INT_GEN BackoffTimeGen = RandomManager::GetRandomManager().GetUniformIntGen(0,m_iCW);
		m_iBackoffTime = BackoffTimeGen() * WIFI_SLOT_US;
	}

	void WiFiUE::UpdateCollisionNum()
	{
		++m_iCollisionNum;
		if ( m_iCollisionNum <= 6 )	//最大退避次数是6
		{
			m_iCW *= 2;
		}
		if ( m_iCollisionNum == 8 )	//最大重传次数是7，即碰撞8次后丢弃包
		{
			//丢包，重新生成
			m_pService->SetData(0);
			m_pService->UpdateThroughput(0);	
			m_iCollisionNum = 0;
			m_iCW = 16;
			std::cout<<endl<<"WiFi UE Retry Limit"<<endl;
		}
	}

	void WiFiUE::AddToActiveAP(const shared_ptr<WiFiAP> pAP)
	{
		m_pServingAP = pAP;
	}

	shared_ptr<WiFiAP> WiFiUE::GetServingAP() const
	{
		return m_pServingAP.lock();
	}

	shared_ptr<AMCEntity> WiFiUE::GetWiFiAMCEntity() const
	{
		return m_pAMCEntity;
	}

	shared_ptr<ARQ> WiFiUE::GetARQ() const
	{
		return m_pARQ;
	}
}
