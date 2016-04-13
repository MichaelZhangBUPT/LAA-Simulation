#pragma once
#include "Service.h"
#include "RandomManager.h"
namespace LTESim
{
	//����ͳ�Ʋ���Visitorģʽ
	class ServiceStatistic;
	class StreamService:public Service		
	{
	public:
		StreamService(shared_ptr<IUE> pUE, double dFileSize);
	public:
		virtual ~StreamService(void);
		virtual void UpdateBuffer(unsigned int timespan/*������ʱ����ms*/);
		virtual void UpdateGenerateInterval(void);														//���ɶ�ȡʱ��
		virtual void UpdateThroughput(DATASIZE throughput);									//����������
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//�������ݰ�������������
		virtual void UpdateMCSIndex(int mcsindex);													//����MCS�ȼ� YRM
		virtual void Accept(ServiceStatistic& visit );													//����ͳ�ƽӿ�
	protected:
		double PacketsSize;
		int PacketsNum;										//��ǰ��Ƶ֡�а������
		std::vector<double> m_PacketSize;					//ÿ��Ƶ֡�еİ���С
		std::vector<double> m_PacketDelayPerFarme;			//ÿ��Ƶ֡�еİ��Ĵ���ʱ��
		std::vector<double> m_PacketTimePerFarme;			//ÿ��Ƶ֡�еİ��Ĵ���ʱ��
		double m_SumThroughputPerFrame;
		double m_SumPacketDelayPerFrame;
		double m_SumPacketPerFrame;
		double m_tempThroughput;
		static const int PacketsPerVideoFrame;				//��Ƶ֡�еİ���
		static const double VideoFrameLength ;				//��Ƶ֡���ȣ�ms��

	};

}