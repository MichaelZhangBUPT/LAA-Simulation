#pragma once
#include <string>

namespace LTESim
{
	using std::string;
	//����������ֵ�ı���
	class IntValue  
	{
	public:
		IntValue (const string &stemp);
		const string& GetKey () const;//��ü���
		int GetValue () const;//��ü�ֵ

	protected:
		string m_Key;     //����
		int m_Value;      //��ֵ

	};

	//���渡������ֵ�ı���
	class DoubleValue
	{
	public:
		DoubleValue (const string &stemp);
		const string& GetKey () const;//��ü���
		double GetValue () const;//��ü�ֵ

	protected:
		string m_Key;     //����
		double m_Value;      //��ֵ
	};

	//�����ַ��͵ı���
	class StringValue
	{
	public:
		StringValue (const string &stemp);
		const string& GetKey () const;//��ü���
		string GetValue () const;//��ü�ֵ

	protected:
		string m_Key;     //����
		string m_Value;      //��ֵ
	};

	//���沼���͵ı���
	class BoolValue
	{
	public:
		BoolValue (const string &stemp);
		const string& GetKey () const;//��ü���
		bool GetValue () const;//��ü�ֵ

	protected:
		string m_Key;     //����
		bool m_Value;      //��ֵ

	};
};


