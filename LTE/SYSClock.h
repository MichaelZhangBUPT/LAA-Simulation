#pragma once
#include "Clock.h"
namespace LTESim{

    ///��С��λ��ʱ��
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

