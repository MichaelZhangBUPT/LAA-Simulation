#pragma once

#include "commondef.h"
#include "HighLayerController.h"
#include "DataDistributeScheme.h"

namespace LTESim
{

	class HighLayerController;
	class ServiceComposite;
	class DataDistributeScheme;
	class DualModeFTPService;

	class IHighLayerScheduler
	{
	public:
		virtual ~IHighLayerScheduler(){};
	public:
		virtual void Process() = 0;
	};

	class HighLayerScheduler : public IHighLayerScheduler
	{
		//Data Type Definition
	public:
		typedef std::map<shared_ptr<ServiceComposite>,DataDistributeScheme>	SERVICEDATADISTSCHEMEMAP;
		typedef std::vector<shared_ptr<ServiceComposite>>	SERVICEVECTOR;
		typedef std::set<shared_ptr<ServiceComposite>>	SERVICESET;
	public:
		HighLayerScheduler( shared_ptr<HighLayerController> pController );
		virtual ~HighLayerScheduler();
	public:
		virtual void Process();
	protected:
		virtual void MakeScheme();

		void PrepareServiceVector();
		void GenerateAndAddCommand();
		void Clear();
		double GetLTERatio( shared_ptr<DualModeFTPService> pDualService );
	protected:
		weak_ptr<HighLayerController>				m_pMasterController;
		SERVICESET											m_ServiceSet;
		SERVICEDATADISTSCHEMEMAP		m_ServiceDataDistSchemeMap;
		struct UEIDDistance
		{
			shared_ptr<ILTEUE> pUE;
			double distance;
			bool operator<(const struct UEIDDistance  &c) const
			{ return (distance<=c.distance) ? true :false; };

		};

		bool							m_bSelectLTE;	//PacketDynamic方案中，是否已经选择好LTE用户
	};

}