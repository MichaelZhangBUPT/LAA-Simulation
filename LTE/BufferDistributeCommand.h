#pragma once

#include "commondef.h"
#include "Command.h"
#include "DataDistributeScheme.h"

/*
∑÷≈‰∑Ω∞∏
*/

namespace LTESim
{
	class ServiceComposite;

	class DataDistributeCommand : public Command
	{
	public:
		DataDistributeCommand( shared_ptr<ServiceComposite> pServiceComposite, DataDistributeScheme& rScheme  );
		virtual ~DataDistributeCommand();
	public:
		virtual void Do( const Clock& clk );
	protected:
		weak_ptr<ServiceComposite>	m_pServiceComposite;
		DataDistributeScheme				m_DataDistributeScheme;
	};
}