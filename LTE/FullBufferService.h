#pragma once
#include "Service.h"
#include "RandomManager.h"

namespace LTESim
{
	//����ͳ�Ʋ���Visitorģʽ
	class ServiceStatistic;
	class FullBufferService:public Service		
	{
	public:
		FullBufferService(shared_ptr<IUE> pUE, LinkDirection linkDirection);
	public:
		virtual ~FullBufferService(void);
		virtual void UpdateBuffer(unsigned int timespan/*������ʱ����ms*/);
		virtual void UpdateGenerateInterval(void);														//���ɶ�ȡʱ��
		virtual void UpdateThroughput(DATASIZE throughput);									//����������
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//�������ݰ�������������
		virtual void UpdateMCSIndex(int mcsindex);														//����MCS�ȼ� YRM
		virtual void Accept(ServiceStatistic& visit );														//����ͳ�ƽӿ�
	protected:
		LinkDirection m_LinkDirection;
	};

}
