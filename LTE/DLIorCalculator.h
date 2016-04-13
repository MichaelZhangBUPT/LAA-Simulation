#pragma once
#include "IorCalculator.h"
#include "UE.h"
#include "NodeB.h"
namespace LTESim
{
	//С���ڸ��ż���
	class DLIorCalculator :
		public IorCalculator
	{
	public:
		DLIorCalculator(void);
	public:
		~DLIorCalculator(void);
		virtual double CalcServingNodeBInterf(ILTEUE& ue, NodeB& nodeb,int slot);	//����С���ڸ���
		static shared_ptr<DLIorCalculator> GetIorCalculator(void);
	};
}

