#include "HSPAUE.h"
#include "CellSearchCommand.h"
#include "Clock.h"
#include "ClockManager.h"
#include "C2ICalculator.h"
#include "HARQEntity.h"
#include "ASHARQEntity.h"
#include "Decisioner.h"
#include "DB.h"
#include "HSPAReporter.h"
#include "SimLog.h"
#include "HSPAFading.h"

namespace LTESim
{
	HSPAUE::HSPAUE( int id, const POS& pos )
		: SystemElement( id, pos )
	{

	}

	HSPAUE::~HSPAUE()
	{

	}

	void HSPAUE::OnClock(const Clock& clock)
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

	void HSPAUE::FrameProcess( const Clock& clock )
	{
		USER_MESSAGE( "HSPAUE " << GetID() << " HSPAUE::FrameProcess. Clock ID:\t"<<clock.GetID());
		//在每一帧的开始进行检测
		//如果未接入HSPANodeB，则生成HSPA小区接入命令放置到命令列表中
		if( !m_pServingNodeB.lock() )
		{
			Command::PTR pCmd( new HSPACellSearchCommand( shared_from_this() ) );
			AddCommand( pCmd );
		}
	}

	void HSPAUE::SubFrameProcess( const Clock& clock )
	{
		USER_MESSAGE( "HSPAUE " << GetID() << " HSPAUE::SubFrameProcess. Clock ID:\t"<<clock.GetID() );
		UpdateHSPACSI(0);
	}

	void HSPAUE::SlotProcess( const Clock& clock )
	{
		USER_MESSAGE( "HSPAUE " << GetID() << " HSPAUE::SlotProcess. Clock ID:\t"<<clock.GetID() );
	}

	void HSPAUE::TxProcess( const Clock& clock )
	{
		USER_MESSAGE( "HSPAUE " << GetID() << " HSPAUE::TxProcess. Clock ID:\t"<<clock.GetID() );
	}

	void HSPAUE::RxProcess( const Clock& clock )
	{
		USER_MESSAGE( "HSPAUE " << GetID() << " HSPAUE::RxProcess. Clock ID:\t"<<clock.GetID() );
	}

#pragma region	Interface SystemElement

	const POS& HSPAUE::GetPos() const
	{
		return SystemElement::GetPos();
	}

	int HSPAUE::GetID() const
	{
		return SystemElement::GetID();
	}

	shared_ptr<System> HSPAUE::Attach( shared_ptr<System> pNewSys )
	{
		return SystemElement::Attach( pNewSys );
	}

#pragma endregion

	SystemType HSPAUE::GetType() const
	{
		return SYSTEM_HSPA;
	}

	void HSPAUE::AddToActiveSet( const shared_ptr<HSPANodeB> pNodeB )
	{
		m_pServingNodeB = pNodeB;
	}

	shared_ptr<Service> HSPAUE::GetService( SystemType systemType )
	{
		//To Be Modified
		if ( systemType == SYSTEM_HSPA )
			return m_pService;
		else
			throw std::invalid_argument( "Invalid System Type in HSPA UE" );
	}

	void HSPAUE::ReceiveChannel( shared_ptr<HSPAChannel> pChannel )
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

	void HSPAUE::MakeDecision( shared_ptr<HSPAChannel> pChannel )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		C2ICalculator * pC2ICalc = C2ICalculator::Instance();
		double dCurrentC2IValue = pC2ICalc->CalculateEffectiveC2I( pChannel );
		double dC2IValue = dCurrentC2IValue;
		//SimLog::Instance()->LogC2I( pChannel->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), dC2IValue );
		//SimLog::Instance()->MakeDecisionCount( m_ID, ClockManager::Instance().GetFrameClock().GetClock(), GetType(), SYSTEM_HSPA );
		shared_ptr<HARQ> pHARQ = m_pDLHARQEntity->GetHARQ();
		//首先判断是新的数据传输，还是重传数据传输
		//如果是重传的数据，则需要进行C2I的合并
		if ( pHARQ->GetNewPacketIndicator() == false )
		{
			double dRetransC2IValue = pHARQ->GetRetransC2I();
			dC2IValue += dRetransC2IValue;
		}
		
		double dEb2No =	C2IToEb2No(dC2IValue,SF_16, pChannel->GetMCS().GetCodeGain(), pChannel->GetMCS().GetModulation());
		//if ( pChannel->GetUE()->GetServingHSPANodeB()->GetID() == 7 )
		//{
		//	SimLog::Instance()->LogC2I( pChannel->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), ClockManager::Instance().GetCurrentTime(), RealTodB( dEb2No ) );
		//}
		const Decisioner& refDecisioner = pSystem->GetDecisioner( SYSTEM_HSPA );
		bool bIsTransSuccess = refDecisioner.CheckCRC( pChannel->GetMCS(), DB(RealTodB( dEb2No )) );

		m_pAMCEntity->UpdateThreshold( bIsTransSuccess );
				
		UpdateMCSIndex(pChannel);	

		//To Be Modified
		//在PF算法，还需要根据接收情况进行平均吞吐量的更新
		//UE根据是否传输正确，然后更新MCS门限
		//bIsTransSuccess = 1;
		if( bIsTransSuccess )
		{
			UpdateThroughput( pChannel );
			UpdateAverageThroughput( pChannel );
			pHARQ->Reset();
		}
		else
		{
			if ( pHARQ->MaxTransTime() )
			{
				//To Be Modified，在HSPAUE中可以添加UpdateDiscardThroughput
				m_iMaxTransTimeNum++;
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
		//SimLog::Instance()->LogC2I( pChannel->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), dEb2No );
		MCS recommendMCS = m_pAMCEntity->SelectMCS( DB(RealTodB(dEb2No)) );
		UpdateReport( dCurrentC2IValue, recommendMCS, recommendWeightMatrix );
	}

	void HSPAUE::Initialization()
	{
		//To be added
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		m_pDLHARQEntity = shared_ptr<HARQEntity>( new ASHARQEntity( shared_from_this(), SYSTEM_HSPA ) );
		m_pAMCEntity = shared_ptr<AMCEntity>(
			new AMCEntity(
			pSystem->GetHSPAAMCTable(), 
			DB(pSystem->GetConfig().GetDouble("HSPA_AMC_UPSTEP_DB")),
			DB(pSystem->GetConfig().GetDouble("HSPA_AMC_DOWNSTEP_DB")))
			);
		m_pReporter = shared_ptr<IHSPAReporter> (new HSPAReporter( shared_from_this() ) );
		m_dNoiseFigure = dBToReal(pSystem->GetConfig().GetDouble("UE_Noise_Figure(db)"));
		m_iMaxTransTimeNum = 0;

	}

	//获取噪声系数
	double HSPAUE::GetNoiseFigure() const
	{
		return m_dNoiseFigure;
	}

	/*返回UE的天线*/
	shared_ptr<Antenna> HSPAUE::GetRxAntenna() const
	{
		return m_pAntenna;
	}

	//业务相关
	//加入一个Service
	bool HSPAUE::AddService(shared_ptr<Service> pService)
	{
		m_pService = pService;
		return true;
	}

	//HARQ
	shared_ptr<HARQ> HSPAUE::GetCurrentDLHARQ( SystemType systemType )
	{
		if ( systemType == SYSTEM_HSPA )
			return m_pDLHARQEntity->GetHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	shared_ptr<HARQ> HSPAUE::GetNextDLHARQ( SystemType systemType )
	{
		if ( systemType == SYSTEM_HSPA )
			return m_pDLHARQEntity->GetNextHARQ();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	void HSPAUE::UpdateDLHARQProcess( SystemType systemType )
	{
		if ( systemType == SYSTEM_HSPA )
			m_pDLHARQEntity->UpdateHARQProcessNo();
		else
			throw std::invalid_argument( "Invalid System Type" );
	}

	void HSPAUE::UpdateThroughput(const shared_ptr<HSPAChannel> pChannel )
	{
		//更新吞吐量统计 -- To Be Verified
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			//每一个CodeChannel的Block大小*信道中的CodeChannel的数量
			double dTableBlock = pChannel->GetMCS().GetTableBlock() * pChannel->GetCodeChannelSet().size();
			if ( pSystem->StatisticEnabled())
			{
				m_pService->UpdateThroughput( dTableBlock );

				if ( m_pService->GetIsEnd() )
					SimLog::Instance()->LogBufferEmpty( m_ID, ClockManager::Instance().GetFrameClock().GetClock(), GetType(), SYSTEM_HSPA );
			}
			m_pService->SetLastPacketsize( dTableBlock );
		}
	}

	shared_ptr<HSPANodeB> HSPAUE::GetServingHSPANodeB() const
	{
		return m_pServingNodeB.lock();
	}

	shared_ptr<IHSPAReporter> HSPAUE::GetHSPAReporter() const
	{
		return m_pReporter;
	}

	void HSPAUE::UpdateReport( double dC2IValue, const MCS& recommendMCS, const Matrix& recommendDLWeightMatrix )
	{
		m_pReporter->SetC2I( dC2IValue );
		m_pReporter->SetRecommendMCS( recommendMCS );
		m_pReporter->SetRecommendDLTransWeightMatrix( recommendDLWeightMatrix );
	}

	void HSPAUE::UpdateAverageThroughput( const shared_ptr<HSPAChannel> pChannel )
	{
		//To Be Verified
		double Tc = 1.7;
		double dTableBlock = 0.0;
		//double dBlock = 0.0;
		//int iCodeSize = 0;

		if ( pChannel )
			dTableBlock = pChannel->GetMCS().GetTableBlock() * pChannel->GetCodeChannelSet().size();
		//dBlock = pChannel->GetMCS().GetTableBlock();
		//iCodeSize =  pChannel->GetCodeChannelSet().size();}

		double dAverageThr = (1- 1/Tc)*m_pService->GetAverageThroughput() + 1/Tc * dTableBlock;
		m_pService->SetAverageThroughput( dAverageThr );

		//SimLog::Instance()->LogC2I( GetID(), ClockManager::Instance().GetFrameClock().GetClock(), dBlock, iCodeSize, dAverageThr );

	}

	void HSPAUE::SetAntenna( shared_ptr<Antenna> Aptr )
	{
		m_pAntenna = Aptr;
	}

	int HSPAUE::GetMaxTransTimeNum() const
	{
		return m_iMaxTransTimeNum;
	}

	void HSPAUE::UpdateMCSIndex( const shared_ptr<HSPAChannel> pChannel )
	{
		if( shared_ptr<System> pSystem = m_pSystem.lock() )
		{
			if ( pSystem->StatisticEnabled())
			{
				m_pService->UpdateMCSIndex( pChannel->GetMCS().ToIndex());
			}
		}
	}

	void HSPAUE::UpdateHSPACSI( int delayslotnum )
	{
		//采用分集，MRC合并；计算每根接收天线的C2I,然后相加
		C2ICalculator * pC2ICalc = C2ICalculator::Instance();
		double C2I = 0;
		shared_ptr<HSPANodeB> nodeb = m_pServingNodeB.lock();
		shared_ptr<IHSPAUE>	ue = shared_from_this();
		shared_ptr<Antenna> pTxAntenna = nodeb->GetAntenna();
		size_t TxantennaPort = pTxAntenna->GetAntennaPortNum();
		shared_ptr<Antenna> pRxAntenna = ue->GetRxAntenna();
		size_t RxantennaPort = pRxAntenna->GetAntennaPortNum();
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const double	BAND_WIDTH	= 5e6;  
		const HSPAFading& fading = pSystem->GetHSPAFading();
		//计算接收天线
		for (size_t rx = 0; rx < RxantennaPort; ++rx)
		{
			for (size_t tx = 0; tx < TxantennaPort; ++tx)
			{
				//小区内干扰功率
				double Ior = 0.0;
				//小区间干扰功率
				double NBInterf = 0.0;
				//有用信号功率
				double SelfPower = 0.0;
				System::HSPANBLIST::const_iterator iter;
				const System::HSPANBLIST  NodeBList = pSystem->GetHSPANBList();
				for (iter = NodeBList.begin(); iter != NodeBList.end(); ++iter)
				{
					const HSPANodeB& NeighborNodeB = *iter->second;
					shared_ptr<HSPANodeB> pNeighborNodeB = iter->second;
					if ( NeighborNodeB.GetID() == nodeb->GetID() )
					{
						double PropLoss_db = 0.0;
						double AntennaGain_db = 0.0;
						double fastfadingself = RealTodB( norm(fading.GetFading(pSystem->ConvertNBAbsoluteIDToOffsetID(pNeighborNodeB->GetID(), SYSTEM_HSPA), ue->GetID(), 0, tx, rx) ) );							
						double fPropLoss_dBself  = pC2ICalc->SCMPropLossDBDL(pNeighborNodeB, ue) + fastfadingself;
						double txpower = nodeb->GetTransmitPower()/TxantennaPort;
						SelfPower = txpower * dBToReal(-fPropLoss_dBself);	
					}
					else
					{	
						double fastfading = RealTodB( norm(fading.GetFading(pSystem->ConvertNBAbsoluteIDToOffsetID(pNeighborNodeB->GetID(), SYSTEM_HSPA ), ue->GetID(), 0, tx, rx) ) );
						double fPropLoss_dB  = pC2ICalc->SCMPropLossDBDL(pNeighborNodeB, ue) + fastfading;
						vector<shared_ptr<IHSPAUE>> UEtemp = pNeighborNodeB->GetScheduledUE();
						if (UEtemp.size() > 1)
						{
							throw std::runtime_error( "Error. More than one UE are scheduled" );
						}

						double TxPowerStream = pNeighborNodeB->GetTransmitPower() / double(TxantennaPort);
						if (TxPowerStream< 0.000000000001 && TxPowerStream > -0.0000000000001)
						{
							cout << "Holly crap " << endl;
						}
						if ( TxPowerStream > 0.0 )
						{						
							double RxPowerStream = TxPowerStream * dBToReal(-fPropLoss_dB);
							//Pathloss_other << ue->GetID() << "  " << dBToReal(-fPropLoss_dB) << endl;
							//SimLog::Instance()->LogTDSelectedUE( pChannel->GetUE()->GetID(), ClockManager::Instance().GetFrameClock().GetClock(), RxPowerStream );
							NBInterf += RxPowerStream;
						}

					}
				}
				double Noisepower  = GetNoiseFigure() * dBToReal(pSystem->GetConfig().GetDouble("HSPA_UE_Noise_Density") ) * BAND_WIDTH;
				//cout << ue->GetNoiseFigure() <<"  " << dBToReal(pSystem->GetConfig().GetDouble("HSPA_UE_Noise_Density") ) << endl;
				C2I += SelfPower / (Ior + NBInterf + Noisepower );

			}
		}

		double effecC2I = C2I;
		//effecC2I /= pChannel->GetC2IVector().size();
		double dCurrentC2IValue = effecC2I/16;
		Matrix recommendWeightMatrix =  InialMatrix(2, 1);
		//Type Dependent
		MCS recommendMCS(0, SYSTEM_HSPA);
		double dEb2No =	C2IToEb2No(dCurrentC2IValue,SF_16);

		recommendMCS = m_pAMCEntity->SelectMCS( DB(RealTodB(dEb2No)) );
		UpdateReport( effecC2I/*dCurrentC2IValue*/, recommendMCS, recommendWeightMatrix );
	}



}