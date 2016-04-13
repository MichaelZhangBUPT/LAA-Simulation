#include "HSPANodeB.h"
#include "commondef.h"
#include "HSPAUE.h"
#include "HSPATDScheduler.h"
#include "HSPAFDScheduler.h"
#include "Command.h"
#include "TimedCommand.h"
#include "HSPAChannelRxCommand.h"
#include "ClockManager.h"
#include "Clock.h"
#include "HighLayerController.h"
#include "HighLayerReport.h"
#include "HighLayerReportUpdateCommand.h"
#include "Configuration.h"
#include "RunParameters.h"


namespace LTESim
{
	HSPANodeB::HSPANodeB( int id, const POS& pos, double dMaxTxPower, shared_ptr<Antenna> pAntenna )
		: SystemElement( id, pos ), m_dMaxTxPower( dMaxTxPower), m_pAntenna( pAntenna )
	{
		
	}

	HSPANodeB::~HSPANodeB()
	{

	}

	void HSPANodeB::OnClock(const Clock& clock)
	{
		switch (clock.GetID())
		{
		case HSPASUBFRAMECLOCK:
			SubFrameProcess(clock);
			break;
		case HSPASLOTCLOCK:
			SlotProcess(clock);
			break;
		case LTESUBFRAMECLOCK:
		case LTESLOTCLOCK:
		case HIGHLAYERCLOCK:
			break;
		default:
			ClockProcessor::OnClock( clock );
			break;
		}

		CMDEngine::Run( clock );
	}

	void HSPANodeB::FrameProcess( const Clock& FrameClock )
	{

	}

	void HSPANodeB::SubFrameProcess( const Clock& SubFrameClock )
	{
		//HSPA NodeB在每一个子帧开始完成调度
		m_pFDScheduler->Process( m_pTDScheduler->Process() );
		//在每一个子帧，HSPANodeB向高层控制器进行上报
		UpdateReportToHighLayer();
	}

	void HSPANodeB::SlotProcess( const Clock& SlotClock )
	{
		//每一个时隙接收一次
		//在每一个子帧的最后一个时隙完成判决
		//现在只考虑下行，所以HSPANodeB只需要发送信道，不用进行接收
	}

	void HSPANodeB::TxProcess( const Clock& txclock )
	{
		
	}

	void HSPANodeB::RxProcess( const Clock& rxclock )
	{
		
	}

	void HSPANodeB::AddUE( const shared_ptr<IHSPAUE> pUE )
	{
		m_UEList.insert( std::make_pair( pUE->GetID(), pUE ) );
	}

	void HSPANodeB::SetTDDownlinkScheduler( const shared_ptr<ITDScheduler> pScheduler )
	{
		m_pTDScheduler = pScheduler;
	}

	void HSPANodeB::SetFDDownlinkScheduler( const shared_ptr<IFDScheduler> pScheduler )
	{
		m_pFDScheduler = pScheduler;
	}

	const HSPANodeB::UELIST& HSPANodeB::GetUEList() const
	{
		return m_UEList;
	}

	void HSPANodeB::TransmitChannel( const shared_ptr<HSPAChannel> pChannel, int iTimeDelay )
	{
		ClockManager& clockManager = ClockManager::Instance();
		//接收操作的参考时钟为HSPASlot时钟，iTimeDelay也是以HSPASlot为单位的
		Clock&	refHSPASlotClock = clockManager.GetHSPASlotClock();
		Command::PTR pCommand( new HSPAChannelRxCommand( pChannel ) );
		IHSPAUE::PTR pUE = pChannel->GetUE();
		CMDEngine::PTR pCmdEngine = dynamic_pointer_cast<CMDEngine>( pUE );
		TimedCommand::PTR pTimedCommand( new TimedCommand( refHSPASlotClock, pCmdEngine, pCommand ) );
		pTimedCommand->SetTimer( iTimeDelay );
		pCmdEngine->AddCommand( pTimedCommand );
	}

	double HSPANodeB::GetTransmitPower() const
	{
		//存储的是实数类型的最大发射功率
		return m_dMaxTxPower;
	}

	shared_ptr<Antenna> HSPANodeB::GetAntenna() const
	{
		return m_pAntenna;
	}

	vector<shared_ptr<IHSPAUE>> HSPANodeB::GetScheduledUE()
	{
		std::vector<shared_ptr<IHSPAUE>> scheduledUEVec;
		for ( UELIST::const_iterator listIter = m_UEList.begin(); listIter != m_UEList.end(); ++listIter )
		{
			shared_ptr<IHSPAUE> pUE = listIter->second.lock();
			//将当前时刻调度上的UE返回
			shared_ptr<Service> pService = pUE->GetService( SYSTEM_HSPA );
			if ( pService->GetIsScheduled() )
				scheduledUEVec.push_back( pUE );
		}
		return scheduledUEVec;
	}

	void HSPANodeB::UpdateReportToHighLayer()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		//默认的是相同的ID号的LTE NB与HighLayerController相对应
		shared_ptr<HighLayerController> pHighLayerController = pSystem->GetAssociateHighLayerController( shared_from_this() );

		//生成Report，将接入到该HSPANodeB的HSPAUE或者DualModeUE中的C2I信息进行记录，然后反馈到高层控制器
		HSPANBReport hspaNBReport;
		for ( UELIST::iterator mapIter = m_UEList.begin(); mapIter != m_UEList.end(); ++mapIter )
		{
			if ( shared_ptr<IHSPAUE> pUE = mapIter->second.lock() )
			{
				shared_ptr<IHSPAReporter> pReporter = pUE->GetHSPAReporter();
				hspaNBReport.m_UEC2IMap.insert( std::make_pair( pUE, pReporter->GetC2I() ) );
			}
			else
			{
				throw std::runtime_error( "Get IHSPAUE Error" );
			}
		}

		//Report的上报更新，延时以HSPA SubFrame为单位
		Command::PTR pReportUpdateCommand( new HighLayerHSPAReportUpdateCommand( pHighLayerController, hspaNBReport ) );
		Clock& rClock = ClockManager::Instance().GetHSPASubFrameClock();
		TimedCommand::PTR pTimedCommand( new TimedCommand( rClock, pHighLayerController, pReportUpdateCommand ) );
		int iDelayHSPANBReport = pSystem->GetConfig().GetInt( "Delay_HSPA_NB_Report(ms)" );
		int iSubFrameDelayHSPANBReport = iDelayHSPANBReport / HSPA_SUBFRAME_LENGTH_MS;
		pTimedCommand->SetTimer( iSubFrameDelayHSPANBReport );
		pHighLayerController->AddCommand( pTimedCommand );
	}

}