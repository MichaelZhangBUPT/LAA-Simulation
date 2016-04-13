#pragma once
#include "FDDownlinkScheduler.h"

namespace LTESim
{

	class FDRRDownlinkScheduler :
		public LTESim::FDDownlinkScheduler
	{
	public:
		FDRRDownlinkScheduler(shared_ptr<NodeB>& pNodeB);
	public: 
		~FDRRDownlinkScheduler(void);
		void CalculateServicePriority(shared_ptr<Service>pService ,int RBindex);	//º∆À„»®÷µ
	};

}
