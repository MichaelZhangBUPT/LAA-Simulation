#pragma once
#include <boost/enable_shared_from_this.hpp>
#include "commondef.h"

#include "WiFiChannel.h"
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
	class WiFiAP;

	class IWiFiUE :
		virtual public IUE
	{
	public:
		typedef boost::shared_ptr<IWiFiUE> PTR;
	public:
		IWiFiUE() {};
		virtual ~IWiFiUE() {};
	public:
		//Interface SystemElement
		//virtual const POS& GetPos() const = 0;
		//virtual int GetID() const = 0;
		//virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys ) = 0;
		virtual void AddToActiveAP(const shared_ptr<WiFiAP> pAP) = 0;
		virtual shared_ptr<WiFiAP> GetServingAP() const = 0;
		//virtual double GetNoiseFigure() const = 0;
		virtual double GetWiFiNoiseFigure() const = 0;

		//virtual void Initialization() = 0;
		virtual const int GetBackoffTime() const = 0;//��ȡ�˱�ʱ��
		virtual void BackoffTimeTick() = 0;//�˱�ʱ���ʱ
		virtual void UpdateBackoffTime() = 0;//������ײ�󣬸����˱�ʱ��
		virtual void UpdateCollisionNum() = 0;
		virtual shared_ptr<AMCEntity> GetWiFiAMCEntity() const = 0;
		virtual shared_ptr<ARQ> GetARQ() const = 0;

		virtual void ReceiveChannel( const shared_ptr<WiFiChannel> pChannel ) = 0;

	};

	class WiFiUE:
		public CMDEngine, 
		public ClockProcessor,
		public IWiFiUE,
		public boost::enable_shared_from_this<WiFiUE>,
		private SystemElement

	{
	public:
		typedef boost::shared_ptr<WiFiUE> PTR;
		typedef boost::weak_ptr<WiFiAP> APPTR;
	public:
		WiFiUE(int ueid, const POS& pos, shared_ptr<Antenna> pRxAntenna);
		virtual ~WiFiUE(void);
	public:
		//Interface SystemElement
		virtual const POS& GetPos() const;
		virtual int GetID() const;
		virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys );
		
		//WiFiUE��ʼ��
		void Initialization();
		//����һ��Service
		bool AddService(shared_ptr<Service> pService);
		shared_ptr<Service> GetService( SystemType systemType );
		shared_ptr<Antenna> GetRxAntenna() const;
		shared_ptr<WiFiAP> GetServingAP() const;
		//��ȡ����ϵ��
		double GetNoiseFigure() const;
		double GetWiFiNoiseFigure() const;
		virtual SystemType GetType() const;

		void OnClock(const Clock& clock);
		void WiFiClockProcess(const Clock& wificlock);
		virtual void FrameProcess( const Clock& FrameClock){};
		virtual void SubFrameProcess( const Clock& SubFrameClock ){};
		virtual void SlotProcess( const Clock& SlotClock ){};
		virtual void TxProcess( const Clock& txclock ){};
		virtual void RxProcess( const Clock& rxclock ){};

		shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType ){return shared_ptr<HARQ> (new HARQ);};
		shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType ){return shared_ptr<HARQ> (new HARQ);};
		void UpdateDLHARQProcess( SystemType systemType ){};

		void AddToActiveAP(const shared_ptr<WiFiAP> pAP);
		const int GetBackoffTime() const;//��ȡ�˱�ʱ��
		void BackoffTimeTick();//�˱�ʱ���ʱ
		void UpdateBackoffTime();//������ײ�󣬸����˱�ʱ��
		void UpdateCollisionNum();//������ײ�󣬸�����ײ����
		shared_ptr<AMCEntity> GetWiFiAMCEntity() const;
		void ReceiveChannel( const shared_ptr<WiFiChannel> pChannel );
		void MakeDecision( const shared_ptr<WiFiChannel> pChannel );
		void UpdateThroughput( const shared_ptr<WiFiChannel> pChannel );
		shared_ptr<ARQ> GetARQ() const;
#pragma region ��Ա����
	protected:
		APPTR					m_pServingAP;		//����AP
		shared_ptr<Service>		m_pService;			//��ǰҵ��
		shared_ptr<Antenna>		m_pRxAntenna;		//����
		int						m_iBackoffTime;		//�˱�ʱ�䣬��λus
		double					m_dNoiseFigure;		//UE����ϵ��,ʵֵ
		shared_ptr<AMCEntity>	m_pAMCEntity;		//AMCʵ��
		shared_ptr<ARQ>			m_pARQ;				//ARQ
		int						m_iCollisionNum;	//����ײ����
		int						m_iCW;				//�������ڴ�С

#pragma endregion 
	};


}