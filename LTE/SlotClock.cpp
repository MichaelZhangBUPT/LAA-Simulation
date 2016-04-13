#include "commondef.h"
#include "SlotClock.h"
#include "ClockManager.h"
namespace LTESim
{
	PeriodicClock::PeriodicClock(int ID, int PERIOD/*ÖÜÆÚ*/):Clock(ID), m_iPeriod(PERIOD)
	{
	}

	PeriodicClock::~PeriodicClock(void)
	{
	}

	int PeriodicClock::GetClock() const
	{
		return Clock::GetInnerClock() % m_iPeriod;
	}



}
