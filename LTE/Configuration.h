#pragma once
#include <vector>
#include <fstream>
namespace LTESim
{
using std::fstream;
using std::vector;
using std::ofstream;
using std::ifstream;
using std::ostream;
using std::string;
using boost::shared_ptr;
class RunParameters;
class Configuration
{
public:
    Configuration(const char* configfilename);


public:
    virtual ~Configuration(void);

	//输出到输出流中
	void Save( ostream &savefile ) const; 

	//得到指定Run的参数
	const shared_ptr<const RunParameters> GetRunParams(unsigned int run) const;
	const shared_ptr<const RunParameters> GetDefaultParams() const;	
	//得到配置的Run数目
    virtual int GetRunNum() const;
	virtual unsigned int GetSeed() const;
protected:
	 //从文本的一定范围内读取所需值，范围是sbegin所在行到send所在行
	bool ReadSection(const string& sbegin, const string& send, RunParameters& parameters );

	//读取#default到#enddefault之间的内容
	bool ReadDefault();

	//读取#run?到#endrun?之间的内容
	bool ReadRun( unsigned int runindex );
	//测试值类型函数
	bool IsString( const string& str ) const;
	bool IsInteger( const string& str ) const;
	bool IsBoolean( const string& str ) const;
	bool IsFloat( const string& str ) const;
	

protected:
	std::fstream m_config; //配置文件		
	boost::shared_ptr<RunParameters> m_DefaultParameters;//存储默认的系统参数
	int m_RunNum;
	unsigned int m_seed;
	std::vector< boost::shared_ptr<RunParameters> > m_Parameters;//存储系统运行时所需要的一组参数

};

}
