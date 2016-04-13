#include "commondef.h"
#include "FTPUpdateCommand.h"

namespace LTESim
{


	FTPUpdateCommand::FTPUpdateCommand( shared_ptr<IUE>& pUE/*, shared_ptr<Service>& pService*/ )
		: m_pUE(pUE)
	{

	}

	FTPUpdateCommand::~FTPUpdateCommand( void )
	{

	}

	void FTPUpdateCommand::Do( const Clock& clk )
	{
		if (shared_ptr<IUE> pUE = m_pUE.lock())
		{
			SystemType systemType = pUE->GetType();
			shared_ptr<Service> pService = pUE->GetService( systemType );
			unsigned int Interval = static_cast<unsigned int>( pService->GetInterval() );
			pService->UpdateBuffer(Interval);
		}
	}
}