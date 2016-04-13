#pragma once
#include "TDDownlinkscheduler.h"

namespace LTESim
{

	class TDRRDownlinkScheduler :
		public LTESim::TDDownlinkScheduler
	{
	public:
        typedef std::vector< shared_ptr<Service> > SERVICELIST;
		TDRRDownlinkScheduler(shared_ptr<NodeB>& pNodeB);
	public: 
		~TDRRDownlinkScheduler(void);
		void CalculateServicePriority(SERVICELIST& CandidateServices);	//º∆À„»®÷µ
	};

}
