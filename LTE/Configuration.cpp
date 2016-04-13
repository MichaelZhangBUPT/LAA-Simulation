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
	//Ĭ�ϲ���m_DefaultParameters���洢��ϵͳ������m_Parameters�У���m_DefaultParameters�Ļ���������ϵͳ����
	Configuration::Configuration( const char* configfilename ):m_RunNum(0)
	{
		m_config.open(configfilename, std::ios::in);
		if(m_config.fail())//�ļ��Ƿ��
		{
			throw std::logic_error("Can't open this file");
		}
		ReadDefault();//��ȡĬ�ϵ�ϵͳ����
		for (int i = 0; i < m_RunNum; ++i)//m_RunNum����ReadDefault()�л����ֵ
		{
			ReadRun(i);
		}
		m_config.close();//�ر��ļ�
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
		string CurrentLine;  //��Ŷ����һ���ַ�
		string flagbegin = sbegin; //��ʼ��ȡ��λ��
		string flagend = send; //��ȡ������λ��
		bool switcher = false; //��ȡ���أ��������ȡ��Χʱ��
		while (! (m_config.eof() || m_config.fail()) ) //�Ƿ��ļ�����
		{
			getline(m_config, CurrentLine); //��ȡ��ǰ��
			if (!CurrentLine.compare(flagbegin)) //�Ƿ��ǿ�ʼλ��
				switcher = true;
		
			else
			{
				if (!CurrentLine.compare(flagend)) //�Ƿ��ǽ���λ��
				{
					switcher = false;
					return true;
				}

				//Ϊ�ǿ����Ҵ��ڿ�ʼ������֮���Section
				else if (!CurrentLine.empty() && switcher && CurrentLine[0] != '[') 
				{
					stringstream ss(CurrentLine);//����ת���õ�sstream
					char equationtoken = '\0';//�����ȡ=
					string key = "\0";//�����ȡkey
					string valuetemp = "\0";//�����ȡvalue������Ϊstring�����ж�

					//����ǰ����ȡ����
					ss >> key >> equationtoken >> valuetemp;
					if (equationtoken == '=')
					{
						//����ȡ�ĵ�ǰ�е��ַ��������ռ�ֵ���࣬������Ե����ݳ�Ա��
						if(IsInteger( valuetemp ) )//�ж��Ƿ�Ϊ���� 
						{
							IntValue inttemp(CurrentLine);
							parameters.AddInt(inttemp);
						}
						else if ( IsFloat(valuetemp) )//�ж��Ƿ�Ϊ������
						{
							DoubleValue doubletemp(CurrentLine);
							parameters.AddDouble(doubletemp);
						}
						else if ( IsString(valuetemp) )//�ж��Ƿ�Ϊ�ַ���
						{
							if (IsBoolean((valuetemp)))//�ж��Ƿ�Ϊ������
							{
								BoolValue booltemp(CurrentLine);
								parameters.AddBoolean(booltemp);
							}
							else//������ǲ�����Ϊ�ַ���
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
		return false;//û������������������false
	}



	//���ı�����ʾ
	void Configuration::Save( ostream &savefile ) const
	{
		m_DefaultParameters->Save(savefile);//��ʾĬ�ϲ���
		for(std::vector< shared_ptr<RunParameters> >::const_iterator iter = m_Parameters.begin();//��ʾϵͳ���в���
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
		if (str.empty())//�Ƿ�Ϊ��
		{
			return false;
		}
		string::const_iterator iter = str.begin();

		//yrm std::vector<bool> IsAlpha(str.size());
		if (*iter == '-')//�ж��Ƿ�Ϊ����
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
		to_lower(s);//Ĭ��ΪСд
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
		boost::shared_ptr<RunParameters> RunParams( new RunParameters(*m_DefaultParameters) );//��Ĭ�ϲ���*m_DefaultParameters�Ļ����϶�ȡ�µ�ֵ
		RunParams->SetRunIndex(runindex);
		ReadSection(start,end, *RunParams);
		m_Parameters.push_back(RunParams);//����ϵͳ����
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
