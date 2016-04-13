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
		/*���ݴ�����ʱ�Ӳ�ͬ���д���*/
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
		/*�����������*/
		CMDEngine::Run(clock);
	}

	bool NodeB::RequireAccess( const shared_ptr<ILTEUE>& pUE )
	{
		//������ʱ����true
		AddUE(pUE);
		return true;
	}

	void NodeB::AddUE( const shared_ptr<ILTEUE>& pUE )
	{
		m_uelist.insert(std::make_pair(pUE->GetID(), pUE));	//���UE���б�
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
		//���е��ȣ�����ŵ��ĳ�ʼ��
		m_pFDDLScheduler->Process(m_pTDDLScheduler->Process());
		//��ÿһ����֡��LTE NodeB��߲�����ϱ�
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
		//ʡȴһ����ֱ������Ӧ�Ľ���ʱ����UE����
		if(shared_ptr<ILTEUE> pUE = pChannel->GetActiveUE())
		{
			//Ҫ��UE�ڴ˶�Ӧ��RXclock�Ͻ���
			Clock& rxclock = ClockManager::Instance().GetRxClock();
			shared_ptr<Command> pRxCommand(new ChannelRxCommand(pUE, pChannel));
			shared_ptr<CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>( pUE );
			shared_ptr<TimedCommand> pTimedRxCommand(new TimedCommand(rxclock , pCmdEngine, pRxCommand));
			pTimedRxCommand->SetTimer(TxTiming);
			pCmdEngine->AddCommand(pTimedRxCommand);
		//	USER_MESSAGE(ClockManager::GetClockManager().GetSysClock().GetClock() << ": NodeB is going to transimmitt Channel " << pChannel  << " after " << TxTiming << "RXCLOCK");
		}
	}

	bool NodeB::CheckResource( const PhyResourceList & ResList ) const	//��Դ���
	{
		return m_pResManager->Require(ResList);
	}

	bool NodeB::AllocateResource( const PhyResourceList & ResList )	//��Դ����
	{
		return m_pResManager->Assign(ResList);
	}

	bool NodeB::CheckPower( const PhyResourceList & ResList )	//���ʼ��
	{
		return true;
		throw UnFinishedFunc();
	}

	bool NodeB::AllocatePower( const PhyResourceList & ResList )	//���ʷ���
	{
		return true;
		throw UnFinishedFunc();
	}

	shared_ptr<TDDownlinkScheduler> NodeB::SetTDDLScheduler( shared_ptr<TDDownlinkScheduler> pTDScheduler )	//�������е���
	{
		pTDScheduler.swap(m_pTDDLScheduler);
		return pTDScheduler;
	}

	shared_ptr<FDDownlinkScheduler> NodeB::SetFDDLScheduler( shared_ptr<FDDownlinkScheduler> pFDScheduler )	//�������е���
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
		//���ڵõ��������߶˿���
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
		//Ĭ�ϵ�����ͬ��ID�ŵ�LTE NB��HighLayerController���Ӧ
		shared_ptr<HighLayerController> pHighLayerController = pSystem->GetAssociateHighLayerController( shared_from_this() );
		
		LTENBReport lteNBReport;
		//����Report
		for ( UELIST::iterator mapIter = m_uelist.begin(); mapIter != m_uelist.end(); ++mapIter )
		{
			if ( shared_ptr<ILTEUE> pUE = mapIter->second.lock() )
			{
				shared_ptr<Reporter> pReporter = pUE->GetReporter();
				lteNBReport.m_UESINRVecMap.insert( make_pair( pUE, pReporter->GetSINRList() ) );
			}
			//��eNodeBһ���ж���RB�������˶���RB����ʾϵͳ�ĸ���
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

		//��������LTE NodeB��߲㷴������Ϣ���ο���ClockΪLTE SubFrame Clock
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
