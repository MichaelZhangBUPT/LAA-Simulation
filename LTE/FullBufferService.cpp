#include "commondef.h"
#include "FullBufferService.h"
#include "ServiceStatistic.h"
#include "UE.h"
#include "ClockManager.h"

#include <limits>

namespace LTESim
{

	FullBufferService::FullBufferService(shared_ptr<IUE> pUE, LinkDirection linkDirection)
		:	Service( pUE ), m_LinkDirection( linkDirection )
	{
		m_dBufferData = std::numeric_limits<double>::max();
		m_ServiceType = FullBuffer;
	}

	FullBufferService::~FullBufferService(void)
	{
	}

	void FullBufferService::UpdateThroughput( DATASIZE throughput ) //更新吞吐量
	{
		m_SumThroughput += throughput;
	}

	void FullBufferService::UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/)
	{
		
	}

	void FullBufferService::UpdateDiscardThroughput( DATASIZE throughput )		//更新丢弃数据包的吞吐量
	{
		m_SumDiscardThroughput += throughput;
	}

	void FullBufferService::Accept( ServiceStatistic& visit )										//数据统计访问接口
	{
		visit.Visit(*this);
	}

	void FullBufferService::UpdateMCSIndex( int mcsindex )									//更新MCS等级
	{
		++m_MCSIndex[mcsindex];
	}

	void FullBufferService::UpdateGenerateInterval( void )
	{

	}
}