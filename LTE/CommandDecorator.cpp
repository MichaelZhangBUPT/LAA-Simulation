#include "commondef.h"
#include "CommandDecorator.h"
#include "Command.h"

namespace LTESim
{
	CommandDecorator::CommandDecorator( shared_ptr<Command> pCmd )
		: m_pImpCommand( pCmd )
	{

	}

	CommandDecorator::~CommandDecorator()
	{

	}

	void CommandDecorator::Do( const Clock& clk )
	{
		m_pImpCommand->Do( clk );
	}
}