#pragma once
#include "Clock.h"
namespace LTESim
{
	class SystemClock;
	class FRAMEClock;
	class PeriodicClock;
	class WiFiClock;
	class LAAClock;

//Clock的管理类，需要使用的Clock都放在这里
class ClockManager
{
public:
    ///析构函数
    ~ClockManager(void);

	//工厂方法返回一个全局的ClockManager
	static ClockManager& Instance();
	
    //返回各种时钟的接口
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

	//返回当前时间，以ms为单位
	double GetCurrentTime() ;

	//将TimeClient对象注册到所有的clock
    void RegisterForAllClock( const shared_ptr<TimeClient> pClient) const;
    
	//将TimeClient对象注册到WiFi的Clock
	void RegisterForWiFiClock( const shared_ptr<TimeClient> pClient) const;
	
	void RegisterForLAAClock( const shared_ptr<TimeClient> pClient ) const;

	//Reset所有Clock
	void Reset() const;

protected:
	///构造函数
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
