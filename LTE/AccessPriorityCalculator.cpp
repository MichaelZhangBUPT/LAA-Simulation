#include "commondef.h"
#include "AccessPriorityCalculator.h"
namespace LTESim
{
	AccessPriorityCalculator::AccessPriorityCalculator( const UEPTR& pUE )
        :m_pLTEUE( pUE )
	{

	}

	AccessPriorityCalculator::AccessPriorityCalculator(const HSPAUEPTR& pUE)
		: m_pHSPAUE( pUE )
	{

	}
	
	AccessPriorityCalculator::AccessPriorityCalculator(const WiFiUEPTR& pUE)
		: m_pWiFiUE( pUE )
	{

	}

	AccessPriorityCalculator::AccessPriorityCalculator(const LAAUEPTR& pUE)
		: m_pLAAUE( pUE )
	{

	}

	AccessPriorityCalculator::~AccessPriorityCalculator(void)
	{

	}

}
