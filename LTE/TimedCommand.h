#pragma once
#include <boost/enable_shared_from_this.hpp>
#include "Command.h"
#include "CommandDecorator.h"
#include "CMDEngine.h"
#include "Clock.h"
namespace LTESim
{
//定时Command对象，会自动判断执行时是否时钟到时，未到时则加入到Command Engine的队列末继续等待
//包含一个内置的Command对象，在定时到达时调用其操作
class TimedCommand :
            public CommandDecorator, 
			public boost::enable_shared_from_this<TimedCommand>
{
public:
    typedef shared_ptr<TimedCommand> PTR;
    //构造函数
    /*
    * @param clock, 定时控制时钟
    * @param pEngine，绑定的CMDEngine对象，当未到时时加回到此对象的命令等待队列
    * @param nativecmd，内部命令对象，定时到达后执行nativecmd的Do函数
    */
	TimedCommand( const Clock& clock, 
                  shared_ptr<CMDEngine> pEngine,
		          Command::PTR nativecmd = Command::PTR());
public:
    ///析构函数
    virtual ~TimedCommand(void);

    //Command接口
    virtual void Do(const Clock& clk);

	///设置命令执行定时
	/*
	* @param timer，单位为注册到的Clock对象的单位  
	* 要求计时周期大于0，否则抛出logic_error
	*/
	void SetTimer(int timer/*计时周期*/);

protected:
    //判断定时到达的辅助函数
    /*
    * @return 注册clock时刻恰好是设置的定时点时返回true，否则false
    */
    virtual bool TimeUp() const;

    //判断定时超出的辅助函数
    /*
    * @return 注册clock时刻超过设置的定时时返回true，否则false，
    * 此现象不应该发生
    */
    virtual bool TimeOut() const;

    //定时过时进行的操作
    virtual void TimeOutProcess();
	
    ///Clock时刻尚未到达定时时刻时将自身加回到CMDEngine中继续等待
	virtual void AddBackToList();

protected:
    const Clock& m_clk;								//使用的计时器
    int m_timelength;									//定时长度
    int m_starttime;										//定时启动时间
    weak_ptr<CMDEngine> m_cmdengine;	//关联的engine
};

}
