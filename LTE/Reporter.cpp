#include "commondef.h"
#include "Service.h"
#include "Reporter.h"
#include "mcs.h"
#include "DB.h"
#include "System.h"
#include "RunParameters.h"
namespace LTESim
{

	Reporter::Reporter(void)
	{
		
	}

	Reporter::~Reporter(void)
	{
	}

	ReporterImp::ReporterImp()
	{
		System::PTR pSystem;
		const RunParameters* pConfig = 0;
		if (( pSystem = System::GetCurrentSystem()) 
			&& ( pConfig = &pSystem->GetConfig())
			)
		{
			const int RBNum = pConfig->GetInt("RB_Number");
			for ( int iIndex = 0; iIndex < RBNum; ++iIndex )
			{
				m_vSINRList.push_back( 0.0 );
				m_vCQIList.push_back( MCS(0, SYSTEM_LTE) );
			}
		}
		else
		{
			throw std::runtime_error("Can't Read Config File");
		}
	}

	ReporterImp::~ReporterImp()
	{

	}

	void ReporterImp::UpdateSINR( int RBIndex, double sinr/*ʵֵ*/ )
	{
		m_vSINRList[RBIndex] = sinr;
	}

	void ReporterImp::UpdateCQI( int RBIndex, const MCS& cqi ) 
	{
		m_vCQIList[RBIndex] = cqi;
	}

	double ReporterImp::GetSINR(  int RBIndex ) const 
	{
		return m_vSINRList[RBIndex];
	}

	const ReporterImp::SINRLIST& ReporterImp::GetSINRList() const
	{
		return m_vSINRList;
	}

	const MCS& ReporterImp::GetCQI(  int RBIndex ) const
	{
		return m_vCQIList[RBIndex]; 
	}

	const ReporterImp::CQILIST& ReporterImp::GetCQIList() const
	{
		return m_vCQIList;
	}

}
