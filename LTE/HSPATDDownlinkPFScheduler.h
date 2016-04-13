#pragma once

#include "HSPATDScheduler.h"

namespace LTESim
{
	class HSPANodeB;

	class HSPATDDownlinkPFScheduler
		: public HSPATDDownlinkScheduler
	{
	public:
		HSPATDDownlinkPFScheduler( shared_ptr<HSPANodeB> pNodeB );
		virtual ~HSPATDDownlinkPFScheduler();
	protected:
		virtual double CalculatePriority( shared_ptr<Service> pService );
	};
}