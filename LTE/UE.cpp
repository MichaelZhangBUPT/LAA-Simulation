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

	//获得UE噪声系数
	double UE::GetNoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	const std::map<int, NodeB::PTR>& UE::GetCandidateNodeBSet() const
	{
		//应当返回该UE的候选集，但是为简单返回所有
		return m_pSystem.lock()->GetNBList();
	}
	//获得当前所在的基站
	shared_ptr<NodeB> UE::GetServingNodeB() const
	{
		return m_pServingNodeB.lock();
	}

	void UE::AddToActiveSet( const shared_ptr<NodeB> pNodeB )
	{
		//现在激活集中只有一个NodeB
		NodeB::PTR TEMPPTR(m_pServingNodeB.lock());
		m_pServingNodeB = pNodeB;
		return;
	}

#pragma region 时钟响应函数族

	void LTESim::UE::OnClock( const Clock& clock )
	{ 
		//处理之前加入的命令队列
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
		//运行命令
		CMDEngine::Run(clock);
	}

	void UE::FrameProcess( const Clock& clock )
	{
	   //USER_MESSAGE("UE " << GetID() << " is processing Frame " << clock.GetClock());

		//若UE没有NodeB服务，则将UE接入小区
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

	//计算所有RB的SINR以及CQI并存在传进去的参数里面
	void UE::MeasureSINRAndCQIofAllTheRB( SINRLIST &sinrlist, CQILIST &cqilist )
	{
		shared_ptr<NodeB> pNodeB = m_pServingNodeB.lock();
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
		for (int i = 0; i < iRBNum; ++i)
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
			cqilist[ i ]  = m_AMCEntity->SelectMCS(DB(RealTodB(sinrofrb)));
		}
		if ( GetServingNodeB()->GetID() == 1 )
			SimLog::Instance()->LogSINR( GetID(), ClockManager::Instance().GetFrameClock().GetClock(), ClockManager::Instance().GetCurrentTime(), sinrlist );
	}

#pragma endregion

	//获得接收天线
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
		//UE噪声系数
		m_dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("UE_Noise_Figure(db)"));
	}

	void UE::ReceiveChannel( const shared_ptr<Channel> pRxChannel )
	{
		ClockManager& clock = ClockManager::Instance();
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
		
		//重传数据应该将上一次HARQ传输时SINR值取得处理
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
		
		//更新AMC门限
		m_AMCEntity->UpdateThreshold(FrameOK);

		//更新MCS等级
		UpdateMCSIndex(pRxChannel);	//YRM
		
		if (FrameOK)
		{
			//重置了什么？
			pHARQ->Reset();
			//判决成功更新吞吐量
			UpdateThroughput(pRxChannel);
			m_pService->SetLastTransTime( ClockManager::Instance().GetCurrentTime() );
		}
		else
		{
			//是否应当判断一下到达最大重传次数？
			//重传次数在发送端更新
			//到达最大重传次数
			if(pHARQ->MaxTransTime())
			{
				USER_MESSAGE("达到最大重传次数，中止此次传输");
				//std::cout << endl << "LTE UE:达到最大重传次数，中止此次传输" << endl;
				pHARQ->Reset();
				UpdateDiscardThroughput(pRxChannel);//更新丢弃数据大小
			}
			else
			{
				//记录错误包的SINR
				pHARQ->RecordSINROfRetranPacket( pRxChannel->GetSINRList());
				pHARQ->UpdateTranTime();																					//更新传输次数
			}
		}

		pHARQ->UpdateTransStatus(true);
	}

	bool UE::AddService( shared_ptr<Service> pService )
	{
		//如果是适合此UE的业务类型应当加入并返回true，如果不适合此UE返回false，现在假定所有UE都能承载所有业务
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


	shared_ptr<HARQ> UE::GetCurrentDLHARQ( SystemType systemType )	//获得下行HARQ
	{
		if ( systemType == SYSTEM_LTE )
			return m_DLHARQEntity->GetHARQ();
		else
			throw std::invalid_argument( "Invalid SystemType" );
	}

	shared_ptr<HARQ> UE::GetNextDLHARQ( SystemType systemType )  // 获得当前下一个HARQ进程
	{
		if ( systemType == SYSTEM_LTE )
			return m_DLHARQEntity->GetNextHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	void UE::UpdateDLHARQProcess( SystemType systemType )	//获得下行HARQ的进程
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

	void UE::UpdateThroughput( const shared_ptr<Channel> pchannel )	//更新吞吐量
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

	void UE::UpdateDiscardThroughput( const shared_ptr<Channel> pchannel )	//更新丢弃的数据包的吞吐量
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

	void UE::UpdateReport( const SINRLIST & sinrlist, const CQILIST& cqilist )	//更新反馈
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


