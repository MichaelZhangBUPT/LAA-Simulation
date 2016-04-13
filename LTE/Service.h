#pragma once

#include "commondef.h"
#include <vector>

namespace LTESim
{
	class IUE;
	class ServiceStatistic;

	//ҵ������
	typedef double DATASIZE;
	
	typedef double DATADELAY;
	//���ȼ�
	typedef double PRIORITY;

	//ҵ����
	class Service
	{
	public:
		Service(shared_ptr<IUE> pUE);
	public:
		virtual ~Service(void);
		//��Ӧ��UE
		shared_ptr<IUE> GetUE();
		//��ǰ��buffer״̬
		virtual DATASIZE	GetData() const;
		void SetData( DATASIZE dDataSize );

		double			GetInterval() const;
		DATASIZE	GetThroughput() const;
		DATASIZE	GetIdealThroughput() const;
		DATASIZE	GetAverageThroughput();
		void				SetAverageThroughput( DATASIZE thr);
		DATASIZE	GetDiscardThroughput() const;

		//���MCS�ȼ� YRM
		const vector<double>& GetMCSIndex() const;

		//���ҵ����Ĵ���Ͷ�ȡʱ��
		std::vector<double> GetPacketTransTime() const;
		std::vector<double> GetPacketReadingTime() const;
		double	GetPacketStartTime() const;
		void	SetPacketStartTime( double starttime );
		double	GetPacketEndTime() const;
		void	SetPacketEndTime( double endtime );

		//���һ�����ݰ��ܹ��Ĵ�������ʹ��������
		void	UpdatePacketTransTimes()	{ m_iPacketTransTimes++; }
		int		GetPacketTransTimes()	{ return m_iPacketTransTimes; }
		void	UpdatePacketFailedTransTimes()	{ m_iPacketFailedTransTimes++; }
		int		GetPacketFailedTransTimes()	{ return m_iPacketFailedTransTimes; }
		void	ResetPacketTransTimes()	{ m_iPacketTransTimes = 0; }
		void	ResetPacketFailedTransTimes()	{ m_iPacketFailedTransTimes = 0; }
		
		//FullBufferʱ��ͳ�ƴ�����
		void SetLastTransTime( double time ) { m_dLastTransTime = time; }
		void SetThisTransTime( double time ) { m_dThisTransTime = time; }
		double GetLastTransTime() { return m_dLastTransTime; }
		double GetThisTransTime() { return m_dThisTransTime; }
		std::vector<double> GetTransInterval(); 
		void UpdateTransInterval();



		//��������ʱ��
		DATADELAY GetPacketDelay() const;

		//ҵ�����ȼ�
		PRIORITY GetQoS() const;
		void SetQoS(PRIORITY p);
		
		//�Ƿ񱻵����ϱ�ʶ
		bool GetIsScheduled() const;
		void SetIsScheduled(bool scheduled);

		//��ǰҵ���Ƿ����
		bool GetIsEnd() const;
		void SetIsEnd(bool ending);
		
		//��������Ȱ���С
		double GetLastPacketsize()const;
		void   SetLastPacketsize( double size);
		//����ҵ�����ֲ�����ҵ��buffer
		virtual void UpdateBuffer(unsigned int timespan/*������ʱ����ms*/) = 0;
		//���ɶ�ȡʱ��
		virtual void UpdateGenerateInterval(void) = 0;	

		virtual void UpdateThroughput(DATASIZE throughput) = 0;

		virtual void UpdateDiscardThroughput(DATASIZE throughput) = 0;

		virtual void UpdateMCSIndex(int mcsindex) = 0;

		//void UpdatePacketTime( double time );

		/*��buffer��ȡ��data����λ�����ݷ���*/
		virtual void PopData(DATASIZE data);

		virtual void Accept( ServiceStatistic& visit ) = 0;
		//VoIPҵ�����
		virtual void UpdateService() {}
		//������
		int GetPacketNum();
		int GetDiscardPacketNum();
		double GetChannelUtilization();

		ServiceType GetServiceType();
	protected:
		/*��buffer�м���data����λ��������*/
		virtual void PutBuffer(DATASIZE data);
	protected:
		DATASIZE						m_dBufferData;
		double							m_iGenInterval;					//������ʱ����
		PRIORITY						m_priority;						//���ȼ�
		bool							m_IsScheduled;					//�Ƿ񱻵�����
		bool							m_IsEnd;						//��ǰҵ���Ƿ����
		double							m_lastblocksize;				//��¼���һ������С(PF�㷨��Ҫ��
		weak_ptr<IUE>					m_pUE;							//������UE
		DATASIZE						m_SumIdealThroughput;			//����״̬�£����Դ���������������ڼ����ŵ�������
		DATASIZE						m_SumThroughput;				//�ۻ�����ɹ���������
		DATASIZE						m_SumDiscardThroughput;			//�������ݰ���������
		DATADELAY						m_SumPacketDelay;				//�ۻ�������ʱ��
		DATASIZE						m_AverageThroughput;			//ƽ��������
		std::vector<double>				m_MCSIndex;						//MCS�ȼ�  YRM
		//ʱ��ͳ��
		double							m_dPacketStartTime;				//����һ��������ʼʱ��
		double							m_dPacketEndTime;				//һ�����ݰ����������ʱ��
		std::vector<double>				m_vecPacketTransTime;			//ÿ�����Ĵ���ʱ��
		std::vector<double>				m_vecPacketReadingTime;			//ÿ�����Ķ�ȡʱ��
		int								m_iPacketTransTimes;			//һ�����ݰ��ܹ��Ĵ������
		int								m_iPacketFailedTransTimes;		//һ�����ݰ��ܹ�����Ĵ������
		//FullBufferҵ��ʱ��ͳ��һ���û����δ���֮���ʱ��
		double							m_dLastTransTime;				//��һ�δ���ʱ��
		double							m_dThisTransTime;				//���δ���ʱ��
		std::vector<double>				m_vecTransInterval;				//������
		int								m_iPacketNum;	//�ܹ���������ݰ��ĸ���,����ͳ�ƶ�����
		int								m_iDiscardPacketNum;	//�ﵽ�������������ݰ��ĸ���
		ServiceType						m_ServiceType;
};
}