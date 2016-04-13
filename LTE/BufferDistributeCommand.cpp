#include "BufferDistributeCommand.h"
#include "DataDistributeScheme.h"
#include "ServiceComposite.h"


namespace LTESim
{

	DataDistributeCommand::DataDistributeCommand( shared_ptr<ServiceComposite> pServiceComposite, DataDistributeScheme& rScheme )
		: m_pServiceComposite( pServiceComposite ), m_DataDistributeScheme( rScheme )
	{
		
	}

	DataDistributeCommand::~DataDistributeCommand()
	{

	}

	void DataDistributeCommand::Do( const Clock& clk )
	{
		if ( shared_ptr<ServiceComposite> pServiceComposite = m_pServiceComposite.lock() )
		{
			pServiceComposite->DistributeBufferData( m_DataDistributeScheme );
		}
		else
		{
			throw std::runtime_error( "Get the master ServiceComposite Error" );
		}
	}

}