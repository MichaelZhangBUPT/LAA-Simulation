#pragma once
#include <boost/enable_shared_from_this.hpp>
#include "Command.h"
#include "CommandDecorator.h"
#include "CMDEngine.h"
#include "Clock.h"
namespace LTESim
{
//��ʱCommand���󣬻��Զ��ж�ִ��ʱ�Ƿ�ʱ�ӵ�ʱ��δ��ʱ����뵽Command Engine�Ķ���ĩ�����ȴ�
//����һ�����õ�Command�����ڶ�ʱ����ʱ���������
class TimedCommand :
            public CommandDecorator, 
			public boost::enable_shared_from_this<TimedCommand>
{
public:
    typedef shared_ptr<TimedCommand> PTR;
    //���캯��
    /*
    * @param clock, ��ʱ����ʱ��
    * @param pEngine���󶨵�CMDEngine���󣬵�δ��ʱʱ�ӻص��˶��������ȴ�����
    * @param nativecmd���ڲ�������󣬶�ʱ�����ִ��nativecmd��Do����
    */
	TimedCommand( const Clock& clock, 
                  shared_ptr<CMDEngine> pEngine,
		          Command::PTR nativecmd = Command::PTR());
public:
    ///��������
    virtual ~TimedCommand(void);

    //Command�ӿ�
    virtual void Do(const Clock& clk);

	///��������ִ�ж�ʱ
	/*
	* @param timer����λΪע�ᵽ��Clock����ĵ�λ  
	* Ҫ���ʱ���ڴ���0�������׳�logic_error
	*/
	void SetTimer(int timer/*��ʱ����*/);

protected:
    //�ж϶�ʱ����ĸ�������
    /*
    * @return ע��clockʱ��ǡ�������õĶ�ʱ��ʱ����true������false
    */
    virtual bool TimeUp() const;

    //�ж϶�ʱ�����ĸ�������
    /*
    * @return ע��clockʱ�̳������õĶ�ʱʱ����true������false��
    * ������Ӧ�÷���
    */
    virtual bool TimeOut() const;

    //��ʱ��ʱ���еĲ���
    virtual void TimeOutProcess();
	
    ///Clockʱ����δ���ﶨʱʱ��ʱ������ӻص�CMDEngine�м����ȴ�
	virtual void AddBackToList();

protected:
    const Clock& m_clk;								//ʹ�õļ�ʱ��
    int m_timelength;									//��ʱ����
    int m_starttime;										//��ʱ����ʱ��
    weak_ptr<CMDEngine> m_cmdengine;	//������engine
};

}
