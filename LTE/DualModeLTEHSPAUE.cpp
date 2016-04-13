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

	//获得UE噪声系数
	double DualModeLTEHSPAUE::GetNoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	bool DualModeLTEHSPAUE::AddService( shared_ptr<Service> pService )
	{
		//DualModeUE只能够使用ServiceComposite
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

	shared_ptr<HARQ> DualModeLTEHSPAUE::GetCurrentDLHARQ( SystemType systemType )	//获得下行HARQ
	{
		//双模UE在LTE系统和HSPA系统中使用独立的HARQ
		if( systemType == SYSTEM_LTE )
			return m_pLTEDLHARQEntity->GetHARQ();
		else if ( systemType == SYSTEM_HSPA )
			return m_pHSPADLHARQEntity->GetHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	shared_ptr<HARQ> DualModeLTEHSPAUE::GetNextDLHARQ( SystemType systemType )  // 获得当前下一个HARQ进程
	{
		if ( systemType == SYSTEM_LTE )
			return m_pLTEDLHARQEntity->GetNextHARQ();
		else if ( systemType == SYSTEM_HSPA )
			return m_pHSPADLHARQEntity->GetNextHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	void DualModeLTEHSPAUE::UpdateDLHARQProcess( SystemType systemType )	//获得下行HARQ的进程
	{
		if ( systemType == SYSTEM_LTE )
			m_pLTEDLHARQEntity->UpdateHARQProcessNo();
		else if ( systemType == SYSTEM_HSPA )
			m_pHSPADLHARQEntity->UpdateHARQProcessNo();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	//获得接收天线
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

		//UE噪声系数
		m_dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("UE_Noise_Figure(db)"));
	}

#pragma endregion

#pragma region 时钟响应函数族

	void DualModeLTEHSPAUE::OnClock( const Clock& clock )
	{
		//处理之前加入的命令队列
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
		//运行命令
		Run(clock);
	}

	void DualModeLTEHSPAUE::FrameProcess( const Clock& clock )
	{
		USER_MESSAGE( "DualModeLTEHSPAUE " << GetID() << " DualModeLTEHSPAUE::FrameProcess. Clock ID:\t"<<clock.GetID() );
		//在每一帧的开头，检测是否接入了LTE NodeB或者HSPA NodeB，如果没有接入，
		//则生成小区接入命令
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
		//应当返回该UE的候选集，但是为简单返回所有
		return m_pSystem.lock()->GetNBList();
	}

	//获得当前所在的基站
	shared_ptr<NodeB> DualModeLTEHSPAUE::GetServingNodeB() const
	{
		return m_pServingLTENodeB.lock();
	}

	void DualModeLTEHSPAUE::AddToActiveSet( const shared_ptr<NodeB> pNodeB )
	{
		//现在激活集中只有一个NodeB
		NodeB::PTR TEMPPTR(m_pServingLTENodeB.lock());
		m_pServingLTENodeB = pNodeB;
		return;
	}

	//接收的为LTE系统的信道
	void DualModeLTEHSPAUE::ReceiveChannel( const shared_ptr<Channel> pRxChannel )
	{
		ClockManager & clock = ClockManager::Instance();
		int islot = clock.GetLTESlotClock().GetClock();
		int isubframe = clock.GetLTESubFrameClock().GetClock();
		int iframe = clock.GetFrameClock().GetClock();
		//监测信道是否使用此时隙
		if (pRxChannel->UsedSlot(islot))
		{	
			SINRCalculator &SINRcalc = SINRCalculator::GetDLCalculator();

			SINRcalc.CalculateSINRofChannel(pRxChannel, iframe, islot);

			//TTI的最后一个时隙，进行帧质量判决
			if (pRxChannel->GetLastSlot() == islot) 
			{
				MakeDecision(pRxChannel, iframe, isubframe);
			}
			//UpdateDLHARQProcess();//更新进程号放在UE端
		}
		else
			throw std::logic_error("执行接收命令的时候发现信道没有使用此时隙");
	}

	void DualModeLTEHSPAUE::MakeDecision( const shared_ptr<Channel> pRxChannel, int iframeno, int isubframeno)
	{
		const Decisioner& decisoner = m_pSystem.lock()->GetDecisioner(SYSTEM_LTE);
		SINRCalculator& SINRcalc = SINRCalculator::GetDLCalculator();
		shared_ptr<HARQ> pHARQ = pRxChannel->GetHARQ();

		//重传数据应该将上一次HARQ传输时SINR值取得处理
		if (!pHARQ->GetNewPacketIndicator())
		{			
			pHARQ->SINRCombining(pRxChannel->GetSINRList());
		}

		double EffSINR = SINRcalc.EESMCalculator(pRxChannel->GetSINRList(), *pRxChannel);
		bool   FrameOK = decisoner.CheckCRC(pRxChannel->GetMCS(), DB(RealTodB(EffSINR)));

		//更新AMC门限
		//Type Dependent
		m_pLTEAMCEntity->UpdateThreshold(FrameOK);

		//更新MCS等级
		UpdateMCSIndex(pRxChannel);	//YRM
		//更新这个数据包的传输次数
		m_pService->GetServiceElement( SYSTEM_LTE )->UpdatePacketTransTimes();

		if (FrameOK)
		{
			//重置了什么？
			pHARQ->Reset();
			//判决成功更新吞吐量
			UpdateThroughput(pRxChannel);
		}
		else
		{
			//是否应当判断一下到达最大重传次数？
			//重传次数在发送端更新
			//到达最大重传次数

			//更新这个数据包的错误传输次数
			m_pService->GetServiceElement( SYSTEM_LTE )->UpdatePacketFailedTransTimes();

			if(pHARQ->MaxTransTime())
			{
				USER_MESSAGE("达到最大重传次数，中止此次传输");
				pHARQ->Reset();
				UpdateDiscardThroughput(pRxChannel);//更新丢弃数据大小
			}
			else
			{
				//记录错误包的SINR
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

		//Channel表示的是LTE Channel，所以只更新LTE Service部分的平均吞吐量
		pLTEService->SetAverageThroughput( dAverageThroughput );
	}

	void DualModeLTEHSPAUE::UpdateThroughput( const shared_ptr<Channel> pchannel )	//更新吞吐量
	{
		const size_t CRCSIZE = 24;
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pchannel->GetMCS().GetTableBlock() * pchannel->GetRBNum() - CRCSIZE;
			//To Be Modified, Type Dependent
			shared_ptr<Service> pLTEService = m_pService->GetServiceElement( SYSTEM_LTE );
			if ( pSystem->StatisticEnabled())
			{
				//To Be Modified，同时更新了Composite Service和Service Element的吞吐量
				//更新Composite Service，有可能会导致Buffer变为空，从而进行FTP Buffer的更新
				pLTEService->UpdateThroughput( dTableBlock );
				m_pServingLTENodeB.lock()->UpdateCellThroughput( dTableBlock );

				if ( pLTEService->GetIsEnd() )
					SimLog::Instance()->LogBufferEmpty( m_ID, ClockManager::Instance().GetFrameClock().GetClock(), GetType(), SYSTEM_LTE );
			}
			m_pService->SetLastPacketsize( dTableBlock );
			pLTEService->SetLastPacketsize( dTableBlock );
		}
	}

	void DualModeLTEHSPAUE::UpdateDiscardThroughput( const shared_ptr<Channel> pchannel )	//更新丢弃的数据包的吞吐量
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

	void DualModeLTEHSPAUE::UpdateReport( const SINRLIST & sinrlist, const CQILIST& cqilist )	//更新反馈
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

	//计算所有RB的SINR以及CQI并存在传进去的参数里面
	void DualModeLTEHSPAUE::MeasureSINRAndCQIofAllTheRB( SINRLIST &sinrlist, CQILIST &cqilist )
	{
		shared_ptr<NodeB> pNodeB = m_pServingLTENodeB.lock();
		//判断是否接入小区
		if (!pNodeB)
			return;

		ClockManager & clockMgr = ClockManager::Instance();
		int islot = clockMgr.GetLTESlotClock().GetClock();
		int iframe = clockMgr.GetFrameClock().GetClock();
		int iTxAntennaNum = pNodeB->GetTxAntenna()->GetAntennaPortNum();
		const int iRBNum = pNodeB->GetRBPerSlot();
		double dNodeBPowerPerRB = pNodeB->GetMaxTxPowerPerRBPerPort();											//这里的2代表的是天线的数量
		SINRCalculator &SINRcalc = SINRCalculator::GetDLCalculator();
		for (int i = 0; i <	iRBNum; ++i)
		{
			double sinrofrb = 0.0;
			for (int j = 0; j < iTxAntennaNum; ++j)
			{
				PhyResource phy(iframe, islot, j , i);
				phy.SetTxPower(dNodeBPowerPerRB);

				//发送分集的时候是双天线SINR相加
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
		
		//在这里不考虑Channel接收的延时，所以可以使用IsUsedSlot进行检测，
		//否则如果考虑延时，该检测不能够通过
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
			throw std::logic_error( "未使用该时隙" );
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
		//首先判断是新的数据传输，还是重传数据传输
		//如果是重传的数据，则需要进行C2I的合并
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

		//更新这个数据包的传输次数
		m_pService->GetServiceElement( SYSTEM_HSPA )->UpdatePacketTransTimes();

		//To Be Modified
		//在PF算法，还需要根据接收情况进行平均吞吐量的更新
		//UE根据是否传输正确，然后更新MCS门限
		if( bIsTransSuccess )
		{
			UpdateThroughput( pChannel );
			UpdateAverageThroughput( pChannel );
			pHARQ->Reset();
		}
		else
		{
			//更新这个数据包的错误传输次数
			m_pService->GetServiceElement( SYSTEM_HSPA )->UpdatePacketFailedTransTimes();
			if ( pHARQ->MaxTransTime() )
			{
				//To Be Modified，在HSPAUE中可以添加UpdateDiscardThroughput
				pHARQ->Reset();
				USER_MESSAGE("HSPA UE下行接收达到最大重传次数，中止此次传输");
			}
			else
			{
				//dC2IValue是已经原来叠加上了重传的C2I，然后直接赋值更新
				pHARQ->SetRetransC2I( dC2IValue );
				//更新传输次数
				pHARQ->UpdateTranTime();
			}
			UpdateAverageThroughput( shared_ptr<HSPAChannel>() );
		}
		//信道接受完后，无论是否传输正确，直接更新传输状态为True
		pHARQ->UpdateTransStatus( true );

		//计算出了当前的信道状况信息，首先在Reporter中将其进行存储，记录当前的信道信息，在PF算法中也需要使用
		//根据当前信道的HMatrix，生成下次传输的推荐的权重, 得到推荐的发送权值
		Matrix recommendWeightMatrix = pC2ICalc->MatrixGenerator(pChannel);
		//根据当前的Es2No进行更新推荐的MCS门限，然后进行上报
		//Type Dependent
		MCS recommendMCS = m_pHSPAAMCEntity->SelectMCS( DB(RealTodB(dEb2No)) );
		UpdateReport( dCurrentC2IValue, recommendMCS, recommendWeightMatrix );
	}

	void DualModeLTEHSPAUE::UpdateThroughput(const shared_ptr<HSPAChannel> pChannel )
	{
		//更新吞吐量统计 -- To Be Modified
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pChannel->GetMCS().GetTableBlock() * pChannel->GetCodeChannelSet().size();
			//To Be Modified, Type Dependent
			//更新吞吐量的时候，ServiceComposite和其中的HSPA Service元素都需要更新
			shared_ptr<Service> pHSPAService = m_pService->GetServiceElement( SYSTEM_HSPA );
			if ( pSystem->StatisticEnabled() )
			{
				//To Be Modified，同时更新了Composite Service和HSPA Service Element的吞吐量
				//更新Composite Service，有可能会导致Buffer变为空，从而进行FTP Buffer的更新
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
		//只更新HSPA Service那一部分的平均吞吐量，在HSPA系统中的调度时候更新
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