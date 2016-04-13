#pragma once
#include "DownLinkScheduler.h"

namespace LTESim
{
	//一个测试用的简单的下行调度
class SimpleDownlinkScheduler :
	public LTESim::DownLinkScheduler
{
public:
	SimpleDownlinkScheduler(shared_ptr<NodeB>& pNodeB);
public:
	~SimpleDownlinkScheduler(void);

	void Process();
private:
	int iService;
};

}
