#pragma once

#include "commondef.h"
#include "CMDEngine.h"
#include "SystemElement.h"
#include "ClockProcessor.h"
#include "HighLayerScheduler.h"
//#include "HighLayerSchedulerScheme.h"
#include "HighLayerReport.h"

namespace LTESim
{

	class IHighLayerScheduler;

	class HighLayerController :
		public CMDEngine,
		public SystemElement,
		public ClockProcessor,
		public boost::enable_shared_from_this<HighLayerController>
	{
	public:
		typedef shared_ptr<HighLayerController> PTR;
	public:
		HighLayerController( int iID, double dPosX, double dPoxY );
		virtual ~HighLayerController();
	public:
		//CMDEngine Interface
		virtual void OnClock(const Clock& clock);

		//ClockProcessor Interface
		virtual void FrameProcess( const Clock& clock);
		virtual void SubFrameProcess( const Clock& clock );
		virtual void SlotProcess( const Clock& clock );
		virtual void TxProcess( const Clock& clock );
		virtual void RxProcess( const Clock& clock );	
		virtual void HighLevelProcess( const Clock& clock );
		
		void Initialize();

		void UpdateReport( const LTENBReport& rReport );
		void UpdateReport( const HSPANBReport& rReport );

		LTENBReport GetLTENBReport() const;
		HSPANBReport GetHSPANBReport() const;
	protected:
		boost::shared_ptr<IHighLayerScheduler> m_pHighLayerScheduler;
		LTENBReport		m_LTENBReport;
		HSPANBReport	m_HSPANBReport;
	};

}