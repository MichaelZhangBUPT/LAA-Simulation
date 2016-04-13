#include <vector>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

#include "commondef.h"
#include "UE.h"
#include "DB.h"
#include "System.h"
#include "NodeB.h"
#include "ClockManager.h"
#include "Channel.h"
#include "SINRCalculator.h"
#include "Decisioner.h"
#include "Service.h"
#include "DLFTPService.h"
#include "TimedCommand.h"
#include "ReporterRxCommand.h"
#include "RunParameters.h"
#include "CellSearchCommand.h"
#include "HARQEntity.h"
#include "SHARQEntity.h"
#include "ASHARQEntity.h"
#include "WiFiAP.h"
#include "SimLog.h"

namespace LTESim
{

	UE::UE( int ueid, const POS& pos, shared_ptr<Antenna> pRxAntenna)
		:	SystemElement(ueid, pos), 
			m_pRxAntenna(pRxAntenna)
	{
	    
	}

	UE::~UE(void)
	{

	}

	//���UE����ϵ��
	double UE::GetNoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	const std::map<int, NodeB::PTR>& UE::GetCandidateNodeBSet() const
	{
		//Ӧ�����ظ�UE�ĺ�ѡ��������Ϊ�򵥷�������
		return m_pSystem.lock()->GetNBList();
	}
	//��õ�ǰ���ڵĻ�վ
	shared_ptr<NodeB> UE::GetServingNodeB() const
	{
		return m_pServingNodeB.lock();
	}

	void UE::AddToActiveSet( const shared_ptr<NodeB> pNodeB )
	{
		//���ڼ����ֻ��һ��NodeB
		NodeB::PTR TEMPPTR(m_pServingNodeB.lock());
		m_pServingNodeB = pNodeB;
		return;
	}

#pragma region ʱ����Ӧ������

	void LTESim::UE::OnClock( const Clock& clock )
	{ 
		//����֮ǰ������������
		switch (clock.GetID())
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
		//��������
		CMDEngine::Run(clock);
	}

	void UE::FrameProcess( const Clock& clock )
	{
	   //USER_MESSAGE("UE " << GetID() << " is processing Frame " << clock.GetClock());

		//��UEû��NodeB������UE����С��
		if (!m_pServingNodeB.lock())
		{
			Command::PTR pSearchCommand( new CellSearchCommand(shared_from_this()) );
			AddCommand(pSearchCommand);
		}
	}

	void UE::SubFrameProcess( const Clock& clock )
	{
		if( m_pServingNodeB.lock()->GetIsUpdateCSI() )
			UpdateCSI(0);
		//USER_MESSAGE("UE " << GetID() << " is processing SubFrame " << clock.GetClock());
	}

	void UE::SlotProcess( const Clock& clock )
	{
		//USER_MESSAGE("UE " << GetID() << " is processing Slot " << clock.GetClock());
	}

	void UE::TxProcess( const Clock& clock )
	{
	   // USER_MESSAGE("UE " << GetID() << " is processing TX" << clock.GetClock());
	}

	void UE::RxProcess( const Clock& clock )
	{
		//USER_MESSAGE("UE " << GetID() << " is processing RX" << clock.GetClock());
	}

	//��������RB��SINR�Լ�CQI�����ڴ���ȥ�Ĳ�������
	void UE::MeasureSINRAndCQIofAllTheRB( SINRLIST &sinrlist, CQILIST &cqilist )
	{
		shared_ptr<NodeB> pNodeB = m_pServingNodeB.lock();
		//�ж��Ƿ����С��
		if (!pNodeB)
			return;

		ClockManager & clockMgr = ClockManager::Instance();
		int islot = clockMgr.GetLTESlotClock().GetClock();
		int iframe = clockMgr.GetFrameClock().GetClock();
		int iTxAntennaNum = pNodeB->GetTxAntenna()->GetAntennaPortNum();
		const int iRBNum = pNodeB->GetRBPerSlot();
		double dNodeBPowerPerRB = pNodeB->GetMaxTxPowerPerRBPerPort();											//�����2����������ߵ�����
		SINRCalculator &SINRcalc = SINRCalculator::GetDLCalculator();
		for (int i = 0; i < iRBNum; ++i)
		{
			double sinrofrb = 0.0;
			for (int j = 0; j < iTxAntennaNum; ++j)
			{
				PhyResource phy(iframe, islot, j , i);
				phy.SetTxPower(dNodeBPowerPerRB);

				//���ͷּ���ʱ����˫����SINR���
				sinrofrb += SINRcalc.CalcSINRofOneRB(*GetServingNodeB(), *this, phy);
			}
			sinrlist[ i ] = sinrofrb;
			cqilist[ i ]  = m_AMCEntity->SelectMCS(DB(RealTodB(sinrofrb)));
		}
		if ( GetServingNodeB()->GetID() == 1 )
			SimLog::Instance()->LogSINR( GetID(), ClockManager::Instance().GetFrameClock().GetClock(), ClockManager::Instance().GetCurrentTime(), sinrlist );
	}

#pragma endregion

	//��ý�������
	shared_ptr<Antenna> UE::GetRxAntenna() const
	{
		return m_pRxAntenna;
	}

	const POS& UE::GetPos() const
	{
		return SystemElement::GetPos();
	}

	int UE::GetID() const
	{
		return SystemElement::GetID();
	}

	shared_ptr<System> UE::Attach( shared_ptr<System> pNewSys )
	{
		return SystemElement::Attach( pNewSys );
	}

	SystemType UE::GetType() const
	{
		return SYSTEM_LTE;
	}

	void UE::Initialization()
	{
		m_DLHARQEntity = shared_ptr<HARQEntity>( new ASHARQEntity( shared_from_this(), SYSTEM_LTE ) );
		
		m_CSIReporter = shared_ptr<Reporter> (new ReporterImp);

		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_AMCEntity = shared_ptr<AMCEntity>(
			new AMCEntity(pSystem->GetAMCTable(), 
			DB(pSystem->GetConfig().GetDouble("AMC_UPSTEP_DB")),
			DB(pSystem->GetConfig().GetDouble("AMC_DOWNSTEP_DB")))
			);
		//UE����ϵ��
		m_dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("UE_Noise_Figure(db)"));
	}

	void UE::ReceiveChannel( const shared_ptr<Channel> pRxChannel )
	{
		ClockManager& clock = ClockManager::Instance();
		int islot = clock.GetLTESlotClock().GetClock();
		int isubframe = clock.GetLTESubFrameClock().GetClock();
		int iframe = clock.GetFrameClock().GetClock();
		//����ŵ��Ƿ�ʹ�ô�ʱ϶
		if (pRxChannel->UsedSlot(islot))
		{	
			SINRCalculator &SINRcalc = SINRCalculator::GetDLCalculator();
			
			SINRcalc.CalculateSINRofChannel(pRxChannel, iframe, islot);

			//TTI�����һ��ʱ϶������֡�����о�
			if (pRxChannel->GetLastSlot() == islot) 
			{
				MakeDecision(pRxChannel, iframe, isubframe);
			}
			//UpdateDLHARQProcess();//���½��̺ŷ���UE��
		}
		else
			throw std::logic_error("ִ�н��������ʱ�����ŵ�û��ʹ�ô�ʱ϶");
	}

	void UE::MakeDecision( const shared_ptr<Channel> pRxChannel, int iframeno, int isubframeno)
	{
		/*
		USER_MESSAGE("UE "<<m_ID<<" Make Decision " 
			<<ClockManager::Instance().GetFrameClock().GetClock()<<" "
			<<ClockManager::Instance().GetSubFrameClock().GetClock()<<" "
			<< ClockManager::Instance().GetLTESlotClock().GetClock() )
			*/
		const Decisioner& decisoner = m_pSystem.lock()->GetDecisioner(SYSTEM_LTE);
		SINRCalculator &SINRcalc = SINRCalculator::GetDLCalculator();
		shared_ptr<HARQ> pHARQ = pRxChannel->GetHARQ();
		
		//�ش�����Ӧ�ý���һ��HARQ����ʱSINRֵȡ�ô���
		if (!pHARQ->GetNewPacketIndicator())
		{			
			pHARQ->SINRCombining(pRxChannel->GetSINRList());
		}
		
		double EffSINR = SINRcalc.EESMCalculator(pRxChannel->GetSINRList(), *pRxChannel);
		//if ( pRxChannel->GetActiveNodeB()->GetID() == 0 )
		//{
		//	SimLog::Instance()->LogUESINR( ClockManager::Instance().GetCurrentTime(), pRxChannel->GetActiveNodeB()->GetID(), pRxChannel->GetActiveUE()->GetID(), RealTodB(EffSINR) );
		//}
		bool   FrameOK = decisoner.CheckCRC(pRxChannel->GetMCS(), DB(RealTodB(EffSINR)));
		
		//����AMC����
		m_AMCEntity->UpdateThreshold(FrameOK);

		//����MCS�ȼ�
		UpdateMCSIndex(pRxChannel);	//YRM
		
		if (FrameOK)
		{
			//������ʲô��
			pHARQ->Reset();
			//�о��ɹ�����������
			UpdateThroughput(pRxChannel);
			m_pService->SetLastTransTime( ClockManager::Instance().GetCurrentTime() );
		}
		else
		{
			//�Ƿ�Ӧ���ж�һ�µ�������ش�������
			//�ش������ڷ��Ͷ˸���
			//��������ش�����
			if(pHARQ->MaxTransTime())
			{
				USER_MESSAGE("�ﵽ����ش���������ֹ�˴δ���");
				//std::cout << endl << "LTE UE:�ﵽ����ش���������ֹ�˴δ���" << endl;
				pHARQ->Reset();
				UpdateDiscardThroughput(pRxChannel);//���¶������ݴ�С
			}
			else
			{
				//��¼�������SINR
				pHARQ->RecordSINROfRetranPacket( pRxChannel->GetSINRList());
				pHARQ->UpdateTranTime();																					//���´������
			}
		}

		pHARQ->UpdateTransStatus(true);
	}

	bool UE::AddService( shared_ptr<Service> pService )
	{
		//������ʺϴ�UE��ҵ������Ӧ�����벢����true��������ʺϴ�UE����false�����ڼٶ�����UE���ܳ�������ҵ��
		m_pService = pService;
		return true;
	}

	shared_ptr<Service> UE::GetService( SystemType systemType )
	{
		if ( systemType == SYSTEM_LTE )
			return m_pService;
		else
			throw std::invalid_argument( "Invalid System Type in LTE UE" );
	}


	shared_ptr<HARQ> UE::GetCurrentDLHARQ( SystemType systemType )	//�������HARQ
	{
		if ( systemType == SYSTEM_LTE )
			return m_DLHARQEntity->GetHARQ();
		else
			throw std::invalid_argument( "Invalid SystemType" );
	}

	shared_ptr<HARQ> UE::GetNextDLHARQ( SystemType systemType )  // ��õ�ǰ��һ��HARQ����
	{
		if ( systemType == SYSTEM_LTE )
			return m_DLHARQEntity->GetNextHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	void UE::UpdateDLHARQProcess( SystemType systemType )	//�������HARQ�Ľ���
	{
		if ( systemType == SYSTEM_LTE )
			m_DLHARQEntity->UpdateHARQProcessNo();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}


	void UE::UpdateAverageThroughput( const shared_ptr<Channel> pChannel )
	{
		const double Tc=1.7;
		const size_t CRCSIZE = 24;
		double dTableBlock = 0.0;
		if ( pChannel )
			dTableBlock = pChannel->GetMCS().GetTableBlock() * pChannel->GetRBNum() - CRCSIZE;
		
		double dAverageThroughput = ( 1 - 1/Tc )*m_pService->GetAverageThroughput() + 1/Tc * dTableBlock;
		m_pService->SetAverageThroughput( dAverageThroughput );
	}

	void UE::GetRBListofSlot( const PhyResourceList &SourceRBlist, PhyResourceList& DesRBList, int frame, int slot )
	{
		throw UnFinishedFunc();
	}

	void UE::UpdateThroughput( const shared_ptr<Channel> pchannel )	//����������
	{
		const size_t CRCSIZE = 24;
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pchannel->GetMCS().GetTableBlock() * pchannel->GetRBNum() - CRCSIZE;
			if ( pSystem->StatisticEnabled())
			{
				m_pService->UpdateThroughput( dTableBlock );
				m_pServingNodeB.lock()->UpdateCellThroughput( dTableBlock );

				if ( m_pService->GetIsEnd() )
					SimLog::Instance()->LogBufferEmpty( m_ID, ClockManager::Instance().GetFrameClock().GetClock(), GetType(), SYSTEM_LTE );
			}
			m_pService->SetLastPacketsize( dTableBlock );
		}
	}

	void UE::UpdateDiscardThroughput( const shared_ptr<Channel> pchannel )	//���¶��������ݰ���������
	{
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pchannel->GetMCS().GetTableBlock() * pchannel->GetRBNum();
			if ( pSystem->StatisticEnabled())
			{
				pchannel->GetActiveService()->UpdateDiscardThroughput( dTableBlock );
			}
		}
	}

	void UE::UpdateCSI( int delayslotnum )
	{
		if (shared_ptr <NodeB> pNodeb = this->GetServingNodeB())
		{
			SINRLIST sinrlist(pNodeb->GetRBPerSlot());
			CQILIST cqilist(pNodeb->GetRBPerSlot());
			MeasureSINRAndCQIofAllTheRB(sinrlist,cqilist);
			Clock & clock = ClockManager::Instance().GetLTESubFrameClock();

			shared_ptr <UE> pUE = shared_from_this();
			shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(pUE);
			shared_ptr<Command> pRxReportercommand(new ReporterRxCommand(pUE, sinrlist, cqilist));
			shared_ptr<TimedCommand> pTimecommand(new TimedCommand(clock,pCmdEngine,pRxReportercommand) );
			pTimecommand->SetTimer(delayslotnum);
			AddCommand(pTimecommand);
		}
	}

	void UE::UpdateReport( const SINRLIST & sinrlist, const CQILIST& cqilist )	//���·���
	{
		assert(sinrlist.size() == cqilist.size());
		for (size_t i = 0; i < sinrlist.size(); ++i)
		{
			m_CSIReporter->UpdateSINR((int)i, sinrlist[i]);
			m_CSIReporter->UpdateCQI((int)i, cqilist[i]);
		}
	}

	shared_ptr<Reporter> UE::GetReporter() const
	{
		return m_CSIReporter;
	}

	void UE::UpdateMCSIndex( const shared_ptr<Channel> pchannel )
	{
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			if ( pSystem->StatisticEnabled())
			{
				m_pService->UpdateMCSIndex( pchannel->GetMCS().ToIndex());
			}
		}
	}

	double UE::GetDin() const
	{
		return m_dDin;
	}

	void UE::SetDin(double din)
	{
		m_dDin = din;
	}

}


