#include "HighLayerController.h"
#include "HighLayerReport.h"
#include "HighLayerSchedulerScheme.h"
#include "System.h"
#include "RunParameters.h"

namespace LTESim
{

	HighLayerController::HighLayerController( int iID, double dPosX, double dPoxY )
		: SystemElement( iID, POS( dPosX, dPoxY ) )
	{

	}

	HighLayerController::~HighLayerController()
	{

	}

	//CMDEngine Interface
	void HighLayerController::OnClock(const Clock& clock)
	{
		switch ( clock.GetID() )
		{
			//该控制器只在High Level Clock进行Update的时候进行实际工作
		case HIGHLAYERCLOCK:
			HighLevelProcess(clock);
			break;
		case HSPASUBFRAMECLOCK:
		case LTESUBFRAMECLOCK:
		case HSPASLOTCLOCK:
		case LTESLOTCLOCK:
		case WIFICLOCK:
			break;
		default:
			ClockProcessor::OnClock(clock);
			break;
		}

		CMDEngine::Run(clock);
	}

	//ClockProcessor Interface
	void HighLayerController::FrameProcess( const Clock& clock)
	{

	}

	void HighLayerController::SubFrameProcess( const Clock& clock )
	{

	}

	void HighLayerController::SlotProcess( const Clock& clock )
	{

	}

	void HighLayerController::TxProcess( const Clock& clock )
	{

	}

	void HighLayerController::RxProcess( const Clock& clock )
	{

	}

	void HighLayerController::HighLevelProcess( const Clock& clock )
	{
		USER_MESSAGE( "HighLayerController::HighLevelProcess" );
		//高层调度的时钟通知，调度器进行操作
		m_pHighLayerScheduler->Process();
	}

	void HighLayerController::Initialize()
	{
		//shared_ptr<System> pSystem = System::GetCurrentSystem();
		//bool bScheme = pSystem->GetConfig().GetBool( "HighLayerSchedulerScheme_HSPAtoDual" );
		//if( !bScheme )
		//{
			shared_ptr<IHighLayerScheduler> pIScheduler( new HighLayerScheduler( shared_from_this() ) );
			m_pHighLayerScheduler = pIScheduler;
		//}
		//if( bScheme )
		//{
		//	shared_ptr<IHighLayerScheduler> pIScheduler( new HighLayerSchedulerScheme( shared_from_this() ) );
		//	m_pHighLayerScheduler = pIScheduler;
		//}
	}

	void HighLayerController::UpdateReport( const HSPANBReport& rReport )
	{
		m_HSPANBReport = rReport;
	}

	void HighLayerController::UpdateReport( const LTENBReport& rReport )
	{
		m_LTENBReport = rReport;
	}

	LTENBReport HighLayerController::GetLTENBReport() const
	{
		return m_LTENBReport;
	}

	HSPANBReport HighLayerController::GetHSPANBReport() const
	{
		return m_HSPANBReport;
	}
}