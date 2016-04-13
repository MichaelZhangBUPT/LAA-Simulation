#pragma once
#include "Scheduler.h"
#include <vector>
#include "commondef.h"
namespace LTESim
{
	class Scheduler;
	class NodeB;
	class Service;
	class DownlinkScheduler:public LTESim::Scheduler
	{
	public:
		typedef std::vector< shared_ptr<Service> > SERVICELIST;
		DownlinkScheduler(shared_ptr<NodeB>& pNodeB);
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
