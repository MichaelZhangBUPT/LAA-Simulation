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
	
	//������UE��Service���������е���
	GetCadidateServiceList();
	if (m_CandidateServices.empty())
	{
		return;
	}

	//ģ��һ��RR�㷨
	
	iService %= m_CandidateServices.size();
	shared_ptr<Service> pService(m_CandidateServices[iService]);
	iService++;
	
	shared_ptr<UE> pUE = pService->GetUE();	
	if (!pUE)
	{
		throw std::logic_error("������һ����Ч��UE");
		return;
	}

	////����ѡ��Ӧ�����ȵĽ��̺�
	//pUE->UpdateDLHARQProcess();		//���½��̷�����UE��									

	//����һ��channel
	
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
	harq->UpdateTranTime();//���´������
	harq->UpdateTransStatus(false);

	//��ʱ�ڵ�ǰ��֡����
	m_DataTiming.push_back(0);
	
	m_ScheduledChannels.push_back(pChannel);

	AddScheduledChannelsToNodeB();

	Clear();


}
}

