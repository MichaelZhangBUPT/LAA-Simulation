#include "commondef.h"
#include "ClockReporter.h"
namespace LTESim
{


ClockReporter::ClockReporter(void)
{
}

ClockReporter::~ClockReporter(void)
{
}
//œ‘ æ ±÷”
void ClockReporter::OnClock( const Clock& clock )
{
    std::cout << clock.GetID()<< "\t" << clock.GetClock();
}
}
