#pragma once
#include "HARQEntity.h"

namespace LTESim
{
	class HARQ;
	class ASHARQEntity :
		public LTESim::HARQEntity
	{
		//异步HARQ
	public:
		ASHARQEntity( shared_ptr<IUE> pUE, SystemType systemType );
	public:
		~ASHARQEntity(void);
		virtual shared_ptr <HARQ> GetHARQ();
		virtual shared_ptr<HARQ> GetNextHARQ();
		virtual void UpdateHARQProcessNo();
		double CalcPriorityOfHARQProcess(const shared_ptr<HARQ> & pHARQ);
		int	   GetMaxPriorityHARQProcessNo();//计算得到当前优先级最高的HARQ进程号
	protected:
		int m_iLoopHARQProcessNo;		//循环HARQ进程数
	};

}
