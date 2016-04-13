#include "commondef.h"
#include "RandomManager.h"
#include "TDRRDownlinkScheduler.h"
#include "NodeB.h"
#include "ClockManager.h"
#include "Service.h"
#include "UE.h"
namespace LTESim
{


	TDRRDownlinkScheduler::TDRRDownlinkScheduler( shared_ptr<NodeB>& pNodeB )
		:TDDownlinkScheduler(pNodeB)
	{

	}
	TDRRDownlinkScheduler::~TDRRDownlinkScheduler(void)
	{

	}
	void TDRRDownlinkScheduler::CalculateServicePriority( SERVICELIST& CandidateServices )
	{
		TDRRDownlinkScheduler::SERVICELIST::iterator iter=CandidateServices.begin();
		UNIFORM_REAL_GEN UEPriorityGen = RandomManager::GetRandomManager().GetUniformRealGen(0.0, 1.0);	
		for ( ; iter != CandidateServices.end(); ++iter)
		{
			double priority=UEPriorityGen();
			(*iter)->SetQoS(priority);
		}
	}	
}
