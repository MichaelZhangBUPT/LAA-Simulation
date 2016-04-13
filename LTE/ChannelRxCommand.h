#pragma once
#include "Command.h"
namespace LTESim
{
	class ILTEUE;
	class Channel;
    ///UE信道接收命令
	class ChannelRxCommand :
		public Command
	{
	public:
		///构造函数
		/*
		* @param pUE，需要执行接收的UE对象
		* @param pRxChannel，需要被接收的Channel
		*/
		ChannelRxCommand(shared_ptr<ILTEUE>& pUE, shared_ptr<Channel>& pRxChannel);
	public:
		~ChannelRxCommand(void);
	    
		///Clock::Do接口实现
		/*
		* 触发时调用pUE->ReceiveChannel(pRxChannel)
		*/
		void Do(const Clock& clk);
	protected:
		//需要接收channel的UE
		weak_ptr<ILTEUE> m_pUE;
	    
		//需要接收的Channel
		shared_ptr<Channel> m_pRxChannel;
	};
}
