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

    //������δ���������ִ�У�ע�����ﲢ����ֱ����m_cmdlist.size()���жϵģ�
    //��Ϊ�е�Command����δ��ʱ������½�������뵽����β�����ʹ��size()==0��
    //�жϷ�������ʹ�ö���ʼ�����в��꣬���������ֻ����ÿ�ν���Runʱ���������е�Command����
    while ( listsize-- )
    {
        //POPȡ������ͷ��ִ��
        CommandPTR pCMD = m_cmdlist.front();
        m_cmdlist.pop_front();
        pCMD->Do( clk );
    }
}
}
