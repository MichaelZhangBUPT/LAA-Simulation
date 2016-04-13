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

	//������������
	void Save( ostream &savefile ) const; 

	//�õ�ָ��Run�Ĳ���
	const shared_ptr<const RunParameters> GetRunParams(unsigned int run) const;
	const shared_ptr<const RunParameters> GetDefaultParams() const;	
	//�õ����õ�Run��Ŀ
    virtual int GetRunNum() const;
	virtual unsigned int GetSeed() const;
protected:
	 //���ı���һ����Χ�ڶ�ȡ����ֵ����Χ��sbegin�����е�send������
	bool ReadSection(const string& sbegin, const string& send, RunParameters& parameters );

	//��ȡ#default��#enddefault֮�������
	bool ReadDefault();

	//��ȡ#run?��#endrun?֮�������
	bool ReadRun( unsigned int runindex );
	//����ֵ���ͺ���
	bool IsString( const string& str ) const;
	bool IsInteger( const string& str ) const;
	bool IsBoolean( const string& str ) const;
	bool IsFloat( const string& str ) const;
	

protected:
	std::fstream m_config; //�����ļ�		
	boost::shared_ptr<RunParameters> m_DefaultParameters;//�洢Ĭ�ϵ�ϵͳ����
	int m_RunNum;
	unsigned int m_seed;
	std::vector< boost::shared_ptr<RunParameters> > m_Parameters;//�洢ϵͳ����ʱ����Ҫ��һ�����

};

}
