#include "commondef.h"
#include "ResManagerImp2.h"
#include "PhyResource.h"
namespace LTESim
{
	ResManagerImp2::ResManagerImp2(size_t _SLOTPERFRAME, size_t _RBPERSLOTPERAP, size_t antennaports)
		:SLOTPERFRAME(_SLOTPERFRAME), 
		RBPERSLOTPERAP(_RBPERSLOTPERAP), 
		ANTENNAPORTS(antennaports)
	{
	}

	ResManagerImp2::~ResManagerImp2(void)
	{
	}

	bool ResManagerImp2::Require( const shared_ptr<const PhyResource>& _phytemp ) const
	{
		//如果该帧无记录则返回true
		if (!m_resources.count(_phytemp->GetFrames()) ) 
		{
			return true;
		}
		//取相应的时隙列表
		const shared_ptr<const FRAMERES>& pFrame = m_resources.find(_phytemp->GetFrames())->second;
		const shared_ptr<const SLOTRES> &pSlot = pFrame->at(_phytemp->GetSlot());
		//如果非空值则说明被占用
		if(pSlot->at(_phytemp->GetAntennaPort() * RBPERSLOTPERAP + _phytemp->GetRB() ))
				return false;
		//未被占用返回true;
		return true;
	}
	bool ResManagerImp2::Assign( const shared_ptr< const PhyResource >& _phytemp )
	{
		if (!Require(_phytemp))
		{
			return false;
		}
		//不存在该帧的话需要创建，添加到m_resource中
		//开辟空间
		if (!m_resources.count(_phytemp->GetFrames()) ) 
		{
			shared_ptr<FRAMERES> pFrame(new FRAMERES);
			for (size_t slot = 0; slot < SLOTPERFRAME; ++slot)
			{
				shared_ptr<SLOTRES> pSlot(new SLOTRES(RBPERSLOTPERAP * ANTENNAPORTS));
				pFrame->push_back(pSlot);
			}
			m_resources.insert(make_pair(_phytemp->GetFrames(), pFrame));
		}
		//将分配的资源添加到m_resource列表中
		shared_ptr<FRAMERES> pFrame = m_resources.find(_phytemp->GetFrames())->second;
		shared_ptr<SLOTRES> pSlot = (*pFrame)[_phytemp->GetSlot()];
		(*pSlot)[RBPERSLOTPERAP * _phytemp->GetAntennaPort() +  _phytemp->GetRB()] = _phytemp;
		return true;
	}
	const shared_ptr<const PhyResource> ResManagerImp2::FindRes( int frame, int slot, int ap, int rb ) const
	{
		const static shared_ptr<const PhyResource> emptyres;
		//如果不存在，则返回空指针
		if (!m_resources.count(frame))
		{
			return emptyres;
		}
		//存在返回所寻找的资源
		shared_ptr<FRAMERES> pFrame = m_resources.find(frame)->second;
		shared_ptr<SLOTRES> pSlot = (*pFrame)[slot];
		return (*pSlot)[RBPERSLOTPERAP * ap + rb];
	}

	void ResManagerImp2::ClearBefore( int time )
	{
		m_resources.erase(m_resources.begin(), m_resources.lower_bound(time));
	}

}
