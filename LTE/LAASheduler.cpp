#include "commondef.h"
#include "LAAScheduler.h"
namespace LTESim
{

Scheduler::Scheduler(shared_ptr<LAANodeB>& pNodeB)
:m_pNodeB(pNodeB)
{

}

Scheduler::~Scheduler(void)
{
}

}
