#include "commondef.h"
#include "Clock.h"
#include "CMDEngine.h"
#include "ClockProcessor.h"
#include <boost/enable_shared_from_this.hpp>

namespace LTESim
{
	class UpperLayerScheduler
		: public CMDEngine, public ClockProcessor, public boost::enable_shared_from_this<UpperLayerScheduler>
	{
	public:
		UpperLayerScheduler();
		virtual ~UpperLayerScheduler();
	public:
		//CMDEngine Interface
		virtual void OnClock(const Clock& clock);

		//ClockProcessor Interface
		virtual void FrameProcess( const Clock& FrameClock);
		virtual void SubFrameProcess( const Clock& SubFrameClock );
		virtual void SlotProcess( const Clock& SlotClock );
		virtual void TxProcess( const Clock& txclock );
		virtual void RxProcess( const Clock& rxclock );	
		virtual void UpperLayerScheduleProcess( const Clock& clock );
	};
}