#include "LAAChannelRxCommand.h"
#include "LAAChannel.h"

namespace LTESim
{
	LAAChannelRxCommand::LAAChannelRxCommand( shared_ptr<LAAChannel> pChannel )
		: m_pChannel( pChannel )
	{

	}

	LAAChannelRxCommand::~LAAChannelRxCommand()
	{

	}

	void LAAChannelRxCommand::Do( const Clock& clk )
	{
		if ( shared_ptr<ILAAUE> pUE = m_pChannel ->GetUE() )
		{
			pUE -> ReceiveChannel( m_pChannel );
			return;
		}
		throw std::runtime_error("In LAAChannelRxCommand::Do().\nUE is not invalid any more!");
	}

}