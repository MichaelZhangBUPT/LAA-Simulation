#pragma once
#include "Clock.h"
namespace LTESim
{
	//WiFiʱ϶ʱ��
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
