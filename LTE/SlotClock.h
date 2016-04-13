#pragma once
#include "Clock.h"
namespace LTESim
{

    ///时隙时钟，分别有3个对象
class PeriodicClock : public LTESim::Clock
{
	friend class ClockManager;
protected:
    ///构造函数
    /*
    * @param ID, 时钟ID
    * @param PERIOD, 归零周期
    */
	PeriodicClock(int ID, int PERIOD);
public:
	~PeriodicClock(void);
    
    //获得当前时钟计数，0~PERIOD
	int GetClock() const;	
protected:
	int m_iPeriod;	//归零周期
};

}
