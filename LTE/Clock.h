#pragma once
//#include <boost/enable_shared_from_this.hpp>
namespace LTESim
{
//前置声明
class Clock;

//需要使用时钟对象的接口，派生出ClockProcessor，进而派生出UE,NodeB
class TimeClient {
public:
	virtual ~TimeClient()
	{
	}
	
    /**时钟消息响应函数，当该对象注册的时钟触发时时钟会调用此函数*/
    virtual void OnClock( const Clock& clock) = 0;
};


//时钟类，用时钟驱动仿真运行
class Clock
{
protected:
    ///构造函数
	/*
    * 不允许直接使用时钟，通过ClockManager得到
    * @param ID, 该时钟的ID
    */
    Clock(int ID);
	friend class ClockManager;

public:
    ///析构函数
    virtual ~Clock(void);

    //时钟嘀嗒
    /*
    * 每嘀嗒一下内部计时器加steps
    */
    void Tick(int steps = 1);

    //时钟ID，用于时钟客户端收到时钟消息时分辨来源时钟
    int GetID() const;

    //获取当前时间（相对时间，即Subframe0~9，slot0~20）
    virtual int GetClock() const;
	
	//获得内部计数
    /*
    * 自时钟启动以来的计数数，0~...
    */
	int GetInnerClock() const;

    //将客户注册到此时钟
    void Register(const shared_ptr<TimeClient> client);

    //更新注册到此时钟的对象
    void Update();

    //清零内部客户队列和计数值，
    void Reset();
protected:
    //客户是否已经注册
    bool Exist( const shared_ptr<const TimeClient> client );
 
protected:
    //内部计时器
    int m_time;

    //注册到此时钟的客户，时钟不负责管理注册对象的删除，用weak_ptr实现
    std::vector< weak_ptr<TimeClient> > m_clients;

    int m_id;//时钟ID
};


inline void Clock::Tick(int steps)	
{
    m_time += steps;
}

inline int Clock::GetID() const	
{
    return m_id;
}
}

