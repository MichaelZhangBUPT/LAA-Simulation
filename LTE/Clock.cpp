#include <stdexcept>
#include "commondef.h"
#include "Clock.h"
namespace LTESim
{

Clock::Clock( int ID/*ʱ��ID*/ )
        :m_id(ID),m_time(0)
{

}
Clock::~Clock(void)
{
}

void Clock::Update()
{
    for (unsigned int i = 0; i < m_clients.size(); ++i)
    {
        //��˳��֪ͨ����ע�ᵽ��Clock��ʱ�ӿͻ���
        if (shared_ptr<TimeClient> pClient = m_clients[i].lock())
        {
            pClient->OnClock(*this);
        }
        else
        {
            //������û��Ѿ�ʧЧ��ȥ��
            m_clients.erase(m_clients.begin()+i);
            //����ѭ������ʹ��ɾ������Ӱ��ѭ����
            --i;
        }

    }
}

void Clock::Register( const shared_ptr<TimeClient> client )
{
    if (!Exist(client))
    {
        //���������û�д��û������
        m_clients.push_back(client);
    }
    else
    {
        USER_MESSAGE("Time Client is already in the list.\n");
        throw std::logic_error("Time Client is already in the list.");
    }

}

void Clock::Reset()
{
    /*����û�����*/
    m_clients.clear();
    /*���ü�ʱ*/
    m_time = 0;
}


/*�ж��û��Ƿ��Ѿ�ע��ĸ�������*/
bool operator==(const weak_ptr<const TimeClient> client1, const shared_ptr<const TimeClient> client2 )
{
    return client1.lock().get() == client2.get();
}

/*�ж��û��Ƿ����*/
bool Clock::Exist( const shared_ptr<const TimeClient> client )
{
    return std::find(m_clients.begin(), m_clients.end(), client) != m_clients.end();
}

int Clock::GetClock() const
{
	return m_time;
}

int Clock::GetInnerClock() const
{
	return m_time;
}
};
