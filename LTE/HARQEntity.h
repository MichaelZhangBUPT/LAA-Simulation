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
		virtual void UpdateHARQProcessNo() = 0;	//通过更新进程数来取得当前应该是哪一个HARQ的进程
																			//同步和异步HARQ在本仿真中将有不同的进程号更新方法
		virtual vector<shared_ptr<HARQ>>	GetHARQList();
	protected:
		vector<shared_ptr<HARQ>>			m_vHARQList;
		int													m_iCurrentProcessNo;
		int													m_iMaxProcessNum;

	};

}
