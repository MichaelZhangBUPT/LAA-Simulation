#include "commondef.h"
#include "ClockManager.h"
#include "SYSClock.h"
#include "FRAMEClock.h"
#include "SlotClock.h"
#include "WiFiClock.h"
#include "LAAClock.h"

namespace LTESim
{

	ClockManager * ClockManager::m_pInstance = NULL;

	ClockManager::ClockManager(void)
	{
		m_pSystemClock = new SystemClock();
		m_pFrameClock = new FRAMEClock();
		m_pLTESubFrameClock = new PeriodicClock( LTESUBFRAMECLOCK, LTESUBFRAMEPERFRAME );
		m_pHSPASubFrameClock = new PeriodicClock( HSPASUBFRAMECLOCK, HSPASUBFRAMEPERFRAME);
		m_pLTESlotClock = new PeriodicClock(LTESLOTCLOCK, LTESLOTPERFRAME);
		m_pHSPASlotClock = new PeriodicClock(HSPASLOTCLOCK, HSPASLOTPERFRAME);
		m_pTxClock = new PeriodicClock(TXCLOCK, LTESLOTPERFRAME);
		m_pRxClock = new PeriodicClock(RXCLOCK, LTESLOTPERFRAME);
		m_pStandardClock = new PeriodicClock(STANDARDCLOCK, STANDARDTIMESLICEPERFRAME);
		m_pHighLayerClock = new Clock(HIGHLAYERCLOCK);
		m_pTestClock = new Clock(TESTCLOCK);
		m_pWiFiClock = new WiFiClock();
		m_pLAAClock = new LAAClock();
	}

	ClockManager::~ClockManager(void)
	{
		if ( m_pSystemClock )
		{
			delete m_pSystemClock;
			m_pSystemClock = NULL;
		}
		if ( m_pFrameClock )
		{
			delete m_pFrameClock;
			m_pFrameClock = NULL;
		}
		if ( m_pLTESubFrameClock )
		{
			delete m_pLTESubFrameClock;
			m_pLTESubFrameClock = NULL;
		}
		if ( m_pHSPASubFrameClock )
		{
			delete m_pHSPASubFrameClock;
			m_pHSPASubFrameClock = NULL;
		}
		if ( m_pLTESlotClock )
		{
			delete m_pLTESlotClock;
			m_pLTESlotClock = NULL;
		}
		if ( m_pHSPASlotClock )
		{
			delete m_pHSPASlotClock;
			m_pHSPASlotClock = NULL;
		}
		if ( m_pTxClock )
		{
			delete m_pTxClock;
			m_pTxClock = NULL;
		}
		if ( m_pRxClock )
		{
			delete m_pRxClock;
			m_pRxClock = NULL;
		}
		if ( m_pStandardClock )
		{
			delete m_pStandardClock;
			m_pStandardClock = NULL;
		}
		if ( m_pTestClock )
		{
			delete m_pTestClock;
			m_pTestClock = NULL;
		}
		if ( m_pHighLayerClock )
		{
			delete m_pHighLayerClock;
			m_pHighLayerClock = NULL;
		}
		if ( m_pWiFiClock )
		{
			delete m_pWiFiClock;
			m_pWiFiClock = NULL;
		}
		if ( m_pLAAClock )
		{
			delete m_pLAAClock;
			m_pLAAClock = NULL;
		}
	}
	//ϵͳʱ��
	Clock& ClockManager::GetSystemClock() const
	{
		return *m_pSystemClock;
	}
	//֡ʱ��
	Clock& ClockManager::GetFrameClock() const
	{
		return *m_pFrameClock;
	}

	//��֡ʱ�ӣ�������
	Clock& ClockManager::GetLTESubFrameClock() const
	{
		return *m_pLTESubFrameClock;
	}

	Clock& ClockManager::GetHSPASubFrameClock() const
	{
		return *m_pHSPASubFrameClock;
	}

	//LTEϵͳʱ϶ʱ�ӣ�������
	Clock& ClockManager::GetLTESlotClock() const
	{
		return *m_pLTESlotClock;
	}

	//HSPAϵͳʱ϶ʱ�ӣ�������
	Clock& ClockManager::GetHSPASlotClock() const
	{
		return *m_pHSPASlotClock;
	}

	//���Ͷ�ʱ�ӣ�����ʱ϶ʱ�Ӵ���
	Clock& ClockManager::GetTxClock() const
	{
		return *m_pTxClock;
	}
	//���ܶ�ʱ�ӣ����ӷ���ʱ�Ӵ���
	Clock& ClockManager::GetRxClock() const
	{
		return *m_pRxClock;
	}

	Clock& ClockManager::GetTestClock() const
	{
		return *m_pTestClock;
	}

	Clock& ClockManager::GetStandardClock() const
	{
		return *m_pStandardClock;
	}

	Clock& ClockManager::GetHighLayerClock() const
	{
		return *m_pHighLayerClock;
	}

	Clock& ClockManager::GetWiFiClock() const
	{
		return *m_pWiFiClock;
	}

	Clock& ClockManager::GetLAAClock() const
	{
		return *m_pLAAClock;
	}

	double ClockManager::GetCurrentTime()
	{
		//���ص�ǰʱ�䣬��λms
		return GetStandardClock().GetInnerClock() * 1.0 * FRAME_LENGTH_MS / STANDARDTIMESLICEPERFRAME;
	}

	//ʱ��ע��
	void ClockManager::RegisterForAllClock( const shared_ptr<TimeClient> pClient ) const
	{
		GetFrameClock().Register(pClient);
		GetLTESubFrameClock().Register(pClient);
		GetHSPASubFrameClock().Register(pClient);
		GetLTESlotClock().Register(pClient);
		GetHSPASlotClock().Register(pClient);
		GetTxClock().Register(pClient);
		GetRxClock().Register(pClient);
		GetTestClock().Register(pClient);
		GetHighLayerClock().Register(pClient);
		//����System Clock��Standard Clock��������ע��
	}

	void ClockManager::RegisterForWiFiClock( const shared_ptr<TimeClient> pClient ) const
	{
		GetWiFiClock().Register(pClient);
	}

	void ClockManager::RegisterForLAAClock( const shared_ptr<TimeClient> pClient ) const
	{
		GetLAAClock().Register(pClient);
	}

	//����
	void ClockManager::Reset() const
	{
		GetFrameClock().Reset();
		GetLTESubFrameClock().Reset();
		GetHSPASubFrameClock().Reset();
		GetLTESlotClock().Reset();
		GetHSPASlotClock().Reset();
		GetTxClock().Reset();
		GetRxClock().Reset();
		GetTestClock().Reset();
		GetStandardClock().Reset();
		GetSystemClock().Reset();
		GetHighLayerClock().Reset();
		GetWiFiClock().Reset();
		GetLAAClock().Reset();
	}

	ClockManager& ClockManager::Instance()
	{
		if ( m_pInstance == NULL )
			m_pInstance = new ClockManager();
		return *m_pInstance;
	}
}
