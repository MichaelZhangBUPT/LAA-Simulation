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
		virtual void UpdateBuffer(unsigned int timespan/*������ʱ����ms*/);
		virtual void UpdateGenerateInterval(void);															//���ɶ�ȡʱ��
		virtual void UpdateThroughput(DATASIZE throughput);										//����������
		virtual void UpdateDiscardThroughput(DATASIZE throughput);							//�������ݰ�������������
		virtual void UpdateMCSIndex(int mcsindex);														//����MCS�ȼ�
		virtual void Accept(ServiceStatistic& visit );														//����ͳ�ƽӿ�
	public:
		virtual shared_ptr<Service> GetServiceElement( SystemType systemType );
		virtual void DistributeBufferData( const DataDistributeScheme& rScheme );
		virtual bool IsDataDistributed();
		virtual std::vector<double> GetHighLayerDelay();		//������ڸ߲���������������ʱ
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

		double	m_dUpdateBufferTime;				//ͳ�����ݰ����ڸ߲�������������������ʱ��
		std::vector<double>	m_vecHighLayerDelay;
		double	m_dLTERatio;						//LTEҵ�����
		bool	m_bIsRatioSure;						//LTEҵ������Ƿ�ȷ��

	};
}