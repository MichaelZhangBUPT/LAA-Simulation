#pragma once
#include "Clock.h"
namespace LTESim{
    ///Frameʱ�Ӷ���ÿ֡�Ŀ�ͷ����һ��
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

