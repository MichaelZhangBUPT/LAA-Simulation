#pragma once

#include "HSPATDScheduler.h"

namespace LTESim
{
	class HSPANodeB;

	class HSPATDDownlinkRRScheduler
		: public HSPATDDownlinkScheduler
	{
	public:
		HSPATDDownlinkRRScheduler( shared_ptr<HSPANodeB> pNodeB );
		virtual ~HSPATDDownlinkRRScheduler();
	protected:
		virtual double CalculatePriority( shared_ptr<Service> pService );
	};
}