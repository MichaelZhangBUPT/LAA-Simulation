#pragma once
#include "HARQEntity.h"

namespace LTESim
{
	class SHARQEntity :
		public LTESim::HARQEntity
	{
		//Í¬²½HARQ
	public:
		SHARQEntity(shared_ptr<IUE> pUE, SystemType systemType);
		~SHARQEntity(void);
		virtual shared_ptr <HARQ> GetHARQ();
		virtual shared_ptr<HARQ> GetNextHARQ();
		virtual void UpdateHARQProcessNo();
	};

}
