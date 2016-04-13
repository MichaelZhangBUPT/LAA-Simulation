#pragma once

#include "commondef.h"
#include <boost/enable_shared_from_this.hpp>
#include "LAAUE.h"
#include "LAAChannel.h"

#include "Clock.h"
#include "CMDEngine.h"
#include "POS.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
#include "Antenna.h"

namespace LTESim
{
	class LAANodeB: 
		public CMDEngine, 
		public ClockProcessor, 
		public SystemElement, 
		public boost::enable_shared_from_this<LAANodeB>
	{
	public:
		typedef boost::shared_ptr<LAANodeB> PTR;
		typedef weak_ptr<ILAAUE> LAAUEPTR;
		typedef std::map<int, LAAUEPTR> LAAUELIST;
	public:
		LAANodeB(int id, const POS& pos, double maxtxpower, shared_ptr<Antenna> pAntenna);
		virtual ~LAANodeB(void);

		void OnClock(const Clock& clock);
		void LAAClockProcess(const Clock& LAAclock);
		virtual void FrameProcess( const Clock& FrameClock){};
		virtual void SubFrameProcess( const Clock& SubFrameClock ){};
		virtual void SlotProcess( const Clock& SlotClock ){};
		virtual void TxProcess( const Clock& txclock ){};
		virtual void RxProcess( const Clock& rxclock ){};

		void AddUE( const shared_ptr<ILAAUE> pUE );
		shared_ptr<Antenna> GetTxAntenna() const;
		double GetMaxTxPower() const;
		const LAAUELIST& GetUEList() const;

		void ChannelIdleProcess();
		void ChannelBackoffProcess();//HFR
		void ChannelBusyProcess();
		void ChannelCollisionProcess();
		ChannelState GetChannelState();

		void GetCandidateUEList();
		void InitChannel();
		void TransmitChannel( const shared_ptr<LAAChannel> pChannel, int iTimeDelay );
		const int GetSumCollisionNum() const;

		virtual void UpdateCellThroughput( double thr );
		virtual double GetCellThroughput();

		virtual int GetTransmitDataTime();

		//LBT退避窗口系列函数HFR
		const int GetLBTBackoffTime() const;
		void LBTBackoffTimeTick();//退避时间计时
		void UpdateLBTBackoffTime();//发生碰撞后，更新退避时间

		void LBTDeferTimeTick();//累计defer时间长度
		const int GetLBTDeferTime();//返回当前LBTdefer的时间

		void GetCCAResult();
		void GetFirstTransBackoffTime();


#pragma region 成员变量
	protected:
		LAAUELIST						m_LAAUEList;//接入此AP的WiFiUE
		LAAUELIST						m_CandidateUEList;//准备好要接收服务的UE
		shared_ptr<Antenna>				m_pTxAntenna;//业务信道发送天线	
		ChannelState                    m_ChannelState;//信道状态
		double							m_dMaxTxPower;//最大发射功率，mW
		double							m_dDataMaxLength;//一次传输最大数据，bit，已不使用
		int								m_TXOPLimit;//一次占用信道的最大时长，us
		int								m_iChannelIdleTime;//信道空闲状态的持续时间，us
		int								m_iChannelBusyTime;//信道忙碌状态持续时间, us
		int								m_iChannelScheduleTime;//调度用户剩余时间
		bool							m_bIsChannelAvailable;//是否已有信道
		bool							m_bFirstTrans;//是否是第一次传输数据
		int								m_iSumCollisionNum;//系统总共碰撞次数
		int								m_iWaitTime;//发送RTS之后的等待时间
		double							m_dCellThroughput;//小区吞吐量，实时监测系统负载情况
		int								m_iTransmitDataTime;//传输有用数据的总时间，用来计算信道利用率,us

		bool                            m_bCCA;//eNB的某一时刻的CCA测量信道是否空闲 1是，0否 HFR
		int                             m_iLBTBackoffTime;//LAA eNB的退避时间，单位us HFR
		int                             m_iLBTCWin;//LAA eNB的生成的竞争窗口大小（未用） HFR
		int                             m_iLBTDeferTime;//LBT退避中defer时间计数 HFR
		int						        m_iCW;				//竞争窗口大小
#pragma endregion 
	};


}