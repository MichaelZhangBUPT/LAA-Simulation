#pragma once
#include "TDDownlinkscheduler.h"

namespace LTESim
{

	class TDPFDownlinkScheduler :
		public LTESim::TDDownlinkScheduler
	{
	public:
		TDPFDownlinkScheduler(shared_ptr<NodeB>& pNodeB);
	public: 
		~TDPFDownlinkScheduler(void);
		void CalculateServicePriority(SERVICELIST& CandidateServices);	//º∆À„»®÷µ
	};

}
