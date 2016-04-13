#pragma once
#include "Service.h"
#include "RandomManager.h"
namespace LTESim
{
	//����ͳ�Ʋ���Visitorģʽ
	class ServiceStatistic;
	class DLFTPService:public Service		
	{
	public:
		DLFTPService(shared_ptr<IUE> pUE, double dFileSize, double dReadingTimeMean);
	public:
		virtual ~DLFTPService(void);
		virtual void UpdateBuffer(unsigned int timespan/*������ʱ����ms*/);
		virtual void UpdateGenerateInterval(void);														//���ɶ�ȡʱ��
		virtual void UpdateThroughput(DATASIZE throughput);									//����������
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//�������ݰ�������������
		virtual void UpdateMCSIndex(int mcsindex);													//����MCS�ȼ� YRM
		virtual void Accept(ServiceStatistic& visit );													//����ͳ�ƽӿ�
	protected:
		double m_dFileSize;
		double m_dReadingTimeMean;
	};

}
