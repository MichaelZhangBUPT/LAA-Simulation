#pragma once
#include "UE.h"
#include "NodeB.h"
namespace LTESim
{
	class IorCalculator
	{
	public:
		IorCalculator(void);
	public:
		virtual ~IorCalculator(void);
		virtual double CalcServingNodeBInterf(ILTEUE& ue, NodeB& nodeb,int slot) = 0;
		//���㱾С�����û��ĸ��� 
	};
}

