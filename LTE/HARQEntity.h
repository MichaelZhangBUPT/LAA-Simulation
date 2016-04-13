#pragma once

namespace LTESim
{
	class HARQ;
	class ILTEUE;
	class IUE;

	class HARQEntity
	{
	public:
	public:
		HARQEntity(shared_ptr<IUE> pUE, SystemType systemType );
		virtual ~HARQEntity(void);
		virtual shared_ptr <HARQ>	GetHARQ() = 0;
		virtual shared_ptr<HARQ>	GetNextHARQ() = 0;
		virtual void UpdateHARQProcessNo() = 0;	//ͨ�����½�������ȡ�õ�ǰӦ������һ��HARQ�Ľ���
																			//ͬ�����첽HARQ�ڱ������н��в�ͬ�Ľ��̺Ÿ��·���
		virtual vector<shared_ptr<HARQ>>	GetHARQList();
	protected:
		vector<shared_ptr<HARQ>>			m_vHARQList;
		int													m_iCurrentProcessNo;
		int													m_iMaxProcessNum;

	};

}
