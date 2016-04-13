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
		virtual shared_ptr<Service> GetServiceElement( SystemType systemType ) = 0;		//�õ�ServiceԪ��
		virtual void DistributeBufferData( const DataDistributeScheme& rScheme ) = 0;		//��Composite��Buffer�����ݷ��䵽��Service Element��
		virtual bool IsDataDistributed() = 0;
		virtual std::vector<double> GetHighLayerDelay() = 0;		//������ڸ߲���������������ʱ
	};
}