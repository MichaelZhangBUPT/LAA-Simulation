#include "commondef.h"
#include "ServiceStatistic.h"
#include "DLFTPService.h"
#include <numeric>

namespace LTESim
{
	ServiceStatistic::ServiceStatistic(void)
	{
	
	}

	ServiceStatistic::~ServiceStatistic(void)
	{

	}

	//统计吞吐量
	ThroughputStatistic::ThroughputStatistic(void):DLFTPSumThroughput(0.0)
	{
	}

	ThroughputStatistic::~ThroughputStatistic(void)
	{

	}

	void ThroughputStatistic::Visit(Service& service)
	{
		m_DLFTPthroughput.push_back(service.GetThroughput());	//更新用户的吞吐量数据表
		DLFTPSumThroughput += service.GetThroughput();	//更新系统总吞吐量
	}
	
	const vector<double>& ThroughputStatistic::GetStatistic() const
	{
		return m_DLFTPthroughput;
	}

	double ThroughputStatistic::GetSumStatistic() const
	{
		return DLFTPSumThroughput;
	}

	//统计丢弃数据包的吞吐量
	DiscardThroughputStatistic::DiscardThroughputStatistic(void):DLFTPSumDiscardThroughput(0.0)
	{
	}

	DiscardThroughputStatistic::~DiscardThroughputStatistic(void)
	{
	}

	void DiscardThroughputStatistic::Visit(Service& service)
	{
		m_DLFTPDiscardThroughput.push_back(service.GetDiscardThroughput());	//更新用户丢弃数据包的吞吐量表
		DLFTPSumDiscardThroughput += service.GetDiscardThroughput();	//更新系统总的丢弃数据包的吞吐量
	}

	const vector<double>& DiscardThroughputStatistic::GetStatistic() const
	{
		return m_DLFTPDiscardThroughput;
	}

	double DiscardThroughputStatistic::GetSumStatistic() const 
	{
		return DLFTPSumDiscardThroughput;
	}

	//统计包传输时延
	PacketDelayStatistic::PacketDelayStatistic(void):DLFTPSumPacketDelay(0.0)
	{
	}

	PacketDelayStatistic::~PacketDelayStatistic(void)
	{
	}

	void PacketDelayStatistic::Visit(Service& service)
	{
		m_DLFTPPacketDelay.push_back(service.GetPacketDelay());	//更新用户延时的统计表
		DLFTPSumPacketDelay += service.GetPacketDelay();	//更新系统的总延时
	}

	const vector<double>& PacketDelayStatistic::GetStatistic() const
	{
		return m_DLFTPPacketDelay;
	}

	double PacketDelayStatistic::GetSumStatistic() const
	{
		return DLFTPSumPacketDelay;
	}
	
	//MCS统计
	MCSStatistic::MCSStatistic( /*SystemType systemtype*/ ):DLFTPSumMCS(12, 0.0)
	{

	}

	MCSStatistic::~MCSStatistic( void )
	{

	}

	void MCSStatistic::Visit( Service& service )
	{
		m_MCS.push_back(service.GetMCSIndex());//各个UE的MCS统计
		vector<double>::const_iterator itrMCS = service.GetMCSIndex().begin();
		for (vector<double>::iterator itr = DLFTPSumMCS.begin() ; ( itr != DLFTPSumMCS.end() ) && ( itrMCS != service.GetMCSIndex().end() ) ; ++itr)
		{
			*itr += *itrMCS;
			++itrMCS;
		}
	}

	const vector<double>& MCSStatistic::GetStatistic( void ) const
	{
		return DLFTPSumMCS;
	}

	double MCSStatistic::GetSumStatistic( void ) const
	{
		return 0.0;
	}

	//统计业务包的传输时间
	PacketTimeStatistic::PacketTimeStatistic(void)
	{
	}

	PacketTimeStatistic::~PacketTimeStatistic(void)
	{

	}

	void PacketTimeStatistic::Visit(Service& service)
	{
		vector<double> PacketTimeVec = service.GetPacketTransTime();
		m_UEAverPacketTime.push_back( std::accumulate(PacketTimeVec.begin(),PacketTimeVec.end(),0.0) / PacketTimeVec.size() );
		m_AllPacketTime.insert( m_AllPacketTime.end(), PacketTimeVec.begin(), PacketTimeVec.end() );
	}
	
	const vector<double>& PacketTimeStatistic::GetStatistic() const
	{
		return m_UEAverPacketTime;
	}

	const vector<double>& PacketTimeStatistic::GetAllStatistic()
	{
		return m_AllPacketTime;
	}

	double PacketTimeStatistic::GetSumStatistic( void ) const
	{
		return 0.0;
	}

	//统计传输时间间隔
	TransIntervalStatistic::TransIntervalStatistic(void)
	{
	}

	TransIntervalStatistic::~TransIntervalStatistic(void)
	{

	}

	void TransIntervalStatistic::Visit(Service& service)
	{
		std::vector<double> TransIntervalVec = service.GetTransInterval();
		double AverTime = std::accumulate(TransIntervalVec.begin(),TransIntervalVec.end(),0.0);
		m_DLFTPAverTransInterval.push_back(AverTime/TransIntervalVec.size());
	}

	const vector<double>& TransIntervalStatistic::GetStatistic() const
	{
		return m_DLFTPAverTransInterval;
	}

	double TransIntervalStatistic::GetSumStatistic( void ) const
	{
		return 0.0;
	}

	//统计丢包率
	PacketLossStatistic::PacketLossStatistic(void):m_PacketNum(0), m_DiscardPacketNum(0)
	{
	}

	PacketLossStatistic::~PacketLossStatistic(void)
	{

	}

	void PacketLossStatistic::Visit(Service& service)
	{
		m_UEPacketLoss.push_back( double(service.GetDiscardPacketNum())/service.GetPacketNum() );
		m_PacketNum += service.GetPacketNum();
		m_DiscardPacketNum += service.GetDiscardPacketNum();
	}

	const vector<double>& PacketLossStatistic::GetStatistic() const
	{
		return m_UEPacketLoss;
	}

	const vector<double>& PacketLossStatistic::GetAllStatistic()
	{
		m_DiscardAndSum.push_back( m_DiscardPacketNum );
		m_DiscardAndSum.push_back( m_PacketNum );
		return m_DiscardAndSum;
	}

	double PacketLossStatistic::GetSumStatistic() const
	{
		return 0.0;
	}

	//统计信道利用率
	ChannelUtilizationStatistic::ChannelUtilizationStatistic(void):m_SumThroughput(0.0), m_SumIdealThroughput(0.0)
	{
	}

	ChannelUtilizationStatistic::~ChannelUtilizationStatistic(void)
	{

	}

	void ChannelUtilizationStatistic::Visit(Service& service)
	{
		m_UEChannelUti.push_back( service.GetThroughput()/service.GetIdealThroughput() );
		m_SumThroughput += service.GetThroughput();
		m_SumIdealThroughput += service.GetIdealThroughput();
	}

	const vector<double>& ChannelUtilizationStatistic::GetStatistic() const
	{
		return m_UEChannelUti;
	}

	const vector<double>& ChannelUtilizationStatistic::GetAllStatistic()
	{
		m_SumAndIdeal.push_back( m_SumThroughput );
		m_SumAndIdeal.push_back( m_SumIdealThroughput );
		return m_SumAndIdeal;
	}

	double ChannelUtilizationStatistic::GetSumStatistic() const
	{
		return 0.0;
	}

}
