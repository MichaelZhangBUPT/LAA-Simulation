#include "commondef.h"
#include "LAAUE.h"
#include "System.h"
#include "LAANodeB.h"
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
#include "LAASINRCalculator.h"
#include "LAAChannel.h"
#include "SimLog.h"
//#include <stdio.h>

namespace LTESim
{
	LAAUE::LAAUE(int ueid, const POS& pos, shared_ptr<Antenna> pRxAntenna)
		: SystemElement(ueid, pos),
		m_pRxAntenna(pRxAntenna)//,m_iCollisionNum(0),m_iCW(0)
	{

	}

	LAAUE::~LAAUE(void)
	{

	}

	const POS& LAAUE::GetPos() const
	{
		return SystemElement::GetPos();
	}

	int LAAUE::GetID() const
	{
		return SystemElement::GetID();
	}

	shared_ptr<System> LAAUE::Attach( shared_ptr<System> pNewSys )
	{
		return SystemElement::Attach( pNewSys );
	}

	void LAAUE::OnClock(const Clock& clock)
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


	void LAAUE::Initialization()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_pARQ = shared_ptr<ARQ>( new ARQ( m_pService ) );
		m_pAMCEntity = shared_ptr<AMCEntity>(new AMCEntity(pSystem->GetLAAAMCTable(), 
			DB(pSystem->GetConfig().GetDouble("LAA_AMC_UPSTEP_DB")),
			DB(pSystem->GetConfig().GetDouble("LAA_AMC_DOWNSTEP_DB"))));
		m_dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("LAA_UE_Noise_Figure(dB)"));
		//UpdateBackoffTime();
	}

	bool LAAUE::AddService(shared_ptr<Service> pService)
	{
		m_pService = pService;
		return true;
	}

	shared_ptr<Service> LAAUE::GetService( SystemType systemType )
	{
		if ( systemType == SYSTEM_LAA )
			return m_pService;
		else
			throw std::invalid_argument( "Invalid System Type in LAA UE" );
	}


	void LAAUE::LAAClockProcess(const Clock& laaclock)
	{
		//在每一时隙的开始进行检测
		//如果未接入laa，则生成laa小区接入命令放置到命令列表中
		if( !m_pServingNB.lock() )
		{
			Command::PTR pCmd( new LAACellSearchCommand( shared_from_this() ) );
			AddCommand( pCmd );
		}
	}

	void LAAUE::ReceiveChannel( const shared_ptr<LAAChannel> pChannel )
	{
		MakeDecision( pChannel );
	}

	void LAAUE::MakeDecision( const shared_ptr<LAAChannel> pChannel )
	{
		const Decisioner& refDecisioner = m_pSystem.lock()->GetDecisioner(SYSTEM_LAA);
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
			std::cout<<endl<<"LAA UE Make Decision Error"<<endl;
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
		//m_iCollisionNum = 0;
		//m_iCW = 16;
		//UpdateBackoffTime();
		//设定上一次传输时间点
		m_pService->SetLastTransTime( ClockManager::Instance().GetLAAClock().GetClock() * WIFI_STANDARD_US );
		//std::cout<<endl<<ClockManager::Instance().GetWiFiClock().GetClock();
	}

	void LAAUE::UpdateThroughput( const shared_ptr<LAAChannel> pChannel )
	{
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pChannel->GetData();
			if ( pSystem->StatisticEnabled() )
			{
				m_pService->UpdateThroughput( dTableBlock );
				m_pServingNB.lock()->UpdateCellThroughput(dTableBlock);

				if ( m_pService->GetIsEnd() )
					SimLog::Instance()->LogBufferEmpty( m_ID, ClockManager::Instance().GetWiFiClock().GetClock(), GetType(), SYSTEM_LAA );
			}
			m_pService->SetLastPacketsize( dTableBlock );
		}
	}

	//获得接收天线
	shared_ptr<Antenna> LAAUE::GetRxAntenna() const
	{
		return m_pRxAntenna;
	}

	//获得UE噪声系数
	double LAAUE::GetNoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	double LAAUE::GetLAANoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	SystemType LAAUE::GetType() const
	{
		return SYSTEM_LAA;
	}

	/*const int LAAUE::GetBackoffTime() const
	{
		return m_iBackoffTime;
	}

	void LAAUE::BackoffTimeTick()
	{
		m_iBackoffTime -= WIFI_STANDARD_US;
	}

	void LAAUE::UpdateBackoffTime()
	{
		UNIFORM_INT_GEN BackoffTimeGen = RandomManager::GetRandomManager().GetUniformIntGen(0,m_iCW);
		m_iBackoffTime = BackoffTimeGen() * WIFI_SLOT_US;
	}*/

	//void LAAUE::UpdateCollisionNum()
	//{
	//	++m_iCollisionNum;
	//	if ( m_iCollisionNum <= 6 )	//最大退避次数是6
	//	{
	//		m_iCW = 0;
	//		//m_iCW *= 2;
	//	}
	//	if ( m_iCollisionNum == 8 )	//最大重传次数是7，即碰撞8次后丢弃包
	//	{
	//		//丢包，重新生成
	//		m_pService->SetData(0);
	//		m_pService->UpdateThroughput(0);	
	//		m_iCollisionNum = 0;
	//		//m_iCW = 16;
	//		std::cout<<endl<<"LAA UE Retry Limit"<<endl;
	//	}
	//}

	void LAAUE::AddToActiveNB(const shared_ptr<LAANodeB> pNB)
	{
		m_pServingNB = pNB;
	}

	shared_ptr<LAANodeB> LAAUE::GetServingNB() const
	{
		return m_pServingNB.lock();
	}

	shared_ptr<AMCEntity> LAAUE::GetLAAAMCEntity() const
	{
		return m_pAMCEntity;
	}

	shared_ptr<ARQ> LAAUE::GetARQ() const
	{
		return m_pARQ;
	}
}
