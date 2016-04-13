#pragma once
#include "commondef.h"
#include "Command.h"
#include "HSPAUE.h"

namespace LTESim
{
	class HSPAChannelRxCommand
		: public Command
	{
	public:
		HSPAChannelRxCommand( shared_ptr<HSPAChannel> pChannel );
		virtual ~HSPAChannelRxCommand();
	public:
		virtual void Do( const Clock& clk );
	protected:
		shared_ptr<HSPAChannel>		m_pChannel;
	};
}