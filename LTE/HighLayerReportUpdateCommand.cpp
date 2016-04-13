#include "HighLayerReportUpdateCommand.h"
#include "commondef.h"
#include "HighLayerController.h"
#include "HighLayerReport.h"

namespace LTESim
{
	HighLayerLTEReportUpdateCommand::HighLayerLTEReportUpdateCommand( shared_ptr<HighLayerController> pController, const LTENBReport& rReport )
		: m_pController( pController ), m_LTENBReport( rReport )
	{

	}

	HighLayerLTEReportUpdateCommand::~HighLayerLTEReportUpdateCommand()
	{

	}

	void HighLayerLTEReportUpdateCommand::Do( const Clock& clk )
	{
		if ( shared_ptr<HighLayerController> pController = m_pController.lock() )
		{
			pController->UpdateReport( m_LTENBReport );
		}
		else
		{
			throw std::runtime_error( "Get HighLayerController Error" );
		}
	}

	HighLayerHSPAReportUpdateCommand::HighLayerHSPAReportUpdateCommand( shared_ptr<HighLayerController> pController, const HSPANBReport& rReport )
		: m_pController( pController ), m_HSPAReport( rReport )
	{

	}

	HighLayerHSPAReportUpdateCommand::~HighLayerHSPAReportUpdateCommand()
	{

	}

	void HighLayerHSPAReportUpdateCommand::Do( const Clock& clk )
	{
		if ( shared_ptr<HighLayerController> pController = m_pController.lock() )
		{
			pController->UpdateReport( m_HSPAReport );
		}
		else
		{
			throw std::runtime_error( "Get HighLayerController Error" );
		}
	}
}