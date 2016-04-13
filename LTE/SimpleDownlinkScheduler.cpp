#include "commondef.h"
#include "SimpleDownlinkScheduler.h"
#include "NodeB.h"
#include "Channel.h"
#include "ClockManager.h"
#include "Service.h"
#include "ChannelMaker.h"
namespace LTESim
{


SimpleDownlinkScheduler::SimpleDownlinkScheduler( shared_ptr<NodeB>& pNodeB )
:DownLinkScheduler(pNodeB),iService(0)
{

}
SimpleDownlinkScheduler::~SimpleDownlinkScheduler(void)
{

}

void SimpleDownlinkScheduler::Process()
{
	shared_ptr<NodeB>	pNodeB = m_pNodeB.lock();
	if (!pNodeB)
	{
		return;
	}

	ClockManager& clkman = ClockManager::GetClockManager();
	
	//将所有UE的Service串起来进行调度
	GetCadidateServiceList();
	if (m_CandidateServices.empty())
	{
		return;
	}

	//模拟一个RR算法
	
	iService %= m_CandidateServices.size();
	shared_ptr<Service> pService(m_CandidateServices[iService]);
	iService++;
	
	shared_ptr<UE> pUE = pService->GetUE();	
	if (!pUE)
	{
		throw std::logic_error("调度了一个无效的UE");
		return;
	}

	////更新选择应当调度的进程号
	//pUE->UpdateDLHARQProcess();		//更新进程放在了UE端									

	//构造一个channel
	
	const MCS& mcs = pUE->GetReporter().GetCQI(0);

	shared_ptr<Channel> pChannel = ChannelMaker::GetDLSCH(pService->GetUE(), mcs, 
		pNodeB->GetMaxTxPower() / pNodeB->GetRBPerSlot() / pNodeB->GetTxAntenna()->GetAntennaPortNum(), 
		clkman.GetFrameClock().GetClock(),
		clkman.GetSlotClock().GetClock(), clkman.GetSlotClock().GetClock()+1, 0, pNodeB->GetRBPerSlot()-1);
	assert(pChannel->GetActiveService() == pService);
	assert(pService == pUE->GetService() );
	shared_ptr<HARQ> harq = pChannel->GetHARQ();
	harq->SetService(pUE->GetService());
	if (harq->GetNewPacketIndicator() )
	{
		harq->InitTrans();
	}
	harq->UpdateTranTime();//更新传输次数
	harq->UpdateTransStatus(false);

	//定时在当前子帧发送
	m_DataTiming.push_back(0);
	
	m_ScheduledChannels.push_back(pChannel);

	AddScheduledChannelsToNodeB();

	Clear();


}
}

