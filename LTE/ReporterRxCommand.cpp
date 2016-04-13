#include "commondef.h"
#include <algorithm>
#include "ReporterRxCommand.h"
#include "NodeB.h"
#include "UE.h"
namespace LTESim
{
	ReporterRxCommand::ReporterRxCommand( shared_ptr<ILTEUE> pUE, const SINRLIST & sinrlist, const CQILIST & cqilist ) :m_pUE(pUE),m_vSINRList(sinrlist), m_vCQIList(cqilist)
	{
		
	}
	ReporterRxCommand::~ReporterRxCommand(void)
	{
	}

	void ReporterRxCommand::Do(const Clock& clk)
	{
		//UE存在，报告更新
		if (shared_ptr<ILTEUE> pUE = m_pUE.lock())
		{
			pUE->UpdateReport(m_vSINRList, m_vCQIList);
			return;
		}
	}
}
