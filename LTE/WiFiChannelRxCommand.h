#pragma once
#include "commondef.h"
#include "Command.h"
#include "WiFiUE.h"

namespace LTESim
{
	class WiFiChannelRxCommand
		: public Command
	{
	public:
		WiFiChannelRxCommand( shared_ptr<WiFiChannel> pChannel );
		virtual ~WiFiChannelRxCommand();
	public:
		virtual void Do( const Clock& clk );
	protected:
		shared_ptr<WiFiChannel>		m_pChannel;
	};
}