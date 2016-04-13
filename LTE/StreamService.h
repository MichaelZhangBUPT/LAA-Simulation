#pragma once
#include "Service.h"
#include "RandomManager.h"
namespace LTESim
{
	//数据统计采用Visitor模式
	class ServiceStatistic;
	class StreamService:public Service		
	{
	public:
		StreamService(shared_ptr<IUE> pUE, double dFileSize);
	public:
		virtual ~StreamService(void);
		virtual void UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/);
		virtual void UpdateGenerateInterval(void);														//生成读取时间
		virtual void UpdateThroughput(DATASIZE throughput);									//吞吐量更新
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//丢弃数据包的吞吐量更新
		virtual void UpdateMCSIndex(int mcsindex);													//更新MCS等级 YRM
		virtual void Accept(ServiceStatistic& visit );													//数据统计接口
	protected:
		double PacketsSize;
		int PacketsNum;										//当前视频帧中包的序号
		std::vector<double> m_PacketSize;					//每视频帧中的包大小
		std::vector<double> m_PacketDelayPerFarme;			//每视频帧中的包的传输时延
		std::vector<double> m_PacketTimePerFarme;			//每视频帧中的包的传输时间
		double m_SumThroughputPerFrame;
		double m_SumPacketDelayPerFrame;
		double m_SumPacketPerFrame;
		double m_tempThroughput;
		static const int PacketsPerVideoFrame;				//视频帧中的包数
		static const double VideoFrameLength ;				//视频帧长度（ms）

	};

}