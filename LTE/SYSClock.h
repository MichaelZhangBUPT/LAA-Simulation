#pragma once
#include "Clock.h"
namespace LTESim{

    ///最小单位的时钟
	class SystemClock :
		public LTESim::Clock
	{
		friend class ClockManager;
	protected:
		SystemClock(void);
	public:
		~SystemClock(void);
	};
}

