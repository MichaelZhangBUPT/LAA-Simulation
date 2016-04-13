#pragma once
#include <iostream>
#include "Clock.h"
namespace LTESim
{
//收到消息时显示时钟的ID和内部计数值，作为一个例子，可用于仿真时的时间显示
class ClockReporter :
            public TimeClient
{
public:
    ClockReporter(void);
public:
    ~ClockReporter(void);
    /**基类接口的实现*/
    void OnClock(const Clock& clock);	
};

}
