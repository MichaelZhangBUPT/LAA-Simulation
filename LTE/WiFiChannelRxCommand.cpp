#include "WiFiChannelRxCommand.h"
#include "WiFiChannel.h"

namespace LTESim
{
	WiFiChannelRxCommand::WiFiChannelRxCommand( shared_ptr<WiFiChannel> pChannel )
		: m_pChannel( pChannel )
	{

	}

	WiFiChannelRxCommand::~WiFiChannelRxCommand()
	{

	}

	void WiFiChannelRxCommand::Do( const Clock& clk )
	{
		if ( shared_ptr<IWiFiUE> pUE = m_pChannel ->GetUE() )
		{
			pUE -> ReceiveChannel( m_pChannel );
			return;
		}
		throw std::runtime_error("In WiFiChannelRxCommand::Do().\nUE is not invalid any more!");
	}

}