#pragma once
#include "LAAScheduler.h"
#include <vector>
#include "commondef.h"
namespace LTESim
{
	class LAAScheduler;
	class LAANodeB;
	class Service;
	class DLLAAScheduler:public LTESim::LAAScheduler
	{
	public:
		typedef std::vector< shared_ptr<Service> > SERVICELIST;
		DownlinkScheduler(shared_ptr<LAANodeB>& pNodeB);
	public:
		virtual ~DownlinkScheduler(void);
		//执行一次调度
		//virtual void Process() {};
		virtual SERVICELIST Process() {return m_CandidateServices;};
		virtual void Process( SERVICELIST services) {};
	protected:
        SERVICELIST m_CandidateServices;
	};

}
