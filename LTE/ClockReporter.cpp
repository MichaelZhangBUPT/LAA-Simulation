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
//��ʾʱ��
void ClockReporter::OnClock( const Clock& clock )
{
    std::cout << clock.GetID()<< "\t" << clock.GetClock();
}
}
