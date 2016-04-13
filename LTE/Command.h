#pragma once
namespace LTESim
{
	class Clock;
	//�������Ϊ�˻�ÿռ��ʱ���ϵĽ����ʹ�ã�����Command����
	//��һ�����������ɲ����ݵ���һ�������У�����һ��ʱ�����ִ��
	class Command
	{
	public:
		typedef ::boost::shared_ptr<Command> PTR;
		//��������Ĺ��캯���п��Լ������ĳ�ʼ���������߸�������ú�������ɸ���Ĺ���
		Command(void);
	public:
		virtual ~Command(void);
		
		///Do�ӿ�
		/*
		* ִ���������涨�Ĳ������������������ʵ��
		* @param clk��������������Clock����
		*/
		virtual void Do( const Clock& clk ) = 0;
	};

}
