#include "commondef.h"
#include "CMDEngine.h"
#include "Command.h"
namespace LTESim
{

CMDEngine::CMDEngine(void)
{
}

CMDEngine::~CMDEngine(void)
{
    m_cmdlist.clear();
}

void CMDEngine::AddCommand( const CommandPTR& cmd )
{
    m_cmdlist.push_back(cmd);
}

void CMDEngine::Run( const Clock& clk )
{
    CMDLIST::size_type listsize = m_cmdlist.size();

    //当队列未处理完继续执行，注意这里并不是直接用m_cmdlist.size()来判断的，
    //因为有的Command会在未到时的情况下将自身加入到队列尾，如果使用size()==0的
    //判断方法，会使得队列始终运行不完，因此在这里只处理每次进入Run时队列中已有的Command对象
    while ( listsize-- )
    {
        //POP取出队列头并执行
        CommandPTR pCMD = m_cmdlist.front();
        m_cmdlist.pop_front();
        pCMD->Do( clk );
    }
}
}
