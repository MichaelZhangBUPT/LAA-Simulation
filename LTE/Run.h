#pragma once
#include <sstream>
namespace LTESim
{
class RunParameters;
class SystemResult;
class RunResult;
class HSPARunResult;

class Run
{
public:
    Run(const shared_ptr<const RunParameters>& parameters);
public:
    virtual ~Run(void);
    //���и�Run
	virtual void RunThis();
	
	//������
	virtual void ResultOutput();
#pragma region �ڲ�����
protected:

	//Run��ʼ��
	virtual void Init();

	//���е�Drop��Drop
    virtual void RunDrop(int Drop);

	//��ĳ��Drop�Ľ���ӵ���ǰRun
	void AddResult( int drop, shared_ptr<SystemResult> dropresult);
#pragma endregion
protected:
    const shared_ptr<const RunParameters> m_parameters;
	shared_ptr<RunResult> m_pResult;//1��run�Ľ��
	std::stringstream outputfile;
	//std::stringstream HSPAoutputfile;
	//std::stringstream WiFioutputfile;
};

}

