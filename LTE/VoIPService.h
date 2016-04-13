#pragma once
#include "Service.h"
#include "RandomManager.h"
namespace LTESim
{
	//数据统计采用Visitor模式
	class ServiceStatistic;
	class VoIPService:public Service		
	{
	public:
		VoIPService(shared_ptr<IUE> pUE, double delay, double period);
	public:
		virtual ~VoIPService(void);
		virtual void UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/);
		virtual void UpdateGenerateInterval(void);														//生成读取时间
		virtual void UpdateThroughput(DATASIZE throughput);									//吞吐量更新
		virtual void UpdateDiscardThroughput(DATASIZE throughput);						//丢弃数据包的吞吐量更新
		virtual void UpdateMCSIndex(int mcsindex);													//更新MCS等级 YRM
		virtual void Accept(ServiceStatistic& visit );													//数据统计接口
		virtual void UpdateService();
		virtual double GetData() const;

	protected:
		double				m_dLastPacketTime;	//上一个数据包产生的时间，单位ms
		mutable UNIFORM_REAL_GEN	m_UniformGen;	//随机数生成器
		map<double,double>	m_IPPacketList;	//存储IP数据包的队列，<生成时间,包大小>
		double				m_dPacketDelay;	//L2分组延迟预算
		double					m_iSamplePeriod;	//语音包产生间隔,ms
		double					m_iPayload;	//每个包的大小，bit
		SystemType			m_SystemType;	//业务对应的UE的类型
	};

}
