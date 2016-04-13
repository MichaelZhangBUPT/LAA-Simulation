#pragma once
#include "Service.h"
#include "RandomManager.h"
namespace LTESim
{
	//����ͳ�Ʋ���Visitorģʽ
	class ServiceStatistic;
	class VoIPService:public Service		
	{
	public:
		VoIPService(shared_ptr<IUE> pUE, double delay, double period);
	public:
		virtual ~VoIPService(void);
		virtual void UpdateBuffer(unsigned int timespan/*������ʱ����ms*/);
		virtual void UpdateGenerateInterval(void);														//���ɶ�ȡʱ��
		virtual void UpdateThroughput(DATASIZE throughput);									//����������
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//�������ݰ�������������
		virtual void UpdateMCSIndex(int mcsindex);													//����MCS�ȼ� YRM
		virtual void Accept(ServiceStatistic& visit );													//����ͳ�ƽӿ�
		virtual void UpdateService();
		virtual double GetData() const;

	protected:
		double				m_dLastPacketTime;	//��һ�����ݰ�������ʱ�䣬��λms
		mutable UNIFORM_REAL_GEN	m_UniformGen;	//�����������
		map<double,double>	m_IPPacketList;	//�洢IP���ݰ��Ķ��У�<����ʱ��,����С>
		double				m_dPacketDelay;	//L2�����ӳ�Ԥ��
		double					m_iSamplePeriod;	//�������������,ms
		double					m_iPayload;	//ÿ�����Ĵ�С��bit
		SystemType			m_SystemType;	//ҵ���Ӧ��UE������
	};

}
