#pragma once
#include "Clock.h"
namespace LTESim
{

    ///ʱ϶ʱ�ӣ��ֱ���3������
class PeriodicClock : public LTESim::Clock
{
	friend class ClockManager;
protected:
    ///���캯��
    /*
    * @param ID, ʱ��ID
    * @param PERIOD, ��������
    */
	PeriodicClock(int ID, int PERIOD);
public:
	~PeriodicClock(void);
    
    //��õ�ǰʱ�Ӽ�����0~PERIOD
	int GetClock() const;	
protected:
	int m_iPeriod;	//��������
};

}
