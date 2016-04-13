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
    //运行该Run
	virtual void RunThis();
	
	//结果输出
	virtual void ResultOutput();
#pragma region 内部函数
protected:

	//Run初始化
	virtual void Init();

	//运行第Drop个Drop
    virtual void RunDrop(int Drop);

	//将某个Drop的结果加到当前Run
	void AddResult( int drop, shared_ptr<SystemResult> dropresult);
#pragma endregion
protected:
    const shared_ptr<const RunParameters> m_parameters;
	shared_ptr<RunResult> m_pResult;//1个run的结果
	std::stringstream outputfile;
	//std::stringstream HSPAoutputfile;
	//std::stringstream WiFioutputfile;
};

}

