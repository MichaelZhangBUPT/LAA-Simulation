#pragma once
#include "Clock.h"
namespace LTESim{
    ///Frame时钟对象，每帧的开头触发一次
	class FRAMEClock :
		public LTESim::Clock
	{
	protected:
		FRAMEClock(void);
		friend class ClockManager;
	public:
		~FRAMEClock(void);
	};
}

