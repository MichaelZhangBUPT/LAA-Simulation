#include "commondef.h"
#include "ChannelRxCommand.h"
#include "UE.h"
namespace LTESim
{
	ChannelRxCommand::ChannelRxCommand( shared_ptr<ILTEUE>& pUE/*需要执行接收的UE*/, shared_ptr<Channel>& pRxChannel/*需要接收的Channel*/ )
		:m_pUE(pUE), m_pRxChannel(pRxChannel)
	{

	}	
	ChannelRxCommand::~ChannelRxCommand(void)
	{
	
	}

	void ChannelRxCommand::Do(const Clock& clk)
	{
		//UE存在则接收Channel
		if(shared_ptr<ILTEUE> pUE = m_pUE.lock())
		{
			pUE->ReceiveChannel(m_pRxChannel);
			return;
		}
		throw std::runtime_error("In ChannelRxCommand::Do().\nUE is not invalid any more!");
	}
}
