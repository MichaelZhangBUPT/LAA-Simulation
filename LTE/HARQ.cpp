#include "commondef.h"
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <iterator>
#include "HARQ.h"
#include "ClockManager.h"


namespace LTESim
{
	HARQ::HARQ(shared_ptr<Service> pService, int processno, int ueid )
		: m_pService(pService), m_iProcessNo(processno), m_UEID(ueid), m_MCS( 0, SYSTEM_LTE )
	{
		m_iTranTime = 0;
		m_iMaxTransTime = 4;
		Reset();
	}

	HARQ::~HARQ(void)
	{
	}

	void HARQ::SetMCS( LTESim::MCS mcs )
	{
		m_MCS = mcs;
	}

	void HARQ::InitTrans()	//获得初始传输时刻
	{
		//To Be Modified
		m_InitialTransClock = ClockManager::Instance().GetLTESlotClock().GetClock();
	}

	void HARQ::RecordSINROfRetranPacket( const vector<double>& vSINRList )	//记录重传数据的SINR列表
	{
		using namespace boost::lambda;
		m_RetranSINRList.clear();	//后添加的，因为这时参数中的SINR已经是求和过了，下面else中的求和是多余的
		if (m_RetranSINRList.empty())
		{
			 m_RetranSINRList = vSINRList;
		}
		else
		{
			std::transform(vSINRList.begin(), vSINRList.end(),m_RetranSINRList.begin(), m_RetranSINRList.begin(), _1 + _2);
		}
	}

	//新数据指示
	bool HARQ::GetNewPacketIndicator( void ) const	 
	{
		//传输次数
		return m_iTranTime == 0;
	}

	//更新传输次数
	void HARQ::UpdateTranTime()	
	{
		m_iTranTime++;
	}

	bool HARQ::MaxTransTime() const	
	{
		//是否为最大传输次数
		//m_iTranTime表示已经传输的次数
		//只有此时还在传输的时候才会调用MaxTransTime，传输次数+1等于一共传输的次数
		// ( 一共传输的次数 == m_iTranTime+1 == m_iMaxTransTime )则返回true
		return m_iTranTime == m_iMaxTransTime - 1;
	}

	void HARQ::SINRCombining( vector<double>& RealSINRList ) 
	{
		using namespace boost::lambda;
		std::transform(m_RetranSINRList.begin(), m_RetranSINRList.end(), RealSINRList.begin(), RealSINRList.begin(), _1+_2);
	}

	void HARQ::Reset()
	{
		m_RetranSINRList.clear();				//清空SINR列表
		m_iTranTime = 0;								//传输次数 注意：2表示重传了一次
		m_InitialTransClock = -100;				//新数据把传输帧号置为无效
		m_dRetransC2I = 0.0;
		UpdateTransStatus(true);					//准备就绪
	}

	void HARQ::SetRetransC2I(double dC2IValue )
	{
		m_dRetransC2I += dC2IValue;
	}

	double HARQ::GetRetransC2I() const
	{
		return m_dRetransC2I;
	}

	void HARQ::SetService(shared_ptr<Service> pService)
	{
		m_pService = pService;
	}

	shared_ptr<Service> HARQ::GetService()
	{
		return m_pService.lock();
	}

	int HARQ::GetTranTime() const	//获得传输次数
	{
		return m_iTranTime;
	}

	bool HARQ::BeReady() const
	{
		return m_bReady;
	}

	int HARQ::GetProcessNo() const
	{
		return m_iProcessNo;
	}

	//更新进程传输状态，调度后未反馈时为false，可以使用作为调度时为true
	void HARQ::UpdateTransStatus( bool bReady )
	{
		m_bReady = bReady;
	}

	//获得初始传输时刻
	int HARQ::GetInitTransTime() const	
	{
		return m_InitialTransClock;
	}
    
	const MCS& HARQ::GetMCS()
	{
		return m_MCS;
	}

	void HARQ::SetUsedRBNum(size_t rbnum)
	{
		m_RBNUM=rbnum;
	}

	size_t  HARQ::GetUsedRBNum()const
	{
		return m_RBNUM;
	}
}

