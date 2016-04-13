#include "commondef.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "Value.h"
namespace LTESim
{

using std::stringstream;
//intvalue 的成员函数
IntValue::IntValue (const string &stemp)
{	
	stringstream ss(stemp);
	char equationtoken = '\0';
	ss >> m_Key >> equationtoken >> m_Value;
	
}

const string& IntValue::GetKey () const
{ 
	return m_Key;
}

int IntValue::GetValue () const
{
	return m_Value;
}

//doublevalue 的成员函数
DoubleValue::DoubleValue (const string &stemp)
{
	stringstream ss(stemp);
	char equationtoken = '\0';
	ss >> m_Key >> equationtoken >> m_Value;
		
}
const string& DoubleValue::GetKey () const
{
	return m_Key;
}
double DoubleValue::GetValue () const
{
	return m_Value;
}

//stringvalue 的成员函数
StringValue::StringValue (const string &stemp)
{
	stringstream ss(stemp);
	char equationtoken = '\0';
	ss >> m_Key >> equationtoken >> m_Value;
	
}
const string& StringValue::GetKey () const
{
	return m_Key;
}
string StringValue::GetValue () const
{
	return m_Value;
}


//boolvalue 的成员函数
BoolValue::BoolValue (const string &stemp)
{
	stringstream ss(stemp);
	char equationtoken = '\0';
	string ValueTemp;
	ss >> m_Key >> equationtoken >> ValueTemp;
	boost::to_lower(ValueTemp);
	if(ValueTemp == "yes")
		m_Value = true;
	else if (ValueTemp == "no")
	{
		m_Value = false;
	}
	else
	{
		throw std::invalid_argument("Bool Value must be Yes or No");
	}
		
	 
}
const string& BoolValue::GetKey () const
{
	return m_Key;
}
bool BoolValue::GetValue () const
{
	return m_Value;
}


};
