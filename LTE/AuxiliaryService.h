#pragma once

#include "Service.h"

namespace LTESim
{
	//��Service��Buffer�е�������ҪServiceComposite�����з���
	class AuxiliaryService : public Service
	{
	public:
		AuxiliaryService( shared_ptr<IUE> pUE, shared_ptr<Service> pCompService );
	public:
		virtual ~AuxiliaryService();
		virtual void UpdateBuffer(unsigned int timespan/*������ʱ����ms*/);
		virtual void UpdateGenerateInterval(void);														//���ɶ�ȡʱ��
		virtual void UpdateThroughput(DATASIZE throughput);									//����������
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//�������ݰ�������������
		virtual void UpdateMCSIndex(int mcsindex);												//����MCS�ȼ� YRM
		virtual void Accept(ServiceStatistic& visit );											//����ͳ�ƽӿ�
		virtual double GetThrInAPacket();
	private:
		weak_ptr<Service>	m_pMasterCompService;
		double	m_dThrInAPacket;
	};
}