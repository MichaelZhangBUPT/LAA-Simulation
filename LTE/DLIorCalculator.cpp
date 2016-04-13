#include "commondef.h"
#include "DLIorCalculator.h"

namespace LTESim
{
	DLIorCalculator::DLIorCalculator(void)
	{
	}

	DLIorCalculator::~DLIorCalculator(void)
	{
	}

	shared_ptr<DLIorCalculator> DLIorCalculator::GetIorCalculator( void )
	{
		static shared_ptr<DLIorCalculator>  calc(new DLIorCalculator);
		return calc;
	}

	double DLIorCalculator::CalcServingNodeBInterf( ILTEUE& ue, NodeB& nodeb,int slot )
	{
		throw UnFinishedFunc();
	}

}
