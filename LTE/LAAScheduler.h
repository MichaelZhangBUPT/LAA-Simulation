#pragma once
#include "commondef.h"
#include "Service.h"
namespace LTESim
{
	class LAANodeB;
	class LAAScheduler
	{
	public:
		typedef std::vector< shared_ptr<Service> > SERVICELIST;
		Scheduler(shared_ptr<LAANodeB>& pNodeB);
	public:
		virtual ~Scheduler(void);
		//ִ��һ�ε���
		//virtual void Process() = 0;
		virtual SERVICELIST Process() =0;
		virtual void Process( SERVICELIST services) =0;
	protected:
		weak_ptr<LAANodeB> m_pNodeB;//�õ�����������NodeB
	};

}
