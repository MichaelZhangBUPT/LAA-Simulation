#include "commondef.h"
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <iterator>
#include "ARQ.h"
#include "ClockManager.h"


namespace LTESim
{
	ARQ::ARQ(shared_ptr<Service> pService)
		: m_pService(pService)
	{
		m_iTranTime = 0;
		m_iMaxTransTime = 4;
		Reset();
	}

	ARQ::~ARQ(void)
	{
	}

	void ARQ::UpdateTranTime()	
	{
		m_iTranTime++;
	}

	bool ARQ::MaxTransTime() const	
	{
		//是否为最大传输次数
		//m_iTranTime表示已经传输的次数
		//只有此时还在传输的时候才会调用MaxTransTime，传输次数+1等于一共传输的次数
		// ( 一共传输的次数 == m_iTranTime+1 == m_iMaxTransTime )则返回true
		return m_iTranTime == m_iMaxTransTime - 1;
	}

	int ARQ::GetTranTime() const	//获得传输次数
	{
		return m_iTranTime;
	}

	void ARQ::Reset()
	{
		m_iTranTime = 0;							//传输次数 注意：2表示重传了一次
		UpdateTransStatus(true);					//准备就绪
	}

	void ARQ::SetService(shared_ptr<Service> pService)
	{
		m_pService = pService;
	}

	shared_ptr<Service> ARQ::GetService()
	{
		return m_pService.lock();
	}


	bool ARQ::BeReady() const
	{
		return m_bReady;
	}

	//更新进程传输状态，调度后未反馈时为false，可以使用作为调度时为true
	void ARQ::UpdateTransStatus( bool bReady )
	{
		m_bReady = bReady;
	}

	void ARQ::SetMCS( LTESim::MCS mcs )
	{
		m_MCS = mcs;
	}

	const MCS& ARQ::GetMCS()
	{
		return m_MCS;
	}

	//新数据指示
	bool ARQ::GetNewPacketIndicator( void ) const	 
	{
		return m_iTranTime == 0;
	}

}

