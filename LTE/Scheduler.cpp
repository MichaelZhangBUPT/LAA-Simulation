#include "commondef.h"
#include "Scheduler.h"
namespace LTESim
{

Scheduler::Scheduler(shared_ptr<NodeB>& pNodeB)
:m_pNodeB(pNodeB)
{

}

Scheduler::~Scheduler(void)
{
}

}
