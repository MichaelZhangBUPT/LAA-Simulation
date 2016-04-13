#pragma once
#include "commondef.h"
#include "Command.h"
#include "LAAUE.h"

namespace LTESim
{
	class LAAChannelRxCommand
		: public Command
	{
	public:
		LAAChannelRxCommand( shared_ptr<LAAChannel> pChannel );
		virtual ~LAAChannelRxCommand();
	public:
		virtual void Do( const Clock& clk );
	protected:
		shared_ptr<LAAChannel>		m_pChannel;
	};
}