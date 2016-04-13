#pragma  once
#include "HighLayerScheduler.h"
#include "ServiceComposite.h"

namespace LTESim
{
	class HighLayerScheduler;
	class ServiceComposite;

	class HighLayerSchedulerScheme :
		public LTESim::HighLayerScheduler
	{
	public:
		typedef std::set<shared_ptr<ServiceComposite>>	SERVICESET;
		HighLayerSchedulerScheme( shared_ptr<HighLayerController> pController );
		virtual ~HighLayerSchedulerScheme();
	protected:
		virtual void MakeScheme();
	protected:
		struct ServiceThr
		{
			shared_ptr<ServiceComposite>	m_pService;
			double				m_dThr;		//吞吐量
			double				m_LTERatio;	//LTE业务比例	
			bool operator<(const struct ServiceThr  &c) const 
			{ return (m_dThr < c.m_dThr) ? true : false; };
		};

		std::vector<ServiceThr>						m_ServiceThrVec;
		std::vector<int>								m_DualUEID;
	};
}