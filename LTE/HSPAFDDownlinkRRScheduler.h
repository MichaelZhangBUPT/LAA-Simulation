#pragma once

#include "HSPAFDScheduler.h"

namespace LTESim
{
	class HSPANodeB;

	class HSPAFDDownlinkRRScheduler
		: public HSPAFDDownlinkScheduler
	{
	public:
		HSPAFDDownlinkRRScheduler( shared_ptr<HSPANodeB> pNodeB );
		virtual ~HSPAFDDownlinkRRScheduler();
	protected:
		virtual double CalculatePriority( shared_ptr<Service> pService, int iCodeIndex );
	};
}