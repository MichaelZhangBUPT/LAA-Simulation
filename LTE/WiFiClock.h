#pragma once
#include "Clock.h"
namespace LTESim
{
	//WiFi ±œ∂ ±÷”
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
