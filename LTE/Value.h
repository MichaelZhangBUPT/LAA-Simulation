#pragma once
#include <string>

namespace LTESim
{
	using std::string;
	//储存整型数值的变量
	class IntValue  
	{
	public:
		IntValue (const string &stemp);
		const string& GetKey () const;//获得键名
		int GetValue () const;//获得键值

	protected:
		string m_Key;     //键名
		int m_Value;      //键值

	};

	//储存浮点型数值的变量
	class DoubleValue
	{
	public:
		DoubleValue (const string &stemp);
		const string& GetKey () const;//获得键名
		double GetValue () const;//获得键值

	protected:
		string m_Key;     //键名
		double m_Value;      //键值
	};

	//储存字符型的变量
	class StringValue
	{
	public:
		StringValue (const string &stemp);
		const string& GetKey () const;//获得键名
		string GetValue () const;//获得键值

	protected:
		string m_Key;     //键名
		string m_Value;      //键值
	};

	//储存布尔型的变量
	class BoolValue
	{
	public:
		BoolValue (const string &stemp);
		const string& GetKey () const;//获得键名
		bool GetValue () const;//获得键值

	protected:
		string m_Key;     //键名
		bool m_Value;      //键值

	};
};


