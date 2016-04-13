#include "HSPATDDownlinkRRScheduler.h"
#include "commondef.h"
#include "RandomManager.h"
#include "HSPATDScheduler.h"
#include "IUE.h"
#include "Service.h"
#include "HSPAUE.h"

namespace LTESim
{
	HSPATDDownlinkRRScheduler::HSPATDDownlinkRRScheduler( shared_ptr<HSPANodeB> pNodeB )
		: HSPATDDownlinkScheduler( pNodeB )
	{

	}

	HSPATDDownlinkRRScheduler::~HSPATDDownlinkRRScheduler()
	{

	}

	double HSPATDDownlinkRRScheduler::CalculatePriority( shared_ptr<Service> pService )
	{
		shared_ptr<IUE> pIUE = pService->GetUE();
		shared_ptr<IHSPAUE> pHSPAUE = dynamic_pointer_cast<IHSPAUE>( pIUE );
		if ( !pHSPAUE )
			throw std::runtime_error( "Convert to HSPA UE Failed" );
		UNIFORM_REAL_GEN UEPriorityGen = RandomManager::GetRandomManager().GetUniformRealGen(0.0, 1.0);	
		double dPriorityValue = UEPriorityGen();
		return dPriorityValue;
	}
}