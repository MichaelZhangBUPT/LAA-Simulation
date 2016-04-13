#include "DualModeLTEHSPAUE.h"
#include "commondef.h"
#include "DB.h"
#include "System.h"
#include "NodeB.h"
#include "ClockManager.h"
#include "Channel.h"
#include "SINRCalculator.h"
#include "Decisioner.h"
#include "Service.h"
#include "ServiceComposite.h"
#include "DLFTPService.h"
#include "Command.h"
#include "TimedCommand.h"
#include "ReporterRxCommand.h"
#include "RunParameters.h"
#include "CellSearchCommand.h"
#include "HARQEntity.h"
#include "SHARQEntity.h"
#include "ASHARQEntity.h"
#include "HSPAReporter.h"
#include "C2ICalculator.h"
#include "SimLog.h"

#include <vector>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

namespace LTESim
{

#pragma region Constructor And Destructor

	DualModeLTEHSPAUE::DualModeLTEHSPAUE(int iUEid, double dPosX, double dPosY, shared_ptr<Antenna> pAntenna)
		:	SystemElement( iUEid, POS( dPosX, dPosY ) ),
			m_pRxAntenna( pAntenna )
	{

	}

	DualModeLTEHSPAUE::~DualModeLTEHSPAUE()
	{

	}

#pragma endregion

#pragma region Interface IUE

	const POS& DualModeLTEHSPAUE::GetPos() const
	{
		return SystemElement::GetPos();
	}

	int DualModeLTEHSPAUE::GetID() const
	{
		return SystemElement::GetID();
	}

	shared_ptr<System> DualModeLTEHSPAUE::Attach( shared_ptr<System> pNewSys )
	{
		return SystemElement::Attach( pNewSys );
	}

	SystemType DualModeLTEHSPAUE::GetType() const
	{
		return SYSTEM_DUAL;
	}

	//���UE����ϵ��
	double DualModeLTEHSPAUE::GetNoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	bool DualModeLTEHSPAUE::AddService( shared_ptr<Service> pService )
	{
		//DualModeUEֻ�ܹ�ʹ��ServiceComposite
		m_pService = dynamic_pointer_cast<ServiceComposite>(pService);
		if ( !m_pService )
			throw std::invalid_argument( "pService should point to a Service Composite Class" );
		return true;
	}

	shared_ptr<Service> DualModeLTEHSPAUE::GetService( SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
		case SYSTEM_HSPA:
			return m_pService->GetServiceElement( systemType );
		case SYSTEM_DUAL:
			return m_pService;
		default:
			throw std::invalid_argument( "Unknown SystemType" );
		}
	}

	shared_ptr<HARQ> DualModeLTEHSPAUE::GetCurrentDLHARQ( SystemType systemType )	//�������HARQ
	{
		//˫ģUE��LTEϵͳ��HSPAϵͳ��ʹ�ö�����HARQ
		if( systemType == SYSTEM_LTE )
			return m_pLTEDLHARQEntity->GetHARQ();
		else if ( systemType == SYSTEM_HSPA )
			return m_pHSPADLHARQEntity->GetHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	shared_ptr<HARQ> DualModeLTEHSPAUE::GetNextDLHARQ( SystemType systemType )  // ��õ�ǰ��һ��HARQ����
	{
		if ( systemType == SYSTEM_LTE )
			return m_pLTEDLHARQEntity->GetNextHARQ();
		else if ( systemType == SYSTEM_HSPA )
			return m_pHSPADLHARQEntity->GetNextHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	void DualModeLTEHSPAUE::UpdateDLHARQProcess( SystemType systemType )	//�������HARQ�Ľ���
	{
		if ( systemType == SYSTEM_LTE )
			m_pLTEDLHARQEntity->UpdateHARQProcessNo();
		else if ( systemType == SYSTEM_HSPA )
			m_pHSPADLHARQEntity->UpdateHARQProcessNo();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	//��ý�������
	shared_ptr<Antenna> DualModeLTEHSPAUE::GetRxAntenna() const
	{
		return m_pRxAntenna;
	}

	void DualModeLTEHSPAUE::Initialization()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		//HARQ
		m_pLTEDLHARQEntity = shared_ptr<HARQEntity>(new ASHARQEntity( shared_from_this(), SYSTEM_LTE ) );
		m_pHSPADLHARQEntity = shared_ptr<HARQEntity>(new ASHARQEntity( shared_from_this(), SYSTEM_HSPA ) );

		m_CSIReporter = shared_ptr<Reporter> (new ReporterImp);
		m_pHSPAReporter = shared_ptr<IHSPAReporter>( new HSPAReporter( shared_from_this() ) );

		m_pLTEAMCEntity = shared_ptr<AMCEntity>(
			new AMCEntity(pSystem->GetAMCTable(), 
			DB(pSystem->GetConfig().GetDouble("AMC_UPSTEP_DB")),
			DB(pSystem->GetConfig().GetDouble("AMC_DOWNSTEP_DB")))
			);
		m_pHSPAAMCEntity = shared_ptr<AMCEntity>(
			new AMCEntity(
			pSystem->GetHSPAAMCTable(), 
			DB(pSystem->GetConfig().GetDouble("HSPA_AMC_UPSTEP_DB")),
			DB(pSystem->GetConfig().GetDouble("HSPA_AMC_DOWNSTEP_DB")))
			);

		//UE����ϵ��
		m_dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("UE_Noise_Figure(db)"));
	}

#pragma endregion

#pragma region ʱ����Ӧ������

	void DualModeLTEHSPAUE::OnClock( const Clock& clock )
	{
		//����֮ǰ������������
		switch (clock.GetID())
		{
		case LTESUBFRAMECLOCK:
			SubFrameProcess(clock);
			break;
		case HSPASUBFRAMECLOCK:
			SubFrameProcess(clock);
			break;
		case LTESLOTCLOCK:
			SlotProcess(clock);
			break;
		case HSPASLOTCLOCK:
			SlotProcess(clock);
			break;
		case HIGHLAYERCLOCK:
			break;
		default:
			ClockProcessor::OnClock(clock);
			break;
		}
		//��������
		Run(clock);
	}

	void DualModeLTEHSPAUE::FrameProcess( const Clock& clock )
	{
		USER_MESSAGE( "DualModeLTEHSPAUE " << GetID() << " DualModeLTEHSPAUE::FrameProcess. Clock ID:\t"<<clock.GetID() );
		//��ÿһ֡�Ŀ�ͷ������Ƿ������LTE NodeB����HSPA NodeB�����û�н��룬
		//������С����������
		if ( !m_pServingLTENodeB.lock() )
		{
			Command::PTR pCommand( new CellSearchCommand( shared_from_this() ) );
			AddCommand( pCommand );
		}
		if ( !m_pServingHSPANodeB.lock() )
		{
			Command::PTR pCommand( new HSPACellSearchCommand( shared_from_this() ) );
			AddCommand( pCommand );
		}
	}

	void DualModeLTEHSPAUE::SubFrameProcess( const Clock& clock )
	{
		USER_MESSAGE( "DualModeLTEHSPAUE " << GetID() << " DualModeLTEHSPAUE::SubFrameProcess. Clock ID:\t"<<clock.GetID() );
		if ( clock.GetID() == ClockManager::Instance().GetLTESubFrameClock().GetID() )
		{
			UpdateCSI( 0 );
		}
		if ( clock.GetID() == ClockManager::Instance().GetHSPASubFrameClock().GetID() )
		{
		}
	}

	void DualModeLTEHSPAUE::SlotProcess( const Clock& clock )
	{
		if ( clock.GetID() == ClockManager::Instance().GetLTESlotClock().GetID() )
		{
			USER_MESSAGE( "DualModeLTEHSPAUE " << GetID() << " DualModeLTEHSPAUE::LTESlotProcess. Clock ID:\t"<<clock.GetID() );
		}
		else if ( clock.GetID() == ClockManager::Instance().GetHSPASlotClock().GetID() )
		{
			USER_MESSAGE( "DualModeLTEHSPAUE " << GetID() << " DualModeLTEHSPAUE::HSPASlotProcess. Clock ID:\t"<<clock.GetID() );
		}
	}

	void DualModeLTEHSPAUE::TxProcess( const Clock& clock )
	{
		USER_MESSAGE( "DualModeLTEHSPAUE " << GetID() << " DualModeLTEHSPAUE::TxProcess. Clock ID:\t"<<clock.GetID() );
	}

	void DualModeLTEHSPAUE::RxProcess( const Clock& clock )
	{
		USER_MESSAGE( "DualModeLTEHSPAUE " << GetID() << " DualModeLTEHSPAUE::RxProcess. Clock ID:\t"<<clock.GetID() );
	}

#pragma endregion

#pragma region Interface ILTEUE

	const std::map<int, NodeB::PTR>& DualModeLTEHSPAUE::GetCandidateNodeBSet() const
	{
		//Ӧ�����ظ�UE�ĺ�ѡ��������Ϊ�򵥷�������
		return m_pSystem.lock()->GetNBList();
	}

	//��õ�ǰ���ڵĻ�վ
	shared_ptr<NodeB> DualModeLTEHSPAUE::GetServingNodeB() const
	{
		return m_pServingLTENodeB.lock();
	}

	void DualModeLTEHSPAUE::AddToActiveSet( const shared_ptr<NodeB> pNodeB )
	{
		//���ڼ����ֻ��һ��NodeB
		NodeB::PTR TEMPPTR(m_pServingLTENodeB.lock());
		m_pServingLTENodeB = pNodeB;
		return;
	}

	//���յ�ΪLTEϵͳ���ŵ�
	void DualModeLTEHSPAUE::ReceiveChannel( const shared_ptr<Channel> pRxChannel )
	{
		ClockManager & clock = ClockManager::Instance();
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

	void DualModeLTEHSPAUE::MakeDecision( const shared_ptr<Channel> pRxChannel, int iframeno, int isubframeno)
	{
		const Decisioner& decisoner = m_pSystem.lock()->GetDecisioner(SYSTEM_LTE);
		SINRCalculator& SINRcalc = SINRCalculator::GetDLCalculator();
		shared_ptr<HARQ> pHARQ = pRxChannel->GetHARQ();

		//�ش�����Ӧ�ý���һ��HARQ����ʱSINRֵȡ�ô���
		if (!pHARQ->GetNewPacketIndicator())
		{			
			pHARQ->SINRCombining(pRxChannel->GetSINRList());
		}

		double EffSINR = SINRcalc.EESMCalculator(pRxChannel->GetSINRList(), *pRxChannel);
		bool   FrameOK = decisoner.CheckCRC(pRxChannel->GetMCS(), DB(RealTodB(EffSINR)));

		//����AMC����
		//Type Dependent
		m_pLTEAMCEntity->UpdateThreshold(FrameOK);

		//����MCS�ȼ�
		UpdateMCSIndex(pRxChannel);	//YRM
		//����������ݰ��Ĵ������
		m_pService->GetServiceElement( SYSTEM_LTE )->UpdatePacketTransTimes();

		if (FrameOK)
		{
			//������ʲô��
			pHARQ->Reset();
			//�о��ɹ�����������
			UpdateThroughput(pRxChannel);
		}
		else
		{
			//�Ƿ�Ӧ���ж�һ�µ�������ش�������
			//�ش������ڷ��Ͷ˸���
			//��������ش�����

			//����������ݰ��Ĵ��������
			m_pService->GetServiceElement( SYSTEM_LTE )->UpdatePacketFailedTransTimes();

			if(pHARQ->MaxTransTime())
			{
				USER_MESSAGE("�ﵽ����ش���������ֹ�˴δ���");
				pHARQ->Reset();
				UpdateDiscardThroughput(pRxChannel);//���¶������ݴ�С
			}
			else
			{
				//��¼�������SINR
				pHARQ->RecordSINROfRetranPacket( pRxChannel->GetSINRList());
				pHARQ->UpdateTranTime();	
			}
		}

		pHARQ->UpdateTransStatus(true);
	}

	void DualModeLTEHSPAUE::GetRBListofSlot( const PhyResourceList &SourceRBlist, PhyResourceList& DesRBList, int frame, int slot )
	{
		throw UnFinishedFunc();
	}

	void DualModeLTEHSPAUE::UpdateAverageThroughput( const shared_ptr<Channel> pChannel )
	{
		const double Tc=1.7;
		const size_t CRCSIZE = 24;
		double dTableBlock = 0.0;
		if ( pChannel )
			dTableBlock = pChannel->GetMCS().GetTableBlock() * pChannel->GetRBNum() - CRCSIZE;
				
		shared_ptr<Service> pLTEService = m_pService->GetServiceElement( SYSTEM_LTE );
		double dAverageThroughput = ( 1 - 1/Tc )*pLTEService->GetAverageThroughput() + 1/Tc * dTableBlock;

		//Channel��ʾ����LTE Channel������ֻ����LTE Service���ֵ�ƽ��������
		pLTEService->SetAverageThroughput( dAverageThroughput );
	}

	void DualModeLTEHSPAUE::UpdateThroughput( const shared_ptr<Channel> pchannel )	//����������
	{
		const size_t CRCSIZE = 24;
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pchannel->GetMCS().GetTableBlock() * pchannel->GetRBNum() - CRCSIZE;
			//To Be Modified, Type Dependent
			shared_ptr<Service> pLTEService = m_pService->GetServiceElement( SYSTEM_LTE );
			if ( pSystem->StatisticEnabled())
			{
				//To Be Modified��ͬʱ������Composite Service��Service Element��������
				//����Composite Service���п��ܻᵼ��Buffer��Ϊ�գ��Ӷ�����FTP Buffer�ĸ���
				pLTEService->UpdateThroughput( dTableBlock );
				m_pServingLTENodeB.lock()->UpdateCellThroughput( dTableBlock );

				if ( pLTEService->GetIsEnd() )
					SimLog::Instance()->LogBufferEmpty( m_ID, ClockManager::Instance().GetFrameClock().GetClock(), GetType(), SYSTEM_LTE );
			}
			m_pService->SetLastPacketsize( dTableBlock );
			pLTEService->SetLastPacketsize( dTableBlock );
		}
	}

	void DualModeLTEHSPAUE::UpdateDiscardThroughput( const shared_ptr<Channel> pchannel )	//���¶��������ݰ���������
	{
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pchannel->GetMCS().GetTableBlock() * pchannel->GetRBNum();
			//To Be Modified, Type Dependent
			shared_ptr<Service> pLTEService = m_pService->GetServiceElement( SYSTEM_LTE );
			if ( pSystem->StatisticEnabled())
			{
				pchannel->GetActiveService()->UpdateDiscardThroughput( dTableBlock );
				pLTEService->UpdateDiscardThroughput( dTableBlock );
			}
		}
	}

	void DualModeLTEHSPAUE::UpdateCSI( int delayslotnum )
	{
		if (shared_ptr <NodeB> pNodeb = this->GetServingNodeB())
		{
			SINRLIST sinrlist(pNodeb->GetRBPerSlot());
			CQILIST cqilist(pNodeb->GetRBPerSlot());
			MeasureSINRAndCQIofAllTheRB(sinrlist,cqilist);

			Clock & clock = ClockManager::Instance().GetLTESubFrameClock();
			shared_ptr <DualModeLTEHSPAUE> pUE = shared_from_this();
			shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(pUE);
			shared_ptr<Command> pRxReportercommand(new ReporterRxCommand(pUE, sinrlist, cqilist));
			shared_ptr<TimedCommand> pTimecommand(new TimedCommand(clock,pCmdEngine,pRxReportercommand) );
			pTimecommand->SetTimer(delayslotnum);
			AddCommand(pTimecommand);
		}
	}

	void DualModeLTEHSPAUE::UpdateReport( const SINRLIST & sinrlist, const CQILIST& cqilist )	//���·���
	{
		assert(sinrlist.size() == cqilist.size());
		for (size_t i = 0; i < sinrlist.size(); ++i)
		{
			m_CSIReporter->UpdateSINR((int)i, sinrlist[i]);
			m_CSIReporter->UpdateCQI((int)i, cqilist[i]);
		}
	}

	shared_ptr<Reporter> DualModeLTEHSPAUE::GetReporter() const
	{
		return m_CSIReporter;
	}

	void DualModeLTEHSPAUE::UpdateMCSIndex( const shared_ptr<Channel> pchannel )
	{
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			//To Be Modified, Type Dependent
			shared_ptr<Service> pLTEService = m_pService->GetServiceElement( SYSTEM_LTE );
			if ( pSystem->StatisticEnabled())
			{
				m_pService->UpdateMCSIndex( pchannel->GetMCS().ToIndex() );
				pLTEService->UpdateMCSIndex( pchannel->GetMCS().ToIndex() );
			}
		}
	}

	//��������RB��SINR�Լ�CQI�����ڴ���ȥ�Ĳ�������
	void DualModeLTEHSPAUE::MeasureSINRAndCQIofAllTheRB( SINRLIST &sinrlist, CQILIST &cqilist )
	{
		shared_ptr<NodeB> pNodeB = m_pServingLTENodeB.lock();
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
		for (int i = 0; i <	iRBNum; ++i)
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
			//Type Dependent
			cqilist[ i ]  = m_pLTEAMCEntity->SelectMCS(DB(RealTodB(sinrofrb)));
		}
	}

#pragma endregion

#pragma region HSPAUE Interface

	void DualModeLTEHSPAUE::AddToActiveSet( const shared_ptr<HSPANodeB> pNodeB )
	{
		m_pServingHSPANodeB = pNodeB;
	}

	void DualModeLTEHSPAUE::ReceiveChannel( const shared_ptr<HSPAChannel> pChannel )
	{
		ClockManager& refClkManager = ClockManager::Instance();
		C2ICalculator * pC2ICalc = C2ICalculator::Instance();
		int iSlot = refClkManager.GetHSPASlotClock().GetClock();
		
		//�����ﲻ����Channel���յ���ʱ�����Կ���ʹ��IsUsedSlot���м�⣬
		//�������������ʱ���ü�ⲻ�ܹ�ͨ��
		if ( pChannel->IsUsedSlot( iSlot ) )
		{
			pC2ICalc->CalculateC2IOfChannel( pChannel );

			if ( pChannel->IsLastSlot( iSlot ) )
			{
				MakeDecision( pChannel );
			}
		}
		else
		{
			throw std::logic_error( "δʹ�ø�ʱ϶" );
		}
	}

	void DualModeLTEHSPAUE::MakeDecision( shared_ptr<HSPAChannel> pChannel )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		C2ICalculator * pC2ICalc = C2ICalculator::Instance();
		double dCurrentC2IValue = pC2ICalc->CalculateEffectiveC2I( pChannel );
		double dC2IValue = dCurrentC2IValue;	

		//Type Dependent
		shared_ptr<HARQ> pHARQ = m_pHSPADLHARQEntity->GetHARQ();
		//SimLog::Instance()->MakeDecisionCount( m_ID, ClockManager::Instance().GetFrameClock().GetClock(), GetType(), SYSTEM_HSPA );
		//�����ж����µ����ݴ��䣬�����ش����ݴ���
		//������ش������ݣ�����Ҫ����C2I�ĺϲ�
		if ( pHARQ->GetNewPacketIndicator() == false )
		{
			double dRetransC2IValue = pHARQ->GetRetransC2I();
			dC2IValue += dRetransC2IValue;
		}

		double dEb2No =	C2IToEb2No(dC2IValue,SF_16, pChannel->GetMCS().GetCodeGain(), pChannel->GetMCS().GetModulation());
		// Type Dependent
		const Decisioner& refDecisioner = pSystem->GetDecisioner( SYSTEM_HSPA );
		bool bIsTransSuccess = refDecisioner.CheckCRC( pChannel->GetMCS(), DB(RealTodB( dEb2No )) );
		//cout << bIsTransSuccess << " dEb2No " << dEb2No << endl;

		//Type Dependent
		m_pHSPAAMCEntity->UpdateThreshold( bIsTransSuccess );

		//����������ݰ��Ĵ������
		m_pService->GetServiceElement( SYSTEM_HSPA )->UpdatePacketTransTimes();

		//To Be Modified
		//��PF�㷨������Ҫ���ݽ����������ƽ���������ĸ���
		//UE�����Ƿ�����ȷ��Ȼ�����MCS����
		if( bIsTransSuccess )
		{
			UpdateThroughput( pChannel );
			UpdateAverageThroughput( pChannel );
			pHARQ->Reset();
		}
		else
		{
			//����������ݰ��Ĵ��������
			m_pService->GetServiceElement( SYSTEM_HSPA )->UpdatePacketFailedTransTimes();
			if ( pHARQ->MaxTransTime() )
			{
				//To Be Modified����HSPAUE�п������UpdateDiscardThroughput
				pHARQ->Reset();
				USER_MESSAGE("HSPA UE���н��մﵽ����ش���������ֹ�˴δ���");
			}
			else
			{
				//dC2IValue���Ѿ�ԭ�����������ش���C2I��Ȼ��ֱ�Ӹ�ֵ����
				pHARQ->SetRetransC2I( dC2IValue );
				//���´������
				pHARQ->UpdateTranTime();
			}
			UpdateAverageThroughput( shared_ptr<HSPAChannel>() );
		}
		//�ŵ�������������Ƿ�����ȷ��ֱ�Ӹ��´���״̬ΪTrue
		pHARQ->UpdateTransStatus( true );

		//������˵�ǰ���ŵ�״����Ϣ��������Reporter�н�����д洢����¼��ǰ���ŵ���Ϣ����PF�㷨��Ҳ��Ҫʹ��
		//���ݵ�ǰ�ŵ���HMatrix�������´δ�����Ƽ���Ȩ��, �õ��Ƽ��ķ���Ȩֵ
		Matrix recommendWeightMatrix = pC2ICalc->MatrixGenerator(pChannel);
		//���ݵ�ǰ��Es2No���и����Ƽ���MCS���ޣ�Ȼ������ϱ�
		//Type Dependent
		MCS recommendMCS = m_pHSPAAMCEntity->SelectMCS( DB(RealTodB(dEb2No)) );
		UpdateReport( dCurrentC2IValue, recommendMCS, recommendWeightMatrix );
	}

	void DualModeLTEHSPAUE::UpdateThroughput(const shared_ptr<HSPAChannel> pChannel )
	{
		//����������ͳ�� -- To Be Modified
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pChannel->GetMCS().GetTableBlock() * pChannel->GetCodeChannelSet().size();
			//To Be Modified, Type Dependent
			//������������ʱ��ServiceComposite�����е�HSPA ServiceԪ�ض���Ҫ����
			shared_ptr<Service> pHSPAService = m_pService->GetServiceElement( SYSTEM_HSPA );
			if ( pSystem->StatisticEnabled() )
			{
				//To Be Modified��ͬʱ������Composite Service��HSPA Service Element��������
				//����Composite Service���п��ܻᵼ��Buffer��Ϊ�գ��Ӷ�����FTP Buffer�ĸ���
				pHSPAService->UpdateThroughput( dTableBlock );

				if ( pHSPAService->GetIsEnd() )
					SimLog::Instance()->LogBufferEmpty( m_ID, ClockManager::Instance().GetFrameClock().GetClock(), GetType(), SYSTEM_HSPA );
			}
			m_pService->SetLastPacketsize( dTableBlock );
			pHSPAService->SetLastPacketsize( dTableBlock );
		}
	}

	void DualModeLTEHSPAUE::UpdateAverageThroughput( const shared_ptr<HSPAChannel> pChannel )
	{
		double Tc = 1.7;
		double dTableBlock = 0.0;
		//double dBlock = 0.0;
		//int iCodeSize = 0;


		if ( pChannel )
			dTableBlock = pChannel->GetMCS().GetTableBlock() * pChannel->GetCodeChannelSet().size();
			//dBlock = pChannel->GetMCS().GetTableBlock();
			//iCodeSize =  pChannel->GetCodeChannelSet().size();}
		
		shared_ptr<Service> pHSPAService = m_pService->GetServiceElement( SYSTEM_HSPA );
		double dAverageThr = (1- 1/Tc)*pHSPAService->GetAverageThroughput() + 1/Tc * dTableBlock;
		//SimLog::Instance()->LogC2I( GetID(), ClockManager::Instance().GetFrameClock().GetClock(), dBlock, iCodeSize, dAverageThr );

		//To Be Modified, Type Dependent
		//ֻ����HSPA Service��һ���ֵ�ƽ������������HSPAϵͳ�еĵ���ʱ�����
		pHSPAService->SetAverageThroughput( dAverageThr );
	}
	
	shared_ptr<IHSPAReporter> DualModeLTEHSPAUE::GetHSPAReporter() const
	{
		return m_pHSPAReporter;
	}

	void DualModeLTEHSPAUE::UpdateReport( double dC2IValue, const MCS& recommendMCS, const Matrix& recommendDLWeightMatrix )
	{
		//Type Dependent
		m_pHSPAReporter->SetC2I( dC2IValue );
		m_pHSPAReporter->SetRecommendMCS( recommendMCS );
		m_pHSPAReporter->SetRecommendDLTransWeightMatrix( recommendDLWeightMatrix );
	}

	shared_ptr<HSPANodeB> DualModeLTEHSPAUE::GetServingHSPANodeB() const
	{
		return m_pServingHSPANodeB.lock();
	}

	void DualModeLTEHSPAUE::SetAntenna( shared_ptr<Antenna> Aptr )
	{
		m_pRxAntenna = Aptr;
	}

	double DualModeLTEHSPAUE::GetDin() const
	{
		return m_dDin;
	}

	void DualModeLTEHSPAUE::SetDin(double din)
	{
		m_dDin = din;
	}

#pragma endregion
}