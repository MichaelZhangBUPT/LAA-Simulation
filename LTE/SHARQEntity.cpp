#include "commondef.h"
#include "SHARQEntity.h"
namespace LTESim
{
	SHARQEntity::SHARQEntity(shared_ptr<IUE> pUE, SystemType systemType) : HARQEntity(pUE,systemType)
	{
	}

	SHARQEntity::~SHARQEntity(void)
	{
	}

	shared_ptr <HARQ> SHARQEntity::GetHARQ()	//返回当前进程的HARQ
	{
		return m_vHARQList[m_iCurrentProcessNo];
	}

	void SHARQEntity::UpdateHARQProcessNo()	//更新进程数
	{
		m_iCurrentProcessNo++;
		m_iCurrentProcessNo = m_iCurrentProcessNo% m_iMaxProcessNum;
	}

	shared_ptr<HARQ> SHARQEntity::GetNextHARQ()
	{
		int tempHARQprocessNo = m_iCurrentProcessNo;
		tempHARQprocessNo++;
		tempHARQprocessNo = tempHARQprocessNo%m_iMaxProcessNum;
		return m_vHARQList[tempHARQprocessNo];
	}
}
