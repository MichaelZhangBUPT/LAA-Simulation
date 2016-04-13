#pragma once

#include "Service.h"

namespace LTESim
{
	class IUE;
	class Service;
	class ServiceStatistic;
	class DataDistributeScheme;

	class ServiceComposite : 
		public Service	
	{
	public:
		ServiceComposite( shared_ptr<IUE> pUE ) : Service( pUE ){};
	public:
		virtual ~ServiceComposite(void){};
		virtual shared_ptr<Service> GetServiceElement( SystemType systemType ) = 0;		//得到Service元素
		virtual void DistributeBufferData( const DataDistributeScheme& rScheme ) = 0;		//将Composite中Buffer的数据分配到其Service Element中
		virtual bool IsDataDistributed() = 0;
		virtual std::vector<double> GetHighLayerDelay() = 0;		//获得由于高层更新周期引入的延时
	};
}