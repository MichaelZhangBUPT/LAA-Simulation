#pragma once
#include "Clock.h"
namespace LTESim
{
	//WiFiʱ϶ʱ��
	class WiFiClock : 
		public LTESim::Clock
	{
		friend class ClockManager;
	protected:
		WiFiClock();
	public:
		~WiFiClock(void);
	};

}
