#include "commondef.h"
#include "IocCalculator.h"

namespace LTESim
{
	IocCalculator::IocCalculator(void)
	{

	}

	IocCalculator::~IocCalculator(void)
	{

	}

	double IocCalculator::CalcIoc(const NodeB &nodeb, const ILTEUE &ue, const PhyResource &RB, int RevAntennaIndex )
	{
		return CalcOtherNodeBsInterf(nodeb,ue,RB, RevAntennaIndex);
	}
}

