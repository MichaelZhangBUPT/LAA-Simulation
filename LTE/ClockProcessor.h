#pragma once
#include "Clock.h"
namespace LTESim
{
class Clock;

//��������ʱ����Ӧ�Ļ��࣬������UE��NodeB
class ClockProcessor : public TimeClient
{
public:
    ClockProcessor(void);
public:
    virtual ~ClockProcessor(void);

public:
    ///����ʱ��ID���ò�ͬ�ĺ���
    virtual void OnClock(const Clock& clock);
protected:
    ///��ӦFrameClockʱ�ӵ�Update
    virtual void FrameProcess( const Clock& FrameClock) = 0;

	//��Ӧslotʱ�ӵ�Update
	virtual void SlotProcess( const Clock& slotClock ) = 0;

    ///��ӦSubFrameClockʱ�ӵ�Update
    virtual void SubFrameProcess( const Clock& SubFrameClock ) = 0;

    ///��ӦTxClockʱ�ӵ�Update��������
    virtual void TxProcess( const Clock& txclock ) = 0;

    ///��ӦRxClockʱ�ӵ�Update��������
    virtual void RxProcess( const Clock& rxclock ) = 0;

};

}

