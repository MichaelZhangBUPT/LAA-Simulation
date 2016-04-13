#include "UpperLayerScheduler.h"

namespace LTESim
{
	UpperLayerScheduler::UpperLayerScheduler()
	{

	}

	UpperLayerScheduler::~UpperLayerScheduler()
	{

	}

	//CMDEngine Interface
	void UpperLayerScheduler::OnClock(const Clock& clock)
	{
		switch ( clock.GetID() )
		{
		case HIGHLEVELCLOCK:
			UpperLayerScheduleProcess(clock);
			break;
		case HSPASLOTCLOCK:
		case LTESLOTCLOCK:
			break;
		default:
			ClockProcessor::OnClock(clock);
			break;
		}
	}

	//ClockProcessor Interface
	void UpperLayerScheduler::FrameProcess( const Clock& FrameClock)
	{

	}

	void UpperLayerScheduler::SubFrameProcess( const Clock& SubFrameClock )
	{

	}

	void UpperLayerScheduler::SlotProcess( const Clock& SlotClock )
	{

	}

	void UpperLayerScheduler::TxProcess( const Clock& txclock )
	{

	}

	void UpperLayerScheduler::RxProcess( const Clock& rxclock )
	{

	}

	void UpperLayerScheduler::UpperLayerScheduleProcess( const Clock& clock )
	{

	}
}