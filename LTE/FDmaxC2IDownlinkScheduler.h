#pragma once
#include "FDDownlinkScheduler.h"

namespace LTESim
{

	class FDmaxC2IDownlinkScheduler :
		public LTESim::FDDownlinkScheduler
	{
	public:
		FDmaxC2IDownlinkScheduler(shared_ptr<NodeB>& pNodeB);
	public: 
		~FDmaxC2IDownlinkScheduler(void);
		void CalculateServicePriority(shared_ptr<Service>pService ,int RBindex);	//º∆À„»®÷µ
	};

}
