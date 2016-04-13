#include "commondef.h"
#include <stdexcept>
#include "TimedCommand.h"
namespace LTESim
{

	TimedCommand::TimedCommand( const Clock& clock, shared_ptr<CMDEngine> pEngine, Command::PTR nativecmd )
			: CommandDecorator( nativecmd), m_cmdengine(pEngine), m_clk(clock)
	{

	}

	TimedCommand::~TimedCommand(void)
	{

	}

	void TimedCommand::SetTimer(int timer)
	{
		if (timer < 0)
		{
			throw std::logic_error("The timer must be set equal or greater than 0.\n");
		}
		//保存当前时间作为定时起点
		m_starttime = m_clk.GetInnerClock();
		//保存定时长度
		m_timelength = timer;
	}

	//判断计时器是否到时
	bool TimedCommand::TimeUp() const
	{
		return m_starttime + m_timelength == m_clk.GetInnerClock();
	}
	bool TimedCommand::TimeOut() const
	{
		return m_starttime + m_timelength <  m_clk.GetInnerClock();
	}

	void TimedCommand::Do(const Clock& clk)
	{
		if (clk.GetID() != m_clk.GetID())
		{
			AddBackToList();
			return;
		}
		if (TimeOut())
		{
			TimeOutProcess();
		}
		if ( TimeUp() )
			//到时则触发真实Do操作
			CommandDecorator::Do(clk);
		else
			AddBackToList();
	}

	//定时超出进行的操作
	void TimedCommand::TimeOutProcess()
	{
		USER_MESSAGE("The command time is out." << "The current clock is "
					 << m_clk.GetClock() << " and the due time is " << m_starttime + m_timelength << endl);
		throw std::logic_error("The command time is out");
	}

	void TimedCommand::AddBackToList()
	{
		//否则加入到Command队列继续等待
		if (CMDEngine::PTR pEngine = m_cmdengine.lock() )
		{
			Command::PTR pTHIS = shared_from_this();
			pEngine->AddCommand(pTHIS);
		}
		else
		{
			//如果engine已经失效，应该怎么办？
			//理论上是不会的，因为Do函数由engine触发，这时engine应当是有效的
			USER_MESSAGE("engine lost");
		}
	}

}



