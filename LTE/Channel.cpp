#include "commondef.h"
#include "Channel.h"
#include <vector>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include "UE.h"
#include <algorithm>
#include "ClockManager.h"

namespace LTESim
{
	Channel::Channel(void) : m_MCSType( 0, SYSTEM_LTE )
	{
		
	}

	Channel::~Channel(void)
	{
	
	}

	
	void Channel::Initilization( shared_ptr<NodeB> nodeb, shared_ptr<ILTEUE> ue, shared_ptr<Service> pService,  shared_ptr<HARQ> pHARQ,const PhyResourceList& phyresourcelist, const MCS& mcstype )
	{
		m_Service = pService;
		m_pActiveUE = ue;
		m_pAcitiveNodeB = nodeb;
		m_PhyResourceList = phyresourcelist;
		m_MCSType = mcstype;
		m_pHARQ = pHARQ;
		//����lambda�ռ䣬ʹ��boost�ĺ��������������
		using namespace boost;
		//����resource list����ռ�õ�ʱ϶
		transform(phyresourcelist.begin(), phyresourcelist.end(),
					inserter(m_vSlotList, m_vSlotList.end()), bind(&PhyResource::GetSlot, _1));
			
	} 

	//���MCS
	const MCS& Channel::GetMCS() const
	{
		return m_MCSType;
	}

	int Channel::GetRBNum() const
	{
		int RBNum = (int) m_PhyResourceList.size();
		return RBNum / m_pAcitiveNodeB.lock()->GetTxAntenna()->GetAntennaPortNum();
	}

    int Channel::GetRBNum( int slot ) const
    {
        int RBNum = (int) m_PhyResourceList.size(); 
        return (int)( RBNum / GetSlotNum() / m_pAcitiveNodeB.lock()->GetTxAntenna()->GetAntennaPortNum() );
    }
	
    //���ʹ�ø�Channel��UE
	shared_ptr<ILTEUE> Channel::GetActiveUE() const 
	{
		return m_pActiveUE.lock();
	}

	//���ʹ�ø�Channel��NodeB
	shared_ptr<NodeB> Channel::GetActiveNodeB()const
	{
		return m_pAcitiveNodeB.lock();
	}
	//���ʹ�ø�Channel��ҵ��
	shared_ptr<Service> Channel::GetActiveService() 
	{
		return m_Service.lock();
	}

	//�����ռ�õ�ʱ϶�б�
	const std::set<int>& Channel::GetSlotList() const
	{
		return m_vSlotList;
	}

	//�жϸ�ʱ϶�Ƿ����ŵ�
	bool Channel::UsedSlot( const int slot ) const//0~SLOTPERFRAME-1
	{
		if (slot >= LTESLOTPERFRAME || slot < 0)
		{
			throw std::invalid_argument("The slot index is not right");
		}
		return m_vSlotList.count(slot) > 0;
	}

	//�����Դ�б�
	const Channel::PhyResourceList & Channel::GetPhyResourceList() const
	{
		return m_PhyResourceList;
	}

	//������һ��ʱ϶
	int Channel::GetLastSlot( void ) const
	{
		return *m_vSlotList.rbegin();
	}

	//��õ�һ��ʱ϶
	int Channel::GetFirstSlot( void ) const
	{
		return *m_vSlotList.begin();
	}

	//���ʱ϶����
	std::size_t Channel::GetSlotNum( void ) const
	{
		return m_vSlotList.size();
	}

	//Ŀǰ�����ǽ�ÿ��ʱ϶��RBһ������һ��List��Ȼ�󾭹�EESMģ��
	void Channel::UpdateSINRListofSlot( vector<double>& SINRlist, int iFrameNo, int iSlotNo )
	{
		if(GetFirstSlot() == iSlotNo)
		{
			m_vSINRListOfRBAndSlot.clear();
		}
		m_vSINRListOfRBAndSlot.insert(m_vSINRListOfRBAndSlot.end(), SINRlist.begin(), SINRlist.end());
	}

	//���SINR�б�
	vector<double>& Channel::GetSINRList()
	{
		return m_vSINRListOfRBAndSlot;
	}

	//���HARQ
	shared_ptr<HARQ> Channel::GetHARQ()
	{
		return m_pHARQ.lock();
	}
};

