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
		//���浱ǰʱ����Ϊ��ʱ���
		m_starttime = m_clk.GetInnerClock();
		//���涨ʱ����
		m_timelength = timer;
	}

	//�жϼ�ʱ���Ƿ�ʱ
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
			//��ʱ�򴥷���ʵDo����
			CommandDecorator::Do(clk);
		else
			AddBackToList();
	}

	//��ʱ�������еĲ���
	void TimedCommand::TimeOutProcess()
	{
		USER_MESSAGE("The command time is out." << "The current clock is "
					 << m_clk.GetClock() << " and the due time is " << m_starttime + m_timelength << endl);
		throw std::logic_error("The command time is out");
	}

	void TimedCommand::AddBackToList()
	{
		//������뵽Command���м����ȴ�
		if (CMDEngine::PTR pEngine = m_cmdengine.lock() )
		{
			Command::PTR pTHIS = shared_from_this();
			pEngine->AddCommand(pTHIS);
		}
		else
		{
			//���engine�Ѿ�ʧЧ��Ӧ����ô�죿
			//�������ǲ���ģ���ΪDo������engine��������ʱengineӦ������Ч��
			USER_MESSAGE("engine lost");
		}
	}

}



