#pragma once
#include <boost/enable_shared_from_this.hpp>
#include "commondef.h"

#include "LAAChannel.h"
#include "Clock.h"
#include "CMDEngine.h"
//#include "POS.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
//#include "Antenna.h"
#include "ARQ.h"

#include "IUE.h"
#include "HARQ.h"

//#include "WiFiAP.h"

namespace LTESim
{
	class POS;
	class System;
	class Antenna;
	class Service;
	class LAANodeB;

	class ILAAUE :
		virtual public IUE
	{
	public:
		typedef boost::shared_ptr<ILAAUE> PTR;
	public:
		ILAAUE() {};
		virtual ~ILAAUE() {};
	public:
		//Interface SystemElement
		//virtual const POS& GetPos() const = 0;
		//virtual int GetID() const = 0;
		//virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys ) = 0;
		virtual void AddToActiveNB(const shared_ptr<LAANodeB> pNB) = 0;
		virtual shared_ptr<LAANodeB> GetServingNB() const = 0;
		//virtual double GetNoiseFigure() const = 0;
		virtual double GetLAANoiseFigure() const = 0;

		//virtual void Initialization() = 0;
		//virtual const int GetBackoffTime() const = 0;//获取退避时间
		//virtual void BackoffTimeTick() = 0;//退避时间计时
		//virtual void UpdateBackoffTime() = 0;//发生碰撞后，更新退避时间
		//virtual void UpdateCollisionNum() = 0;
		virtual shared_ptr<AMCEntity> GetLAAAMCEntity() const = 0;
		virtual shared_ptr<ARQ> GetARQ() const = 0;

		virtual void ReceiveChannel( const shared_ptr<LAAChannel> pChannel ) = 0;

	};

	class LAAUE:
		public CMDEngine, 
		public ClockProcessor,
		public ILAAUE,
		public boost::enable_shared_from_this<LAAUE>,
		private SystemElement

	{
	public:
		typedef boost::shared_ptr<LAAUE> PTR;
		typedef boost::weak_ptr<LAANodeB> NBPTR;
	public:
		LAAUE(int ueid, const POS& pos, shared_ptr<Antenna> pRxAntenna);
		virtual ~LAAUE(void);
	public:
		//Interface SystemElement
		virtual const POS& GetPos() const;
		virtual int GetID() const;
		virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys );
		
		//WiFiUE初始化
		void Initialization();
		//加入一个Service
		bool AddService(shared_ptr<Service> pService);
		shared_ptr<Service> GetService( SystemType systemType );
		shared_ptr<Antenna> GetRxAntenna() const;
		shared_ptr<LAANodeB> GetServingNB() const;
		//获取噪声系数
		double GetNoiseFigure() const;
		double GetLAANoiseFigure() const;
		virtual SystemType GetType() const;

		void OnClock(const Clock& clock);
		void LAAClockProcess(const Clock& laaclock);
		virtual void FrameProcess( const Clock& FrameClock){};
		virtual void SubFrameProcess( const Clock& SubFrameClock ){};
		virtual void SlotProcess( const Clock& SlotClock ){};
		virtual void TxProcess( const Clock& txclock ){};
		virtual void RxProcess( const Clock& rxclock ){};

		shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType ){return shared_ptr<HARQ> (new HARQ);};
		shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType ){return shared_ptr<HARQ> (new HARQ);};
		void UpdateDLHARQProcess( SystemType systemType ){};

		void AddToActiveNB(const shared_ptr<LAANodeB> pNB);
		//const int GetBackoffTime() const;//获取退避时间
		//void BackoffTimeTick();//退避时间计时
		//void UpdateBackoffTime();//发生碰撞后，更新退避时间
		//void UpdateCollisionNum();//发生碰撞后，更新碰撞次数
		shared_ptr<AMCEntity> GetLAAAMCEntity() const;
		void ReceiveChannel( const shared_ptr<LAAChannel> pChannel );
		void MakeDecision( const shared_ptr<LAAChannel> pChannel );
		void UpdateThroughput( const shared_ptr<LAAChannel> pChannel );
		shared_ptr<ARQ> GetARQ() const;
#pragma region 成员变量
	protected:
		NBPTR					m_pServingNB;		//所属AP
		shared_ptr<Service>		m_pService;			//当前业务
		shared_ptr<Antenna>		m_pRxAntenna;		//天线
		//int						m_iBackoffTime;		//退避时间，单位us
		double					m_dNoiseFigure;		//UE噪声系数,实值
		shared_ptr<AMCEntity>	m_pAMCEntity;		//AMC实体
		shared_ptr<ARQ>			m_pARQ;				//ARQ
		//int						m_iCollisionNum;	//已碰撞次数
		//int						m_iCW;				//竞争窗口大小

#pragma endregion 
	};


}