#include "HSPAChannelRxCommand.h"
#include "HSPAChannel.h"

namespace LTESim
{
	HSPAChannelRxCommand::HSPAChannelRxCommand( shared_ptr<HSPAChannel> pChannel )
		: m_pChannel( pChannel )
	{

	}

	HSPAChannelRxCommand::~HSPAChannelRxCommand()
	{

	}

	void HSPAChannelRxCommand::Do( const Clock& clk )
	{
		switch ( m_pChannel->GetLinkDirection() )
		{
		case UPLINK:
			throw UnFinishedFunc();
			break;
		case DOWNLINK:
			if ( shared_ptr<IHSPAUE> pUE = m_pChannel->GetUE() )
			{
				pUE->ReceiveChannel( m_pChannel );
			}
			break;
		default:
			throw UnFinishedFunc();
			break;
		}
	}

}