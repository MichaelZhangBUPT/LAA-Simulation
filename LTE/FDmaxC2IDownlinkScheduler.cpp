#include "commondef.h"
#include "FDmaxC2IDownlinkScheduler.h"
#include "NodeB.h"
#include "ClockManager.h"
#include "Service.h"
#include "UE.h"
namespace LTESim
{


	FDmaxC2IDownlinkScheduler::FDmaxC2IDownlinkScheduler( shared_ptr<NodeB>& pNodeB )
		:FDDownlinkScheduler(pNodeB)
	{

	}
	FDmaxC2IDownlinkScheduler::~FDmaxC2IDownlinkScheduler(void)
	{

	}
	void FDmaxC2IDownlinkScheduler::CalculateServicePriority( shared_ptr<Service>pService,int RBindex )
	{
		shared_ptr<IUE> pIUE = pService->GetUE();
		shared_ptr<ILTEUE> ue = dynamic_pointer_cast<ILTEUE>(pIUE);
		if ( ue == 0 )
			throw std::runtime_error( "不匹配的UE类型" );
		double sinr = ue->GetReporter()->GetSINR(RBindex);
		ServicePriority servicePriority;
		servicePriority.pService = pService;
		servicePriority.priority = sinr;
		servicePriority.RBindex = RBindex;
		m_servicePriorityList.push_back(servicePriority);
	}	
}
