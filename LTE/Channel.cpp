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
		//引入lambda空间，使用boost的函数对象和适配器
		using namespace boost;
		//根据resource list更新占用的时隙
		transform(phyresourcelist.begin(), phyresourcelist.end(),
					inserter(m_vSlotList, m_vSlotList.end()), bind(&PhyResource::GetSlot, _1));
			
	} 

	//获得MCS
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
	
    //获得使用该Channel的UE
	shared_ptr<ILTEUE> Channel::GetActiveUE() const 
	{
		return m_pActiveUE.lock();
	}

	//获得使用该Channel的NodeB
	shared_ptr<NodeB> Channel::GetActiveNodeB()const
	{
		return m_pAcitiveNodeB.lock();
	}
	//获得使用该Channel的业务
	shared_ptr<Service> Channel::GetActiveService() 
	{
		return m_Service.lock();
	}

	//获得所占用的时隙列表
	const std::set<int>& Channel::GetSlotList() const
	{
		return m_vSlotList;
	}

	//判断该时隙是否有信道
	bool Channel::UsedSlot( const int slot ) const//0~SLOTPERFRAME-1
	{
		if (slot >= LTESLOTPERFRAME || slot < 0)
		{
			throw std::invalid_argument("The slot index is not right");
		}
		return m_vSlotList.count(slot) > 0;
	}

	//获得资源列表
	const Channel::PhyResourceList & Channel::GetPhyResourceList() const
	{
		return m_PhyResourceList;
	}

	//获得最后一个时隙
	int Channel::GetLastSlot( void ) const
	{
		return *m_vSlotList.rbegin();
	}

	//获得第一个时隙
	int Channel::GetFirstSlot( void ) const
	{
		return *m_vSlotList.begin();
	}

	//获得时隙数量
	std::size_t Channel::GetSlotNum( void ) const
	{
		return m_vSlotList.size();
	}

	//目前做法是将每个时隙的RB一起做成一个List，然后经过EESM模型
	void Channel::UpdateSINRListofSlot( vector<double>& SINRlist, int iFrameNo, int iSlotNo )
	{
		if(GetFirstSlot() == iSlotNo)
		{
			m_vSINRListOfRBAndSlot.clear();
		}
		m_vSINRListOfRBAndSlot.insert(m_vSINRListOfRBAndSlot.end(), SINRlist.begin(), SINRlist.end());
	}

	//获得SINR列表
	vector<double>& Channel::GetSINRList()
	{
		return m_vSINRListOfRBAndSlot;
	}

	//获得HARQ
	shared_ptr<HARQ> Channel::GetHARQ()
	{
		return m_pHARQ.lock();
	}
};

