#include "commondef.h"
#include "Simulation.h"
#include "Configuration.h"
#include "RunParameters.h"
#include "Run.h"
#include "RandomManager.h"
namespace LTESim
{
Simulation::Simulation(Configuration& config)
        :m_configuration(config)   //读入仿真参数
{
}

Simulation::~Simulation(void)
{
}

void Simulation::Init()
{
	
 
}

void Simulation::Run()
{
    int TotalRunNum = m_configuration.GetRunNum();	//得到运行次数
    for (int run = 0; run < TotalRunNum; ++run)
    {
		//获得当前Run的参数并初始化一个Run
		const shared_ptr<const RunParameters> pConfig(m_configuration.GetRunParams(run + 1));//加1跳过default，0是default的标号
		LTESim::Run currentrun(pConfig);
        
		//运行一个Run
		currentrun.RunThis();
        
		//输出该Run的结果
		currentrun.ResultOutput();
    }
}
}
