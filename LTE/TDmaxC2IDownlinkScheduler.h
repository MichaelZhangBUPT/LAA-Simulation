#pragma once
#include "TDDownlinkscheduler.h"

namespace LTESim
{

	class TDmaxC2IDownlinkScheduler :
		public LTESim::TDDownlinkScheduler
	{
	public:
		TDmaxC2IDownlinkScheduler(shared_ptr<NodeB>& pNodeB);
	public: 
		~TDmaxC2IDownlinkScheduler(void);
		void CalculateServicePriority(SERVICELIST& CandidateServices);	//º∆À„»®÷µ
	};

}
