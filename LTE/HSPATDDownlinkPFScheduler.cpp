#include "HSPATDDownlinkPFScheduler.h"
#include "commondef.h"
#include "HSPATDScheduler.h"
#include "IUE.h"
#include "Service.h"
#include "HSPAUE.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "ClockManager.h"
#include "System.h"

namespace LTESim
{
	HSPATDDownlinkPFScheduler::HSPATDDownlinkPFScheduler( shared_ptr<HSPANodeB> pNodeB )
		: HSPATDDownlinkScheduler( pNodeB )
	{

	}

	HSPATDDownlinkPFScheduler::~HSPATDDownlinkPFScheduler()
	{

	}

	double HSPATDDownlinkPFScheduler::CalculatePriority( shared_ptr<Service> pService )
	{
		shared_ptr<IUE> pIUE = pService->GetUE();
		//shared_ptr<System> pSystem = System::GetCurrentSystem();
		shared_ptr<IHSPAUE> pHSPAUE = dynamic_pointer_cast<IHSPAUE>( pIUE );
		if ( !pHSPAUE )
			throw std::runtime_error( "Convert to HSPA UE Failed" );
		shared_ptr<IHSPAReporter> pHSPAReporter = pHSPAUE->GetHSPAReporter();
		double dPriorityValue = pHSPAReporter->GetC2I() / pService->GetAverageThroughput();

		//m_C2I.push_back(pHSPAReporter->GetC2I()); 
		//m_AverThr.push_back(pService->GetAverageThroughput());
		//pSystem->m_fout<< ClockManager::Instance().GetFrameClock().GetClock() <<"   "<< ClockManager::Instance().GetHSPASubFrameClock().GetClock() << "   "
		//	<<pIUE->GetID()<<"    "<<pHSPAReporter->GetC2I()<<"    "<<pService->GetAverageThroughput()<<endl;
		return dPriorityValue;
	}
}
