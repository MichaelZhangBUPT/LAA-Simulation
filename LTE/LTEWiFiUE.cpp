#include "commondef.h"
#include "LTEWiFiUE.h"
#include "DB.h"
#include "System.h"
#include "NodeB.h"
#include "WiFiAP.h"
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
#include "ARQ.h"
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

	LTEWiFiUE::LTEWiFiUE(int iUEid, double dPosX, double dPosY, shared_ptr<Antenna> pAntenna)
		:	SystemElement( iUEid, POS( dPosX, dPosY ) ),
		m_pRxAntenna( pAntenna ),m_iCollisionNum(0),m_bNear(false)
	{

	}

	LTEWiFiUE::~LTEWiFiUE()
	{

	}

#pragma endregion

#pragma region Interface IUE

	const POS& LTEWiFiUE::GetPos() const
	{
		return SystemElement::GetPos();
	}

	int LTEWiFiUE::GetID() const
	{
		return SystemElement::GetID();
	}

	shared_ptr<System> LTEWiFiUE::Attach( shared_ptr<System> pNewSys )
	{
		return SystemElement::Attach( pNewSys );
	}

	SystemType LTEWiFiUE::GetType() const
	{
		return SYSTEM_DUAL;
	}

	//获得UE噪声系数
	double LTEWiFiUE::GetNoiseFigure() const
	{
		return m_dLTENoiseFigure;
	}

	double LTEWiFiUE::GetWiFiNoiseFigure() const
	{
		return m_dWiFiNoiseFigure;
	}

	bool LTEWiFiUE::AddService( shared_ptr<Service> pService )
	{
		//DualModeUE只能够使用ServiceComposite
		m_pService = dynamic_pointer_cast<ServiceComposite>(pService);
		if ( !m_pService )
			throw std::invalid_argument( "pService should point to a Service Composite Class" );
		return true;
	}

	shared_ptr<Service> LTEWiFiUE::GetService( SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
		case SYSTEM_WIFI:
			return m_pService->GetServiceElement( systemType );
		case SYSTEM_DUAL:
			return m_pService;
		default:
			throw std::invalid_argument( "Unknown SystemType" );
		}
	}

	shared_ptr<HARQ> LTEWiFiUE::GetCurrentDLHARQ( SystemType systemType )	//获得下行HARQ
	{
		//双模UE在LTE系统和HSPA系统中使用独立的HARQ
		if( systemType == SYSTEM_LTE )
			return m_pLTEDLHARQEntity->GetHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	shared_ptr<HARQ> LTEWiFiUE::GetNextDLHARQ( SystemType systemType )  // 获得当前下一个HARQ进程
	{
		if ( systemType == SYSTEM_LTE )
			return m_pLTEDLHARQEntity->GetNextHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	void LTEWiFiUE::UpdateDLHARQProcess( SystemType systemType )	//获得下行HARQ的进程
	{
		if ( systemType == SYSTEM_LTE )
			m_pLTEDLHARQEntity->UpdateHARQProcessNo();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	//获得接收天线
	shared_ptr<Antenna> LTEWiFiUE::GetRxAntenna() const
	{
		return m_pRxAntenna;
	}

	void LTEWiFiUE::Initialization()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		m_pLTEDLHARQEntity = shared_ptr<HARQEntity>(new ASHARQEntity( shared_from_this(), SYSTEM_LTE ) );
		m_pARQ = shared_ptr<ARQ>( new ARQ( m_pService ) );
		m_CSIReporter = shared_ptr<Reporter> (new ReporterImp);

		m_pLTEAMCEntity = shared_ptr<AMCEntity>(
			new AMCEntity(pSystem->GetAMCTable(), 
			DB(pSystem->GetConfig().GetDouble("AMC_UPSTEP_DB")),
			DB(pSystem->GetConfig().GetDouble("AMC_DOWNSTEP_DB")))
			);
		m_pWiFiAMCEntity = shared_ptr<AMCEntity>(new AMCEntity(pSystem->GetWiFiAMCTable(), 
			DB(pSystem->GetConfig().GetDouble("WiFi_AMC_UPSTEP_DB")),
			DB(pSystem->GetConfig().GetDouble("WiFi_AMC_DOWNSTEP_DB"))));
		//UE噪声系数
		m_dLTENoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("UE_Noise_Figure(db)"));
		m_dWiFiNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("WiFi_UE_Noise_Figure(dB)"));
		UpdateBackoffTime();
	}

#pragma endregion

#pragma region 时钟响应函数族

	void LTEWiFiUE::OnClock( const Clock& clock )
	{
		//处理之前加入的命令队列
		switch (clock.GetID())
		{
		case LTESUBFRAMECLOCK:
			SubFrameProcess(clock);
			break;
		case WIFICLOCK:
			WiFiClockProcess(clock);
		case HSPASUBFRAMECLOCK:
		case LTESLOTCLOCK:
		case HSPASLOTCLOCK:
		case HIGHLAYERCLOCK:
			break;
		default:
			ClockProcessor::OnClock(clock);
			break;
		}
		//运行命令
		Run(clock);
	}

	void LTEWiFiUE::WiFiClockProcess(const Clock& wificlock)
	{
		//在每一时隙的开始进行检测
		//如果未接入WiFi AP，则生成WiFi小区接入命令放置到命令列表中
		if( !m_pServingAP.lock() )
		{
			Command::PTR pCmd( new WiFiCellSearchCommand( shared_from_this() ) );
			AddCommand( pCmd );
		}
	}

	void LTEWiFiUE::FrameProcess( const Clock& clock )
	{
		USER_MESSAGE( "LTEWiFiUE " << GetID() << " LTEWiFiUE::FrameProcess. Clock ID:\t"<<clock.GetID() );
		//在每一帧的开头，检测是否接入了LTE NodeB，如果没有接入，则生成小区接入命令
		if ( !m_pServingLTENodeB.lock() )
		{
			Command::PTR pCommand( new CellSearchCommand( shared_from_this() ) );
			AddCommand( pCommand );
		}
		//if ( !m_pServingAP.lock() )
		//{
		//	Command::PTR pCommand( new WiFiCellSearchCommand( shared_from_this() ) );
		//	AddCommand( pCommand );
		//}
	}

	void LTEWiFiUE::SubFrameProcess( const Clock& clock )
	{
		USER_MESSAGE( "LTEWiFiUE " << GetID() << " LTEWiFiUE::SubFrameProcess. Clock ID:\t"<<clock.GetID() );
		if( m_pServingLTENodeB.lock()->GetIsUpdateCSI() )
			UpdateCSI( 0 );
	}


	void LTEWiFiUE::TxProcess( const Clock& clock )
	{
		USER_MESSAGE( "LTEWiFiUE " << GetID() << " LTEWiFiUE::TxProcess. Clock ID:\t"<<clock.GetID() );
	}

	void LTEWiFiUE::RxProcess( const Clock& clock )
	{
		USER_MESSAGE( "LTEWiFiUE " << GetID() << " LTEWiFiUE::RxProcess. Clock ID:\t"<<clock.GetID() );
	}

	void LTEWiFiUE::SlotProcess(const Clock& slotclock)
	{
	}


#pragma endregion

#pragma region Interface ILTEUE

	const std::map<int, NodeB::PTR>& LTEWiFiUE::GetCandidateNodeBSet() const
	{
		//应当返回该UE的候选集，但是为简单返回所有
		return m_pSystem.lock()->GetNBList();
	}

	//获得当前所在的基站
	shared_ptr<NodeB> LTEWiFiUE::GetServingNodeB() const
	{
		return m_pServingLTENodeB.lock();
	}

	void LTEWiFiUE::AddToActiveSet( const shared_ptr<NodeB> pNodeB )
	{
		//现在激活集中只有一个NodeB
		NodeB::PTR TEMPPTR(m_pServingLTENodeB.lock());
		m_pServingLTENodeB = pNodeB;
		return;
	}

	//接收的为LTE系统的信道
	void LTEWiFiUE::ReceiveChannel( const shared_ptr<Channel> pRxChannel )
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

	void LTEWiFiUE::MakeDecision( const shared_ptr<Channel> pRxChannel, int iframeno, int isubframeno)
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

	void LTEWiFiUE::GetRBListofSlot( const PhyResourceList &SourceRBlist, PhyResourceList& DesRBList, int frame, int slot )
	{
		throw UnFinishedFunc();
	}

	void LTEWiFiUE::UpdateAverageThroughput( const shared_ptr<Channel> pChannel )
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

	void LTEWiFiUE::UpdateThroughput( const shared_ptr<Channel> pchannel )	//更新吞吐量
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

	void LTEWiFiUE::UpdateDiscardThroughput( const shared_ptr<Channel> pchannel )	//更新丢弃的数据包的吞吐量
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

	void LTEWiFiUE::UpdateCSI( int delayslotnum )
	{
		if (shared_ptr <NodeB> pNodeb = this->GetServingNodeB())
		{
			SINRLIST sinrlist(pNodeb->GetRBPerSlot());
			CQILIST cqilist(pNodeb->GetRBPerSlot());
			MeasureSINRAndCQIofAllTheRB(sinrlist,cqilist);

			Clock & clock = ClockManager::Instance().GetLTESubFrameClock();
			shared_ptr <LTEWiFiUE> pUE = shared_from_this();
			shared_ptr <CMDEngine> pCmdEngine = dynamic_pointer_cast<CMDEngine>(pUE);
			shared_ptr<Command> pRxReportercommand(new ReporterRxCommand(pUE, sinrlist, cqilist));
			shared_ptr<TimedCommand> pTimecommand(new TimedCommand(clock,pCmdEngine,pRxReportercommand) );
			pTimecommand->SetTimer(delayslotnum);
			AddCommand(pTimecommand);
		}
	}

	void LTEWiFiUE::UpdateReport( const SINRLIST & sinrlist, const CQILIST& cqilist )	//更新反馈
	{
		assert(sinrlist.size() == cqilist.size());
		for (size_t i = 0; i < sinrlist.size(); ++i)
		{
			m_CSIReporter->UpdateSINR((int)i, sinrlist[i]);
			m_CSIReporter->UpdateCQI((int)i, cqilist[i]);
		}
	}

	shared_ptr<Reporter> LTEWiFiUE::GetReporter() const
	{
		return m_CSIReporter;
	}

	void LTEWiFiUE::UpdateMCSIndex( const shared_ptr<Channel> pchannel )
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
	void LTEWiFiUE::MeasureSINRAndCQIofAllTheRB( SINRLIST &sinrlist, CQILIST &cqilist )
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

#pragma region WiFiUE Interface

	void LTEWiFiUE::ReceiveChannel( const shared_ptr<WiFiChannel> pChannel )
	{
		MakeDecision( pChannel );
	}

	void LTEWiFiUE::MakeDecision( const shared_ptr<WiFiChannel> pChannel )
	{
		const Decisioner& refDecisioner = m_pSystem.lock()->GetDecisioner(SYSTEM_WIFI);
		shared_ptr<ARQ> pARQ = pChannel -> GetARQ(); 
		double dSINR = pChannel ->GetSINR();
		bool bIsTransSuccess = refDecisioner.CheckCRC( pChannel->GetMCS(), DB(RealTodB(dSINR)) );
		SimLog::Instance()->MakeDecisionCount( m_ID, ClockManager::Instance().GetFrameClock().GetClock(), GetType(), SYSTEM_WIFI, bIsTransSuccess );
		//更新AMC门限
		m_pWiFiAMCEntity->UpdateThreshold( bIsTransSuccess );
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
		UpdateBackoffTime();
	}

	void LTEWiFiUE::UpdateThroughput( const shared_ptr<WiFiChannel> pChannel )
	{
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			double dTableBlock = pChannel->GetData();
			shared_ptr<Service> pWiFiService = m_pService->GetServiceElement( SYSTEM_WIFI );
			if ( pSystem->StatisticEnabled() )
			{
				pWiFiService->UpdateThroughput( dTableBlock );
				m_pServingAP.lock()->UpdateCellThroughput(dTableBlock);

				if ( pWiFiService->GetIsEnd() )
					SimLog::Instance()->LogBufferEmpty( m_ID, ClockManager::Instance().GetWiFiClock().GetClock(), GetType(), SYSTEM_WIFI );
			}
			pWiFiService->SetLastPacketsize( dTableBlock );
			m_pService->SetLastPacketsize( dTableBlock );
		}
	}

	void LTEWiFiUE::AddToActiveAP(const shared_ptr<WiFiAP> pAP)
	{
		m_pServingAP = pAP;
	}

	shared_ptr<WiFiAP> LTEWiFiUE::GetServingAP() const
	{
		return m_pServingAP.lock();
	}

	const int LTEWiFiUE::GetBackoffTime() const
	{
		return m_iBackoffTime;
	}

	void LTEWiFiUE::BackoffTimeTick()
	{
		m_iBackoffTime -= WIFI_STANDARD_US;
	}

	void LTEWiFiUE::UpdateBackoffTime()
	{
		UNIFORM_INT_GEN BackoffTimeGen = RandomManager::GetRandomManager().GetUniformIntGen(0,pow(2.0,m_iCollisionNum+3));
		m_iBackoffTime = BackoffTimeGen() * WIFI_SLOT_US;
	}

	void LTEWiFiUE::UpdateCollisionNum()
	{
		++m_iCollisionNum;
		if ( m_iCollisionNum == 16 )
		{
			//丢包，重新生成
			shared_ptr<Service> pWiFiService = m_pService->GetServiceElement( SYSTEM_WIFI );
			pWiFiService->SetData(0);
			pWiFiService->UpdateThroughput(0);		
			m_iCollisionNum = 0;
			UpdateBackoffTime();
		}
	}

	shared_ptr<AMCEntity> LTEWiFiUE::GetWiFiAMCEntity() const
	{
		return m_pWiFiAMCEntity;
	}

	shared_ptr<ARQ> LTEWiFiUE::GetARQ() const
	{
		return m_pARQ;
	}


#pragma endregion

	void LTEWiFiUE::SetAntenna( shared_ptr<Antenna> Aptr )
	{
		m_pRxAntenna = Aptr;
	}

	double LTEWiFiUE::GetDin() const
	{
		return m_dDin;
	}

	void LTEWiFiUE::SetDin(double din)
	{
		m_dDin = din;
	}

	bool LTEWiFiUE::GetBoolNear() const
	{
		return m_bNear;
	}
	void LTEWiFiUE::SetBoolNear(bool b)
	{
		m_bNear = b;
	}


}