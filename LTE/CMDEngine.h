#pragma once
#include <list>

namespace LTESim
{
class Command;
class Clock;
//CMDEngine类，
/*
* Command命令对象执行器，Command::Do函数由此函数调用
* 负责保存Command对象并在要求时遍历执行
*/
class CMDEngine
{
public:
    typedef ::boost::shared_ptr<CMDEngine> PTR;
    typedef shared_ptr<Command> CommandPTR;
    ///构造函数
    CMDEngine(void);
public:
    ///析构函数
    virtual ~CMDEngine(void);

    //加入一个Command对象到命令队列中
    /*
    * @param cmd,要加入的命令对象
    */
    virtual void AddCommand(const CommandPTR& cmd);

    //执行队列中的Command
    virtual void Run(const Clock& clk );

protected:
    typedef std::list< CommandPTR > CMDLIST;

    //Command Engine需要保存并负责Command对象的删除，使用shared_ptr
    CMDLIST m_cmdlist;
};
}
