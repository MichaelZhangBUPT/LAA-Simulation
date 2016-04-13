#pragma once
#include "Clock.h"
namespace LTESim
{
	class SystemClock;
	class FRAMEClock;
	class PeriodicClock;
	class WiFiClock;
	class LAAClock;

//Clock�Ĺ����࣬��Ҫʹ�õ�Clock����������
class ClockManager
{
public:
    ///��������
    ~ClockManager(void);

	//������������һ��ȫ�ֵ�ClockManager
	static ClockManager& Instance();
	
    //���ظ���ʱ�ӵĽӿ�
    Clock& GetSystemClock() const;
    Clock& GetFrameClock() const;
    Clock& GetLTESubFrameClock() const;
	Clock& GetHSPASubFrameClock() const;
    Clock& GetLTESlotClock() const;
	Clock& GetHSPASlotClock() const;
    Clock& GetTxClock() const;
    Clock& GetRxClock() const;
    Clock& GetTestClock() const;
	Clock& GetStandardClock() const;
	Clock& GetHighLayerClock() const;
	Clock& GetWiFiClock() const;
	Clock& GetLAAClock() const;

	//���ص�ǰʱ�䣬��msΪ��λ
	double GetCurrentTime() ;

	//��TimeClient����ע�ᵽ���е�clock
    void RegisterForAllClock( const shared_ptr<TimeClient> pClient) const;
    
	//��TimeClient����ע�ᵽWiFi��Clock
	void RegisterForWiFiClock( const shared_ptr<TimeClient> pClient) const;
	
	void RegisterForLAAClock( const shared_ptr<TimeClient> pClient ) const;

	//Reset����Clock
	void Reset() const;

protected:
	///���캯��
	ClockManager(void);

private:
	static ClockManager * m_pInstance;
	FRAMEClock * m_pFrameClock;
	PeriodicClock * m_pLTESubFrameClock;
	PeriodicClock * m_pHSPASubFrameClock;
	PeriodicClock * m_pLTESlotClock;
	PeriodicClock * m_pHSPASlotClock;
	PeriodicClock * m_pTxClock;
	PeriodicClock * m_pRxClock;
	PeriodicClock * m_pStandardClock;
	SystemClock * m_pSystemClock;
	Clock * m_pHighLayerClock;
	Clock * m_pTestClock;
	WiFiClock * m_pWiFiClock;
	LAAClock * m_pLAAClock;
};

}
