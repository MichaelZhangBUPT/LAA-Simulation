#include "commondef.h"
#include "Service.h"


namespace LTESim
{
#pragma region 内联函数

	//获得缓存数据
	DATASIZE Service::GetData() const
	{
		return m_dBufferData;
	}

	void Service::SetData( DATASIZE dDataSize )
	{
		m_dBufferData = dDataSize;
	}

	//获得生成传输包的时间间隔
	double Service::GetInterval() const
	{
		return m_iGenInterval;
	}

	//获得优先权
	PRIORITY Service::GetQoS() const
	{
		return m_priority;
	}

	//获得是否被调度上标识
	bool Service::GetIsScheduled()const
	{
		return m_IsScheduled;
	}

	//获得当前业务是否结束的标示
	bool Service::GetIsEnd()const
	{
		return m_IsEnd;
	}

	void Service::SetIsEnd(bool ending)
	{
		m_IsEnd = ending;
	}

	void Service::SetAverageThroughput(DATASIZE thr)
	{
		m_AverageThroughput = thr;
	}

	DATASIZE Service::GetAverageThroughput()
	{
		return m_AverageThroughput;
	}

	//获得最后传输包大小
	double Service::GetLastPacketsize()const
	{
		return m_lastblocksize;
	}

	//获得吞吐量
	DATASIZE Service::GetThroughput()const
	{
		return m_SumThroughput;
	}

	//获得理想的吞吐量，用以计算信道利用率
	DATASIZE Service::GetIdealThroughput()const
	{
		return m_SumIdealThroughput;
	}

	//获得丢弃数据包的吞吐量
	DATASIZE Service::GetDiscardThroughput()const
	{
		return m_SumDiscardThroughput;
	}

	//获得MCS等级  YRM
	const vector<double>& Service::GetMCSIndex() const
	{
		return m_MCSIndex;
	}

	//获得包延时
	DATADELAY Service::GetPacketDelay() const
	{
		return m_SumPacketDelay;
	}

	//获得UE
	shared_ptr<IUE> Service::GetUE()
	{
		return m_pUE.lock();
	}

	void Service::PopData( DATASIZE data )
	{
		m_dBufferData -= data;
	}
	//累加缓存
	void Service::PutBuffer( DATASIZE data )
	{
		m_dBufferData += data;
	}

	//设置优先权
	void Service::SetQoS( PRIORITY p )
	{
		m_priority = p;
	}

	void Service::SetIsScheduled(bool scheduled)
	{
		m_IsScheduled = scheduled;
	}

	void Service::SetLastPacketsize( double size)
	{
		m_lastblocksize = size;
	}

	double Service::GetPacketStartTime() const
	{
		return m_dPacketStartTime;
	}

	void Service::SetPacketStartTime( double starttime )
	{
		m_dPacketStartTime = starttime;
	}

	double Service::GetPacketEndTime() const
	{
		return m_dPacketEndTime;
	}

	void Service::SetPacketEndTime( double endtime )
	{
		m_dPacketEndTime = endtime;
	}

	//void Service::UpdatePacketTime( double time )
	//{
	//	m_PacketTime.push_back(time);
	//}

	std::vector<double> Service::GetPacketTransTime() const
	{
		return m_vecPacketTransTime;
	}

	std::vector<double> Service::GetPacketReadingTime() const
	{
		return m_vecPacketReadingTime;
	}

	std::vector<double> Service::GetTransInterval() 
	{ 
		if ( m_vecTransInterval.size() >= 2 )
		{
			vector<double> usefuldata( m_vecTransInterval.begin()++,m_vecTransInterval.end() );
			return usefuldata; 
		}
		else
		{
			vector<double> usefuldata( 2, 0.0 );//
			return usefuldata; 
		}
	}

	void Service::UpdateTransInterval() 
	{ 
		m_vecTransInterval.push_back( m_dThisTransTime-m_dLastTransTime );
	}

	int Service::GetPacketNum()
	{
		return m_iPacketNum;
	}

	int Service::GetDiscardPacketNum()
	{
		return m_iDiscardPacketNum;
	}

	double Service::GetChannelUtilization()
	{
		return double(m_SumThroughput) / double(m_SumIdealThroughput);
	}

	ServiceType Service::GetServiceType()
	{
		return m_ServiceType;
	}



#pragma endregion

	Service::Service(shared_ptr<IUE> pUE):m_pUE(pUE),m_MCSIndex(12, 0.0)
	{
		m_SumIdealThroughput = 0.0;
		m_SumThroughput = 0.0;				//初始化吞吐量
		m_SumDiscardThroughput = 0.0;	//初始化丢弃报的吞吐量
		m_SumPacketDelay = 0.0;				//初始化包的延时
		m_dBufferData = 0.0;					//生成的数据包大小
		m_iGenInterval = 0.0;				//生成传输数据包的时间间隔
		m_priority = 0.0;							//初始化优先级
		m_IsScheduled = false;					//初始化是否被调度上标识
		m_IsEnd = false;							//初始化业务是否结束
		m_lastblocksize = 0.0;					//初始化最后一个块大小
		m_AverageThroughput = 0.01;		//初始化平均吞吐量
		m_dPacketStartTime = 0.0;		//初始化包的起始时间
		m_dPacketEndTime = 0.0;
		m_iPacketTransTimes = 0;
		m_iPacketFailedTransTimes = 0;
		m_dLastTransTime = 0.0;
		m_dThisTransTime = 0.0;
		m_iPacketNum = 0;
		m_iDiscardPacketNum = 0;
	}

	Service::~Service(void)
	{

	}


}
