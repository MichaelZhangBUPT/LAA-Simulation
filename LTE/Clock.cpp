#include <stdexcept>
#include "commondef.h"
#include "Clock.h"
namespace LTESim
{

Clock::Clock( int ID/*时钟ID*/ )
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
        //按顺序通知所有注册到此Clock的时钟客户端
        if (shared_ptr<TimeClient> pClient = m_clients[i].lock())
        {
            pClient->OnClock(*this);
        }
        else
        {
            //如果该用户已经失效则去掉
            m_clients.erase(m_clients.begin()+i);
            //调整循环变量使得删除不会影响循环。
            --i;
        }

    }
}

void Clock::Register( const shared_ptr<TimeClient> client )
{
    if (!Exist(client))
    {
        //如果队列中没有此用户则加入
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
    /*清除用户队列*/
    m_clients.clear();
    /*重置计时*/
    m_time = 0;
}


/*判断用户是否已经注册的辅助函数*/
bool operator==(const weak_ptr<const TimeClient> client1, const shared_ptr<const TimeClient> client2 )
{
    return client1.lock().get() == client2.get();
}

/*判断用户是否存在*/
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
