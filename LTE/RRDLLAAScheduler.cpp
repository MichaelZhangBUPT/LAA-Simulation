#include "commondef.h"
#include "RandomManager.h"
#include "RRDLLAAScheduler.h"
#include "LAANodeB.h"
#include "ClockManager.h"
#include "Service.h"
#include "LAAUE.h"
namespace LTESim
{


	RRDLLAAScheduler::RRDLLAAScheduler( shared_ptr<LAANodeB>& pNodeB )
		:RRDLLAAScheduler(pNodeB)
	{

	}
	RRDLLAAScheduler::~RRDLLAAScheduler(void)
	{

	}
	void RRDLLAAScheduler::CalculateServicePriority( SERVICELIST& CandidateServices )
	{
		RRDLLAAScheduler::SERVICELIST::iterator iter=CandidateServices.begin();
		UNIFORM_REAL_GEN UEPriorityGen = RandomManager::GetRandomManager().GetUniformRealGen(0.0, 1.0);	
		for ( ; iter != CandidateServices.end(); ++iter)
		{
			double priority=UEPriorityGen();
			(*iter)->SetQoS(priority);
		}
	}	
}
