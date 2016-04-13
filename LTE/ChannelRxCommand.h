#pragma once
#include "Command.h"
namespace LTESim
{
	class ILTEUE;
	class Channel;
    ///UE�ŵ���������
	class ChannelRxCommand :
		public Command
	{
	public:
		///���캯��
		/*
		* @param pUE����Ҫִ�н��յ�UE����
		* @param pRxChannel����Ҫ�����յ�Channel
		*/
		ChannelRxCommand(shared_ptr<ILTEUE>& pUE, shared_ptr<Channel>& pRxChannel);
	public:
		~ChannelRxCommand(void);
	    
		///Clock::Do�ӿ�ʵ��
		/*
		* ����ʱ����pUE->ReceiveChannel(pRxChannel)
		*/
		void Do(const Clock& clk);
	protected:
		//��Ҫ����channel��UE
		weak_ptr<ILTEUE> m_pUE;
	    
		//��Ҫ���յ�Channel
		shared_ptr<Channel> m_pRxChannel;
	};
}
