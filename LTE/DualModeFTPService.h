#pragma once 

#include "ServiceComposite.h"
#include <boost\enable_shared_from_this.hpp>

namespace LTESim
{
	class IUE;
	class ServiceStatistic;

	class DualModeFTPService : 
		public ServiceComposite,
		public boost::enable_shared_from_this<DualModeFTPService>
	{
	public:
		static shared_ptr<DualModeFTPService> CreateInstance( shared_ptr<IUE> pUE, double dFileSize, double dReadingTimeMean );
	protected:
		DualModeFTPService( shared_ptr<IUE> pUE, double dFileSize, double dReadingTimeMean );
	public:
		virtual ~DualModeFTPService(void);
		virtual void UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/);
		virtual void UpdateGenerateInterval(void);															//生成读取时间
		virtual void UpdateThroughput(DATASIZE throughput);										//吞吐量更新
		virtual void UpdateDiscardThroughput(DATASIZE throughput);							//丢弃数据包的吞吐量更新
		virtual void UpdateMCSIndex(int mcsindex);														//更新MCS等级
		virtual void Accept(ServiceStatistic& visit );														//数据统计接口
	public:
		virtual shared_ptr<Service> GetServiceElement( SystemType systemType );
		virtual void DistributeBufferData( const DataDistributeScheme& rScheme );
		virtual bool IsDataDistributed();
		virtual std::vector<double> GetHighLayerDelay();		//获得由于高层更新周期引入的延时
		virtual double GetLTERatio(void);
		virtual void SetLTERatio(const double ratio);
		virtual double GetFileSize(void);

		//shared_ptr<NodeB> GetServingNodeB();
		//shared_ptr<WiFiAP> GetServingAP();
protected:
		shared_ptr<Service> m_pLTEService;
		shared_ptr<Service> m_pHSPAService;
		shared_ptr<Service> m_pWiFiService;
		bool		m_bIsDataDistributed;
		double	m_dFileSize;
		double	m_dReadingTimeMean;

		double	m_dUpdateBufferTime;				//统计数据包由于高层控制器更新周期引入的时延
		std::vector<double>	m_vecHighLayerDelay;
		double	m_dLTERatio;						//LTE业务比例
		bool	m_bIsRatioSure;						//LTE业务比例是否确定

	};
}