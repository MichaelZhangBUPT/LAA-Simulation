#pragma once

#include "commondef.h"
#include "Command.h"
#include "HighLayerReport.h"

namespace LTESim
{
	class HighLayerController;

	class HighLayerLTEReportUpdateCommand
		: public Command
	{
	public:
		HighLayerLTEReportUpdateCommand( shared_ptr<HighLayerController> pController, const LTENBReport& rReport );
		virtual ~HighLayerLTEReportUpdateCommand();
	public:
		virtual void Do( const Clock& clk );
	private:
		weak_ptr<HighLayerController>	m_pController;
		LTENBReport								m_LTENBReport;
	};

	class HighLayerHSPAReportUpdateCommand
		: public Command
	{
	public:
		HighLayerHSPAReportUpdateCommand( shared_ptr<HighLayerController> pController, const HSPANBReport& rReport );
		virtual ~HighLayerHSPAReportUpdateCommand();
	public:
		virtual void Do( const Clock& clk );
	private:
		weak_ptr<HighLayerController>	m_pController;
		HSPANBReport							m_HSPAReport;
	};

}