#pragma once
#include <iostream>
#include "Clock.h"
namespace LTESim
{
//�յ���Ϣʱ��ʾʱ�ӵ�ID���ڲ�����ֵ����Ϊһ�����ӣ������ڷ���ʱ��ʱ����ʾ
class ClockReporter :
            public TimeClient
{
public:
    ClockReporter(void);
public:
    ~ClockReporter(void);
    /**����ӿڵ�ʵ��*/
    void OnClock(const Clock& clock);	
};

}
