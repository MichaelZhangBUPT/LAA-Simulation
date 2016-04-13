#pragma  once
#include "command.h"
namespace LTESim
{

//用于测试的命令对象
class TestCommand : public Command
{
public:
    TestCommand(NodeB::PTR& nb):m_nodeb(nb),m_executedtimes(0)
    {

    }
    ~TestCommand()
    {
    }
	virtual void Do(const Clock& clk)
    {
        //累加执行次数
        ++m_executedtimes;
        //打印出NodeB的ID和位置
        cout << "This is an example command!\nTo display NodeB's ID and position\n";
        if (NodeB::PTR pNodeB = m_nodeb.lock())
        {
            cout << "NodeB:" << pNodeB->GetID() << "\tPos:" << pNodeB->GetPos() << endl;
        }
    }
    //命令被
    int Flag() const
    {
        return m_executedtimes;
    }
protected:
    weak_ptr<NodeB> m_nodeb;
    int m_executedtimes;//命令被执行过的次数
};

}
