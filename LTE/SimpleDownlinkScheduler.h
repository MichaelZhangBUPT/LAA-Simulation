#pragma once
#include "DownLinkScheduler.h"

namespace LTESim
{
	//һ�������õļ򵥵����е���
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
