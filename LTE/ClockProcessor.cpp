#include "commondef.h"
#include "ClockProcessor.h"
namespace LTESim
{
	ClockProcessor::ClockProcessor(void)
	{
	}

	ClockProcessor::~ClockProcessor(void)
	{
	}

	void ClockProcessor::OnClock( const Clock& clock )
	{
		//����Clock�����Ĳ�ͬ���ò�ͬ�ĺ�����TemplateMethod Pattern
		switch (clock.GetID())
		{
		case FRAMECLOCK :
			FrameProcess(clock);
			break;
		case TXCLOCK :
			TxProcess(clock);
			break;
		case RXCLOCK :
			RxProcess(clock);
			break;
		case TESTCLOCK:
			break;
		default:
			throw std::invalid_argument("Get UnKnown Clock ID ");
			break;
		}
	}
}

