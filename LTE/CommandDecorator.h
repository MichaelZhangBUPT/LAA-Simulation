#pragma once
#include "commondef.h"
#include "Command.h"

namespace LTESim
{
	class CommandDecorator
		: public Command
	{
	public:
		CommandDecorator( shared_ptr<Command> pCmd );
		virtual ~CommandDecorator();
	public:
		virtual void Do( const Clock& clk );
	public:
		shared_ptr<Command>		m_pImpCommand;
	};
}