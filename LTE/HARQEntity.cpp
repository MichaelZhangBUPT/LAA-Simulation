#include "commondef.h"
#include "HARQEntity.h"
#include "UE.h"
#include "Service.h"
#include "DLFTPService.h"
namespace LTESim
{

	HARQEntity::HARQEntity( shared_ptr<IUE> pUE, SystemType systemType )
		:m_iMaxProcessNum( 8 )
	{
		for(int i = 0; i < m_iMaxProcessNum; ++i)							//HARQ列表初始化
		{
			shared_ptr<HARQ> harq(new HARQ(pUE->GetService( systemType ),i,pUE->GetID()));
			m_vHARQList.push_back(harq);
		}
		m_iCurrentProcessNo = 0;
	}

	HARQEntity::~HARQEntity(void)
	{
	}

	vector<shared_ptr<HARQ> > HARQEntity::GetHARQList()		//获得HARQ列表
	{
		return m_vHARQList;
	}
}
