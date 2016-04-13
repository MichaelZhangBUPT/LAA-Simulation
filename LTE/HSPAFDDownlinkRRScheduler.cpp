#include "HSPAFDDownlinkRRScheduler.h"
#include "commondef.h"
#include "HSPAFDScheduler.h"

namespace LTESim
{
	HSPAFDDownlinkRRScheduler::HSPAFDDownlinkRRScheduler( shared_ptr<HSPANodeB> pNodeB )
		: HSPAFDDownlinkScheduler( pNodeB )
	{

	}

	HSPAFDDownlinkRRScheduler::~HSPAFDDownlinkRRScheduler()
	{

	}

	double HSPAFDDownlinkRRScheduler::CalculatePriority( shared_ptr<Service> pService, int iCodeIndex )
	{
		//To Be Modified
		return 1.0;
	}
}