#include "commondef.h"
#include "RunParameters.h"
#include "Value.h"
namespace LTESim
{
RunParameters::RunParameters(void)
{
}

RunParameters::~RunParameters(void)
{
}

int RunParameters::GetDrops() const
{
  return GetInt(string("Drop_Number"));
}

int RunParameters::GetDropTime() const
{
   return GetInt(string("Drop_Length(frame)"));
    return 0;
}


bool RunParameters::AddInt( const IntValue& value )
{
	m_IntValues[value.GetKey()] = value.GetValue();
	return true;
}

bool RunParameters::AddDouble( const DoubleValue& value )
{
	m_DoubleValues[value.GetKey()] = value.GetValue();
	return true;
}

bool RunParameters::AddBoolean( const BoolValue& value )
{
	m_BoolValues[value.GetKey()] = value.GetValue();
	return true;
}

bool RunParameters::AddString( const StringValue& value )
{
	m_StringValues[value.GetKey()] = value.GetValue();
	return true;
}

int RunParameters::GetInt( const string& key ) const
{
	if(!m_IntValues.count(key))
	{
		string s = "Can't find the int parameter with key = ";
		s += key;
		throw std::invalid_argument(s);
	}
	return m_IntValues[key];

}

double RunParameters::GetDouble( const string& key ) const
{
	if(!m_DoubleValues.count(key))
	{
		string s = "Can't find the double parameter with key = ";
		s += key;
		throw std::invalid_argument(s);
	}
	return m_DoubleValues[key];
}

bool RunParameters::GetBool( const string& key ) const
{
	if(!m_BoolValues.count(key))
	{
		string s = "Can't find the bool parameter with key = ";
		s += key;
		throw std::invalid_argument(s);
	}
	return m_BoolValues[key];
}

const string& RunParameters::GetString( const string& key ) const
{
	if(!m_StringValues.count(key))
	{
		string s = "Can't find the string parameter with key = ";
		s += key;
		throw std::invalid_argument(s);
	}
	return m_StringValues[key];

}

bool RunParameters::Save(std::ostream& savefile ) const
{
	savefile << "\nRun " << m_RunIndex << "\n";
	for(std::map< string, int>::iterator iter = m_IntValues.begin() ; iter != m_IntValues.end() ; ++iter)
		savefile << iter->first<< " = " << iter->second << "\n";
	for(std::map< string, double>::iterator iter = m_DoubleValues.begin() ; iter != m_DoubleValues.end() ; ++iter)
		savefile << iter->first<< " = " << iter->second << "\n";
	for(std::map< string, bool>::iterator iter = m_BoolValues.begin() ; iter != m_BoolValues.end() ; ++iter)
		savefile << iter->first<< " = " << ( iter->second ? "Yes" : "No" ) << "\n";
	for(std::map< string, string>::iterator iter = m_StringValues.begin() ; iter != m_StringValues.end() ; ++iter)
		savefile << iter->first<< " = " << iter->second << "\n";
	return savefile.good();
}
}
