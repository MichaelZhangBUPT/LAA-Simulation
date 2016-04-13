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

	void FullBufferService::UpdateThroughput( DATASIZE throughput ) //����������
	{
		m_SumThroughput += throughput;
	}

	void FullBufferService::UpdateBuffer(unsigned int timespan/*������ʱ����ms*/)
	{
		
	}

	void FullBufferService::UpdateDiscardThroughput( DATASIZE throughput )		//���¶������ݰ���������
	{
		m_SumDiscardThroughput += throughput;
	}

	void FullBufferService::Accept( ServiceStatistic& visit )										//����ͳ�Ʒ��ʽӿ�
	{
		visit.Visit(*this);
	}

	void FullBufferService::UpdateMCSIndex( int mcsindex )									//����MCS�ȼ�
	{
		++m_MCSIndex[mcsindex];
	}

	void FullBufferService::UpdateGenerateInterval( void )
	{

	}
}