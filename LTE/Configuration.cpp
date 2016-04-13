#include "commondef.h"
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <numeric>
#include "Configuration.h"
#include "RunParameters.h"
#include "Value.h"
using std::cout;
using std::endl;
using std::stringstream;

using boost::to_lower;

namespace LTESim
{
	//默认参数m_DefaultParameters不存储在系统参数表m_Parameters中，在m_DefaultParameters的基础上设置系统参数
	Configuration::Configuration( const char* configfilename ):m_RunNum(0)
	{
		m_config.open(configfilename, std::ios::in);
		if(m_config.fail())//文件是否打开
		{
			throw std::logic_error("Can't open this file");
		}
		ReadDefault();//读取默认的系统参数
		for (int i = 0; i < m_RunNum; ++i)//m_RunNum可在ReadDefault()中获得新值
		{
			ReadRun(i);
		}
		m_config.close();//关闭文件
	}



	Configuration::~Configuration( void )
	{

	}

	int Configuration::GetRunNum() const
	{
		return m_RunNum;	
	}

	bool Configuration::ReadSection( const string& sbegin, const string& send, RunParameters& parameters )
	{
		string CurrentLine;  //存放读入的一行字符
		string flagbegin = sbegin; //开始读取的位置
		string flagend = send; //读取结束的位置
		bool switcher = false; //读取开关，当进入读取范围时打开
		while (! (m_config.eof() || m_config.fail()) ) //是否到文件结束
		{
			getline(m_config, CurrentLine); //获取当前行
			if (!CurrentLine.compare(flagbegin)) //是否是开始位置
				switcher = true;
		
			else
			{
				if (!CurrentLine.compare(flagend)) //是否是结束位置
				{
					switcher = false;
					return true;
				}

				//为非空行且处于开始结束符之间非Section
				else if (!CurrentLine.empty() && switcher && CurrentLine[0] != '[') 
				{
					stringstream ss(CurrentLine);//进行转换用的sstream
					char equationtoken = '\0';//负责读取=
					string key = "\0";//负责读取key
					string valuetemp = "\0";//负责读取value，保存为string，再判断

					//将当前行提取出来
					ss >> key >> equationtoken >> valuetemp;
					if (equationtoken == '=')
					{
						//将读取的当前行的字符串，按照键值分类，存入相对的数据成员中
						if(IsInteger( valuetemp ) )//判断是否为整型 
						{
							IntValue inttemp(CurrentLine);
							parameters.AddInt(inttemp);
						}
						else if ( IsFloat(valuetemp) )//判断是否为浮点型
						{
							DoubleValue doubletemp(CurrentLine);
							parameters.AddDouble(doubletemp);
						}
						else if ( IsString(valuetemp) )//判断是否为字符型
						{
							if (IsBoolean((valuetemp)))//判断是否为布尔型
							{
								BoolValue booltemp(CurrentLine);
								parameters.AddBoolean(booltemp);
							}
							else//如果不是布尔则为字符型
							{
								StringValue stringtemp(CurrentLine);
								parameters.AddString(stringtemp);
							}
						}
						else
						{
							throw std::invalid_argument("Value type can't be determined.");
						}
					}
				}
			}
		}
		return false;//没有遇到结束符，返回false
	}



	//在文本中显示
	void Configuration::Save( ostream &savefile ) const
	{
		m_DefaultParameters->Save(savefile);//显示默认参数
		for(std::vector< shared_ptr<RunParameters> >::const_iterator iter = m_Parameters.begin();//显示系统运行参数
			iter != m_Parameters.end();
			++iter)
		{
			(*iter)->Save(savefile);
		}
	}

	bool Configuration::IsString( const string& str ) const
	{
		//yrm string::iterator iter;
		using namespace boost::lambda;
		//yrm std::vector<bool> IsAlpha(str.size());
		bool result = std::accumulate(str.begin(), str.end(),true,( _1 && bind(isgraph, _2)));
		return result;
	}

	bool Configuration::IsInteger( const string& str ) const
	{
		if (str.empty())//是否为空
		{
			return false;
		}
		string::const_iterator iter = str.begin();

		//yrm std::vector<bool> IsAlpha(str.size());
		if (*iter == '-')//判断是否为负数
		{
			++iter;
		}
		using namespace boost::lambda;
		bool result = std::accumulate(iter, str.end(),true,( _1 && bind(isdigit, _2)  ));
		return result;
	}

	bool Configuration::IsBoolean( const string& str ) const
	{
		if (str.empty())
		{
			return false;
		}
		string s(str);
		to_lower(s);//默认为小写
		return s == "yes" || s == "no";
	}

	bool Configuration::IsFloat( const string& str ) const
	{
		if (str.empty())
		{
			return false;
		}
		string::const_iterator iter = str.begin();
		if (*iter == '-')
		{
			++iter;
		}
		using namespace boost::lambda;
		bool result = std::accumulate(iter, str.end(),true,( _1 && ( bind(isdigit, _2) || _2 == '.') ));
		return result;
	}

	bool Configuration::ReadDefault()
	{
		m_DefaultParameters.reset(new RunParameters);
		m_DefaultParameters->SetRunIndex(0);
		ReadSection("#default", "#end_default", *m_DefaultParameters);
		m_RunNum = m_DefaultParameters->GetInt(string("Run_Number"));
		m_seed = m_DefaultParameters->GetInt(string("SEED"));
		return true;
	}

	bool Configuration::ReadRun( unsigned int runindex )
	{
		string c = boost::lexical_cast<string>(runindex + 1);		
		string start = "#run" + c ;
		string end = "#endrun" + c ;	
		boost::shared_ptr<RunParameters> RunParams( new RunParameters(*m_DefaultParameters) );//在默认参数*m_DefaultParameters的基础上读取新的值
		RunParams->SetRunIndex(runindex);
		ReadSection(start,end, *RunParams);
		m_Parameters.push_back(RunParams);//存入系统参数
		return true;
	}
	const shared_ptr<const RunParameters> Configuration::GetRunParams( unsigned int run ) const
	{
		if (run > m_Parameters.size())
		{
			throw std::out_of_range("Run index out of range");
		}
		if (run == 0)
		{
			return GetDefaultParams();
		}
		return m_Parameters[run-1];
	}
	const shared_ptr<const RunParameters> Configuration::GetDefaultParams() const
	{
		return m_DefaultParameters;
	}

	unsigned int Configuration::GetSeed() const
	{
		return m_seed;
	}

}
