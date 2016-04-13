#include "commondef.h"
#include "RandomManager.h"
#include "FDRRDownlinkScheduler.h"
#include "NodeB.h"
#include "ClockManager.h"
#include "Service.h"
#include "UE.h"
namespace LTESim
{


	FDRRDownlinkScheduler::FDRRDownlinkScheduler( shared_ptr<NodeB>& pNodeB )
		:FDDownlinkScheduler(pNodeB)
	{

	}
	FDRRDownlinkScheduler::~FDRRDownlinkScheduler(void)
	{

	}
	void FDRRDownlinkScheduler::CalculateServicePriority( shared_ptr<Service> pService,int RBindex )
	{
		UNIFORM_REAL_GEN UEPriorityGen = RandomManager::GetRandomManager().GetUniformRealGen(0.0, 1.0);	
		ServicePriority servicePriority;
		servicePriority.pService = pService;
		servicePriority.priority = UEPriorityGen();
		servicePriority.RBindex = RBindex;
		m_servicePriorityList.push_back(servicePriority);
	}	
}
