#pragma once
#include "Clock.h"
namespace LTESim
{
class Clock;

//定义用作时钟响应的基类，派生出UE和NodeB
class ClockProcessor : public TimeClient
{
public:
    ClockProcessor(void);
public:
    virtual ~ClockProcessor(void);

public:
    ///根据时钟ID调用不同的函数
    virtual void OnClock(const Clock& clock);
protected:
    ///响应FrameClock时钟的Update
    virtual void FrameProcess( const Clock& FrameClock) = 0;

	//响应slot时钟的Update
	virtual void SlotProcess( const Clock& slotClock ) = 0;

    ///响应SubFrameClock时钟的Update
    virtual void SubFrameProcess( const Clock& SubFrameClock ) = 0;

    ///响应TxClock时钟的Update，处理发送
    virtual void TxProcess( const Clock& txclock ) = 0;

    ///响应RxClock时钟的Update，处理发送
    virtual void RxProcess( const Clock& rxclock ) = 0;

};

}

