#pragma once
#include "HARQEntity.h"

namespace LTESim
{
	class HARQ;
	class ASHARQEntity :
		public LTESim::HARQEntity
	{
		//�첽HARQ
	public:
		ASHARQEntity( shared_ptr<IUE> pUE, SystemType systemType );
	public:
		~ASHARQEntity(void);
		virtual shared_ptr <HARQ> GetHARQ();
		virtual shared_ptr<HARQ> GetNextHARQ();
		virtual void UpdateHARQProcessNo();
		double CalcPriorityOfHARQProcess(const shared_ptr<HARQ> & pHARQ);
		int	   GetMaxPriorityHARQProcessNo();//����õ���ǰ���ȼ���ߵ�HARQ���̺�
	protected:
		int m_iLoopHARQProcessNo;		//ѭ��HARQ������
	};

}
