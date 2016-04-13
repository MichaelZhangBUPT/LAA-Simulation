#include "commondef.h"
#include "Service.h"


namespace LTESim
{
#pragma region ��������

	//��û�������
	DATASIZE Service::GetData() const
	{
		return m_dBufferData;
	}

	void Service::SetData( DATASIZE dDataSize )
	{
		m_dBufferData = dDataSize;
	}

	//������ɴ������ʱ����
	double Service::GetInterval() const
	{
		return m_iGenInterval;
	}

	//�������Ȩ
	PRIORITY Service::GetQoS() const
	{
		return m_priority;
	}

	//����Ƿ񱻵����ϱ�ʶ
	bool Service::GetIsScheduled()const
	{
		return m_IsScheduled;
	}

	//��õ�ǰҵ���Ƿ�����ı�ʾ
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

	//�����������С
	double Service::GetLastPacketsize()const
	{
		return m_lastblocksize;
	}

	//���������
	DATASIZE Service::GetThroughput()const
	{
		return m_SumThroughput;
	}

	//�������������������Լ����ŵ�������
	DATASIZE Service::GetIdealThroughput()const
	{
		return m_SumIdealThroughput;
	}

	//��ö������ݰ���������
	DATASIZE Service::GetDiscardThroughput()const
	{
		return m_SumDiscardThroughput;
	}

	//���MCS�ȼ�  YRM
	const vector<double>& Service::GetMCSIndex() const
	{
		return m_MCSIndex;
	}

	//��ð���ʱ
	DATADELAY Service::GetPacketDelay() const
	{
		return m_SumPacketDelay;
	}

	//���UE
	shared_ptr<IUE> Service::GetUE()
	{
		return m_pUE.lock();
	}

	void Service::PopData( DATASIZE data )
	{
		m_dBufferData -= data;
	}
	//�ۼӻ���
	void Service::PutBuffer( DATASIZE data )
	{
		m_dBufferData += data;
	}

	//��������Ȩ
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
		m_SumThroughput = 0.0;				//��ʼ��������
		m_SumDiscardThroughput = 0.0;	//��ʼ����������������
		m_SumPacketDelay = 0.0;				//��ʼ��������ʱ
		m_dBufferData = 0.0;					//���ɵ����ݰ���С
		m_iGenInterval = 0.0;				//���ɴ������ݰ���ʱ����
		m_priority = 0.0;							//��ʼ�����ȼ�
		m_IsScheduled = false;					//��ʼ���Ƿ񱻵����ϱ�ʶ
		m_IsEnd = false;							//��ʼ��ҵ���Ƿ����
		m_lastblocksize = 0.0;					//��ʼ�����һ�����С
		m_AverageThroughput = 0.01;		//��ʼ��ƽ��������
		m_dPacketStartTime = 0.0;		//��ʼ��������ʼʱ��
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
