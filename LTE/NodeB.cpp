#include "commondef.h"
#include "NodeB.h"
#include "UE.h"
#include "Channel.h"
#include "Scheduler.h"
#include "ChannelRxCommand.h"
#include "TimedCommand.h"
#include "ClockManager.h"
#include "ResManagerImp2.h"
#include "PowerManager.h"
#include "DownlinkScheduler.h"
#include "ServiceStatistic.h"
#include "HighLayerController.h"
#include "HighLayerReport.h"
#include "HighLayerReportUpdateCommand.h"
#include "Configuration.h"
#include "RunParameters.h"

#include <numeric>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/bind.hpp>

namespace LTESim
{

	NodeB::NodeB( int id, const POS& pos, int RBPERSLOT, double maxtxpower, shared_ptr<Antenna> pTxAntenna )
		:	SystemElement(id, pos), 
			m_pResManager(new ResManagerImp2(LTESLOTPERFRAME, RBPERSLOT, pTxAntenna->GetAntennaPortNum())),
			m_pPowerManager(new SimplePowerManager),
			m_RBPERSLOT(RBPERSLOT), 
			m_MAXTxPower(maxtxpower),
			m_pTxAntenna(pTxAntenna),m_dCellThroughput(0.0),m_iUsedRBNum(0),m_bUpdateCSI(false)
	{

	}

	NodeB::~NodeB(void)
	{

	}

	void NodeB::OnClock( const Clock& clock )
	{
		/*根据触发的时钟不同进行处理*/
		switch(clock.GetID())
		{
		case LTESUBFRAMECLOCK:
			SubFrameProcess(clock);
			break;
		case LTESLOTCLOCK:
			SlotProcess(clock);
			break;
		case HSPASUBFRAMECLOCK:
		case HSPASLOTCLOCK:
		case HIGHLAYERCLOCK:
			break;
		default:
			ClockProcessor::OnClock(clock);
			break;
		}
		/*处理命令队列*/
		CMDEngine::Run(clock);
	}

	bool NodeB::RequireAccess( const shared_ptr<ILTEUE>& pUE )
	{
		//现在临时返回true
		AddUE(pUE);
		return true;
	}

	void NodeB::AddUE( const shared_ptr<ILTEUE>& pUE )
	{
		m_uelist.insert(std::make_pair(pUE->GetID(), pUE));	//添加UE到列表
	}

	const NodeB::UELIST& NodeB::GetUEList() const
	{
		return m_uelist;
	}

	void NodeB::FrameProcess( const Clock& clock )
	{
		m_pResManager->ClearBefore( ClockManager::Instance().GetFrameClock().GetClock() - 1 );
		//USER_MESSAGE("NodeB " << GetID() << " is processing Frame " << clock.GetClock());
	}

	void NodeB::SubFrameProcess( const Clock& clock )
	{
		//USER_MESSAGE("NodeB " << GetID() << " is processing SubFrame " << clock.GetClock());
		//下行调度，完成信道的初始化
		m_pFDDLScheduler->Process(m_pTDDLScheduler->Process());
		//在每一个子帧，LTE NodeB向高层进行上报
		UpdateReportToHighLayer();
	}

	void NodeB::SlotProcess( const Clock& clock )
	{
		//USER_MESSAGE("NodeB " << GetID() << " is processing Slot " << clock.GetClock());
	}

	void NodeB::TxProcess( const Clock& clock )
	{
		//    USER_MESSAGE("NodeB " << GetID() << " is processing TX" << clock.GetClock());
	}

	void NodeB::RxProcess( const Clock& clock )
	{
	  //  USER_MESSAGE("NodeB " << GetID() << " is processing RX" << clock.GetClock());
	}

	shared_ptr<Antenna> NodeB::GetTxAntenna() const
	{
		return m_pTxAntenna;
	}

	double NodeB::GetTXPower( const PhyResource & phyresource ) const
	{
		if(const shared_ptr<const PhyResource>& rb = 
			m_pResManager->FindRes(phyresource.GetFrames(), phyresource.GetSlot(), 
			phyresource.GetAntennaPort(), phyresource.GetRB()))
			return rb->GetTxPower();
		return 0.0;
	}

	void NodeB::TransmittChannel( shared_ptr<Channel> pChannel, int TxTiming )
	{
		//省却一步，直接在相应的接收时刻让UE接收
		if(shared_ptr<ILTEUE> pUE = pChannel->GetActiveUE())
		{
			//要求UE在此对应的RXclock上接收
			Clock& rxclock = ClockManager::Instance().GetRxClock();
			shared_ptr<Command> pRxCommand(new ChannelRxCommand(pUE, pChannel));
			shared_ptr<CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>( pUE );
			shared_ptr<TimedCommand> pTimedRxCommand(new TimedCommand(rxclock , pCmdEngine, pRxCommand));
			pTimedRxCommand->SetTimer(TxTiming);
			pCmdEngine->AddCommand(pTimedRxCommand);
		//	USER_MESSAGE(ClockManager::GetClockManager().GetSysClock().GetClock() << ": NodeB is going to transimmitt Channel " << pChannel  << " after " << TxTiming << "RXCLOCK");
		}
	}

	bool NodeB::CheckResource( const PhyResourceList & ResList ) const	//资源检测
	{
		return m_pResManager->Require(ResList);
	}

	bool NodeB::AllocateResource( const PhyResourceList & ResList )	//资源分配
	{
		return m_pResManager->Assign(ResList);
	}

	bool NodeB::CheckPower( const PhyResourceList & ResList )	//功率检测
	{
		return true;
		throw UnFinishedFunc();
	}

	bool NodeB::AllocatePower( const PhyResourceList & ResList )	//功率分配
	{
		return true;
		throw UnFinishedFunc();
	}

	shared_ptr<TDDownlinkScheduler> NodeB::SetTDDLScheduler( shared_ptr<TDDownlinkScheduler> pTDScheduler )	//设置下行调度
	{
		pTDScheduler.swap(m_pTDDLScheduler);
		return pTDScheduler;
	}

	shared_ptr<FDDownlinkScheduler> NodeB::SetFDDLScheduler( shared_ptr<FDDownlinkScheduler> pFDScheduler )	//设置下行调度
	{
		pFDScheduler.swap(m_pFDDLScheduler);
		return pFDScheduler;
	}

	int NodeB::GetRBPerSlot() const
	{
		return m_RBPERSLOT;
	}

	double NodeB::GetMaxTxPower() const
	{
		return m_MAXTxPower;
	}

	double NodeB::GetMaxTxPowerPerRBPerPort() const
	{
		//用于得到发送天线端口数
		return m_MAXTxPower / GetTxAntenna()->GetAntennaPortNum() / m_RBPERSLOT;
	}

	shared_ptr<ILTEUE> NodeB::GetUEToRB( int RB, int antennaPort ) const
	{
		return m_UEToRBList[RB * (m_pTxAntenna->GetAntennaPortNum() ) + antennaPort];
	}

	void NodeB::UpdateUEForRB( vector <shared_ptr<ILTEUE> >& UEToRBList )
	{
		m_UEToRBList.swap(UEToRBList);
	}

	void NodeB::UpdateReportToHighLayer()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		//默认的是相同的ID号的LTE NB与HighLayerController相对应
		shared_ptr<HighLayerController> pHighLayerController = pSystem->GetAssociateHighLayerController( shared_from_this() );
		
		LTENBReport lteNBReport;
		//生成Report
		for ( UELIST::iterator mapIter = m_uelist.begin(); mapIter != m_uelist.end(); ++mapIter )
		{
			if ( shared_ptr<ILTEUE> pUE = mapIter->second.lock() )
			{
				shared_ptr<Reporter> pReporter = pUE->GetReporter();
				lteNBReport.m_UESINRVecMap.insert( make_pair( pUE, pReporter->GetSINRList() ) );
			}
			//该eNodeB一共有多少RB，分配了多少RB，表示系统的负载
			lteNBReport.m_iTotalRBNum = m_RBPERSLOT;
			int iUsedRBNumCountAntennaPort = 0;
			int iAntennaPortNum = m_pTxAntenna->GetAntennaPortNum();
			for( std::vector<shared_ptr<ILTEUE>>::iterator vecIter = m_UEToRBList.begin(); vecIter != m_UEToRBList.end(); ++vecIter )
			{
				if ( *vecIter )
				 iUsedRBNumCountAntennaPort++;
			}
			lteNBReport.m_iUsedRBNum = iUsedRBNumCountAntennaPort / iAntennaPortNum;
		}

		//反馈的是LTE NodeB向高层反馈的信息，参考的Clock为LTE SubFrame Clock
		Command::PTR pReportUpdateCommand( new HighLayerLTEReportUpdateCommand( pHighLayerController, lteNBReport ) );
		Clock& rClock = ClockManager::Instance().GetLTESubFrameClock();
		TimedCommand::PTR pTimedCommand( new TimedCommand( rClock, pHighLayerController, pReportUpdateCommand ) );
		int iDelayLTENBReport = pSystem->GetConfig().GetInt( "Delay_LTE_NB_Report(ms)" );
		int iSubFrameDelayLTENBReport = iDelayLTENBReport / LTE_SUBFRAME_LENGTH_MS;
		pTimedCommand->SetTimer( iSubFrameDelayLTENBReport );
		pHighLayerController->AddCommand( pTimedCommand );
	}

	void NodeB::SetLastCandidateService( int id, int rbnum )
	{
		m_LastCandidateService.clear();
		m_LastCandidateService.insert( std::make_pair(id,rbnum) );
	}

	map<int,int> NodeB::GetLastCandidateService()
	{
		return m_LastCandidateService;
	}

	void NodeB::UpdateCellThroughput( double thr )
	{
		m_dCellThroughput += thr;
	}

	double NodeB::GetCellThroughput()
	{
		return m_dCellThroughput;
	}

	void NodeB::UpdateUsedRBNum( int num )
	{
		m_iUsedRBNum += num;
	}

	int NodeB::GetUsedRBNum()
	{
		return m_iUsedRBNum;
	}

	void NodeB::SetIsUpdateCSI(bool flag)
	{
		m_bUpdateCSI = flag;
	}

	bool NodeB::GetIsUpdateCSI()
	{
		return m_bUpdateCSI;
	}

}
