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
		//�����֡�޼�¼�򷵻�true
		if (!m_resources.count(_phytemp->GetFrames()) ) 
		{
			return true;
		}
		//ȡ��Ӧ��ʱ϶�б�
		const shared_ptr<const FRAMERES>& pFrame = m_resources.find(_phytemp->GetFrames())->second;
		const shared_ptr<const SLOTRES> &pSlot = pFrame->at(_phytemp->GetSlot());
		//����ǿ�ֵ��˵����ռ��
		if(pSlot->at(_phytemp->GetAntennaPort() * RBPERSLOTPERAP + _phytemp->GetRB() ))
				return false;
		//δ��ռ�÷���true;
		return true;
	}
	bool ResManagerImp2::Assign( const shared_ptr< const PhyResource >& _phytemp )
	{
		if (!Require(_phytemp))
		{
			return false;
		}
		//�����ڸ�֡�Ļ���Ҫ��������ӵ�m_resource��
		//���ٿռ�
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
		//���������Դ��ӵ�m_resource�б���
		shared_ptr<FRAMERES> pFrame = m_resources.find(_phytemp->GetFrames())->second;
		shared_ptr<SLOTRES> pSlot = (*pFrame)[_phytemp->GetSlot()];
		(*pSlot)[RBPERSLOTPERAP * _phytemp->GetAntennaPort() +  _phytemp->GetRB()] = _phytemp;
		return true;
	}
	const shared_ptr<const PhyResource> ResManagerImp2::FindRes( int frame, int slot, int ap, int rb ) const
	{
		const static shared_ptr<const PhyResource> emptyres;
		//��������ڣ��򷵻ؿ�ָ��
		if (!m_resources.count(frame))
		{
			return emptyres;
		}
		//���ڷ�����Ѱ�ҵ���Դ
		shared_ptr<FRAMERES> pFrame = m_resources.find(frame)->second;
		shared_ptr<SLOTRES> pSlot = (*pFrame)[slot];
		return (*pSlot)[RBPERSLOTPERAP * ap + rb];
	}

	void ResManagerImp2::ClearBefore( int time )
	{
		m_resources.erase(m_resources.begin(), m_resources.lower_bound(time));
	}

}
