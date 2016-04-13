#pragma once
#include "Service.h"
#include "RandomManager.h"

namespace LTESim
{
	//数据统计采用Visitor模式
	class ServiceStatistic;
	class FullBufferService:public Service		
	{
	public:
		FullBufferService(shared_ptr<IUE> pUE, LinkDirection linkDirection);
	public:
		virtual ~FullBufferService(void);
		virtual void UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/);
		virtual void UpdateGenerateInterval(void);														//生成读取时间
		virtual void UpdateThroughput(DATASIZE throughput);									//吞吐量更新
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//丢弃数据包的吞吐量更新
		virtual void UpdateMCSIndex(int mcsindex);														//更新MCS等级 YRM
		virtual void Accept(ServiceStatistic& visit );														//数据统计接口
	protected:
		LinkDirection m_LinkDirection;
	};

}
