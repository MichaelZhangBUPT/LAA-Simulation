#pragma once
#include "DLLAAScheduler.h"

namespace LTESim
{

	class RRDLLAAScheduler :
		public LTESim::RRDLLAAScheduler
	{
	public:
        typedef std::vector< shared_ptr<Service> > SERVICELIST;
		RRDLLAAScheduler(shared_ptr<LAANodeB>& pNodeB);
	public: 
		~RRDLLAAScheduler(void);
		void CalculateServicePriority(SERVICELIST& CandidateServices);	//º∆À„»®÷µ
	};

}
