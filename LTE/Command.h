#pragma once
namespace LTESim
{
	class Clock;
	//命令对象，为了获得空间和时间上的解耦合使用，这样Command可以
	//在一个对象中生成并传递到另一个对象中，在另一个时间点上执行
	class Command
	{
	public:
		typedef ::boost::shared_ptr<Command> PTR;
		//在派生类的构造函数中可以加入更多的初始化参数或者更多的设置函数以完成更多的功能
		Command(void);
	public:
		virtual ~Command(void);
		
		///Do接口
		/*
		* 执行命令对象规定的操作，具体操作由子类实现
		* @param clk，触发命令对象的Clock对象
		*/
		virtual void Do( const Clock& clk ) = 0;
	};

}
