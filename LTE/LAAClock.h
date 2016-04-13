#pragma once
#include "Clock.h"
namespace LTESim
{
	//WiFi ±œ∂ ±÷”
	class LAAClock : 
		public LTESim::Clock
	{
		friend class ClockManager;
	protected:
		LAAClock();
	public:
		~LAAClock(void);
	};

}
