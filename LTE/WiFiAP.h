#pragma once

//#include <map>
//#include <set>
#include "commondef.h"
#include <boost/enable_shared_from_this.hpp>
#include "WiFiUE.h"
#include "WiFiChannel.h"

#include "Clock.h"
#include "CMDEngine.h"
#include "POS.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
#include "Antenna.h"



namespace LTESim
{
	class WiFiAP : 
		public CMDEngine, 
		public ClockProcessor, 
		public SystemElement, 
		public boost::enable_shared_from_this<WiFiAP>  //对一个类 A ，当我们希望使用 shared_ptr 来管理其类对象时，
		                                               //而且需要在自己定义的函数里把类对象 shared_ptr 传给其他函数时，
													   //可以让类 A 从 enable_shared_from_this 继承
	{
	public:
		typedef boost::shared_ptr<WiFiAP> PTR;
		typedef weak_ptr<IWiFiUE> WiFiUEPTR;
		typedef std::map<int, WiFiUEPTR> WiFiUELIST;
	public:
		WiFiAP(int id, const POS& pos, double maxtxpower, shared_ptr<Antenna> pAntenna);
		virtual ~WiFiAP(void);

		void OnClock(const Clock& clock);
		void WiFiClockProcess(const Clock& wificlock);
		virtual void FrameProcess( const Clock& FrameClock){};
		virtual void SubFrameProcess( const Clock& SubFrameClock ){};
		virtual void SlotProcess( const Clock& SlotClock ){};
		virtual void TxProcess( const Clock& txclock ){};
		virtual void RxProcess( const Clock& rxclock ){};

		void AddUE( const shared_ptr<IWiFiUE> pUE );
		shared_ptr<Antenna> GetTxAntenna() const;
		double GetMaxTxPower() const;
		const WiFiUELIST& GetUEList() const;

		void ChannelIdleProcess();
		void ChannelBusyProcess();
		void ChannelCollisionProcess();

		void GetCandidateUEList();
		void InitChannel();
		void TransmitChannel( const shared_ptr<WiFiChannel> pChannel, int iTimeDelay );
		const int GetSumCollisionNum() const;

		virtual void UpdateCellThroughput( double thr );
		virtual double GetCellThroughput();

		virtual int GetTransmitDataTime();
		ChannelState GetChannelState();


#pragma region 成员变量
	protected:
		//WiFiUE							m_APItself;	//AP本身，用于传输下行业务
		WiFiUELIST						m_WiFiUEList;//接入此AP的WiFiUE
		WiFiUELIST						m_CandidateUEList;//准备好要接收服务的UE
		shared_ptr<Antenna>				m_pTxAntenna;//业务信道发送天线
		ChannelState				m_ChannelState;//信道状态
		double							m_dMaxTxPower;//最大发射功率，mW
		double							m_dDataMaxLength;//一次传输最大数据，bit，已不使用
		int								m_TXOPLimit;//一次占用信道的最大时长，us
		int								m_iChannelIdleTime;//信道空闲状态的持续时间，us
		int								m_iChannelBusyTime;//信道忙碌状态的剩余时间, us
		bool							m_bIsChannelAvailable;//是否已有信道
		bool							m_bFirstTrans;//是否是第一次传输数据
		int								m_iSumCollisionNum;//系统总共碰撞次数
		int								m_iWaitTime;//发送RTS之后的等待时间
		double							m_dCellThroughput;//小区吞吐量，实时监测系统负载情况
		int								m_iTransmitDataTime;//传输有用数据的总时间，用来计算信道利用率,us
	    bool                            cs;
#pragma endregion 
	};


}