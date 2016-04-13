#pragma once
#include "IorCalculator.h"
#include "UE.h"
#include "NodeB.h"
namespace LTESim
{
	//小区内干扰计算
	class DLIorCalculator :
		public IorCalculator
	{
	public:
		DLIorCalculator(void);
	public:
		~DLIorCalculator(void);
		virtual double CalcServingNodeBInterf(ILTEUE& ue, NodeB& nodeb,int slot);	//计算小区内干扰
		static shared_ptr<DLIorCalculator> GetIorCalculator(void);
	};
}

