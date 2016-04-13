#pragma once
//#include <boost/enable_shared_from_this.hpp>
namespace LTESim
{
//ǰ������
class Clock;

//��Ҫʹ��ʱ�Ӷ���Ľӿڣ�������ClockProcessor������������UE,NodeB
class TimeClient {
public:
	virtual ~TimeClient()
	{
	}
	
    /**ʱ����Ϣ��Ӧ���������ö���ע���ʱ�Ӵ���ʱʱ�ӻ���ô˺���*/
    virtual void OnClock( const Clock& clock) = 0;
};


//ʱ���࣬��ʱ��������������
class Clock
{
protected:
    ///���캯��
	/*
    * ������ֱ��ʹ��ʱ�ӣ�ͨ��ClockManager�õ�
    * @param ID, ��ʱ�ӵ�ID
    */
    Clock(int ID);
	friend class ClockManager;

public:
    ///��������
    virtual ~Clock(void);

    //ʱ�����
    /*
    * ÿ���һ���ڲ���ʱ����steps
    */
    void Tick(int steps = 1);

    //ʱ��ID������ʱ�ӿͻ����յ�ʱ����Ϣʱ�ֱ���Դʱ��
    int GetID() const;

    //��ȡ��ǰʱ�䣨���ʱ�䣬��Subframe0~9��slot0~20��
    virtual int GetClock() const;
	
	//����ڲ�����
    /*
    * ��ʱ�����������ļ�������0~...
    */
	int GetInnerClock() const;

    //���ͻ�ע�ᵽ��ʱ��
    void Register(const shared_ptr<TimeClient> client);

    //����ע�ᵽ��ʱ�ӵĶ���
    void Update();

    //�����ڲ��ͻ����кͼ���ֵ��
    void Reset();
protected:
    //�ͻ��Ƿ��Ѿ�ע��
    bool Exist( const shared_ptr<const TimeClient> client );
 
protected:
    //�ڲ���ʱ��
    int m_time;

    //ע�ᵽ��ʱ�ӵĿͻ���ʱ�Ӳ��������ע������ɾ������weak_ptrʵ��
    std::vector< weak_ptr<TimeClient> > m_clients;

    int m_id;//ʱ��ID
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

