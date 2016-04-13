#include "commondef.h"
#include "DownlinkScheduler.h"
namespace LTESim
{

	DownlinkScheduler::DownlinkScheduler(shared_ptr<NodeB>& pNodeB)
		:Scheduler(pNodeB)
	{

	}

	DownlinkScheduler::~DownlinkScheduler(void)
	{
	}

}
