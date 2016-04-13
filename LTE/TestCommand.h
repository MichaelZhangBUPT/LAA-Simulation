#pragma  once
#include "command.h"
namespace LTESim
{

//���ڲ��Ե��������
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
        //�ۼ�ִ�д���
        ++m_executedtimes;
        //��ӡ��NodeB��ID��λ��
        cout << "This is an example command!\nTo display NodeB's ID and position\n";
        if (NodeB::PTR pNodeB = m_nodeb.lock())
        {
            cout << "NodeB:" << pNodeB->GetID() << "\tPos:" << pNodeB->GetPos() << endl;
        }
    }
    //���
    int Flag() const
    {
        return m_executedtimes;
    }
protected:
    weak_ptr<NodeB> m_nodeb;
    int m_executedtimes;//���ִ�й��Ĵ���
};

}
