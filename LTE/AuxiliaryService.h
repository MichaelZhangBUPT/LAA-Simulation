#pragma once

#include "Service.h"

namespace LTESim
{
	//该Service的Buffer中的数据需要ServiceComposite来进行分配
	class AuxiliaryService : public Service
	{
	public:
		AuxiliaryService( shared_ptr<IUE> pUE, shared_ptr<Service> pCompService );
	public:
		virtual ~AuxiliaryService();
		virtual void UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/);
		virtual void UpdateGenerateInterval(void);														//生成读取时间
		virtual void UpdateThroughput(DATASIZE throughput);									//吞吐量更新
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//丢弃数据包的吞吐量更新
		virtual void UpdateMCSIndex(int mcsindex);												//更新MCS等级 YRM
		virtual void Accept(ServiceStatistic& visit );											//数据统计接口
		virtual double GetThrInAPacket();
	private:
		weak_ptr<Service>	m_pMasterCompService;
		double	m_dThrInAPacket;
	};
}