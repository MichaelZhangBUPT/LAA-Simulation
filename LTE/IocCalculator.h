#pragma once
#include "UE.h"
#include "NodeB.h"
#include "PhyResource.h"
namespace LTESim
{
	class IocCalculator
	{
	public:
		IocCalculator(void);
	public:
		virtual ~IocCalculator(void);

		//virtual double CalcOtherNodeBsInterf(const Channel &channel, int RBIndex, int RevAntennaIndex) = 0;

		virtual double CalcOtherNodeBsInterf(const NodeB & nodeb, const ILTEUE &ue, const PhyResource &rb, int RevAntennaIndex) = 0;
		//计算其他小区对用户的干扰
		//double CalcIoc(const Channel &channel, int RBIndex, int RevAntennaIndex);
		double CalcIoc(const NodeB &nodeb, const ILTEUE &ue, const PhyResource &RB, int RevAntennaIndex);
	};
}
