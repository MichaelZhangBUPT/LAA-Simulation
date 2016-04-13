#pragma once
#include <list>

namespace LTESim
{
class Command;
class Clock;
//CMDEngine�࣬
/*
* Command�������ִ������Command::Do�����ɴ˺�������
* ���𱣴�Command������Ҫ��ʱ����ִ��
*/
class CMDEngine
{
public:
    typedef ::boost::shared_ptr<CMDEngine> PTR;
    typedef shared_ptr<Command> CommandPTR;
    ///���캯��
    CMDEngine(void);
public:
    ///��������
    virtual ~CMDEngine(void);

    //����һ��Command�������������
    /*
    * @param cmd,Ҫ������������
    */
    virtual void AddCommand(const CommandPTR& cmd);

    //ִ�ж����е�Command
    virtual void Run(const Clock& clk );

protected:
    typedef std::list< CommandPTR > CMDLIST;

    //Command Engine��Ҫ���沢����Command�����ɾ����ʹ��shared_ptr
    CMDLIST m_cmdlist;
};
}
