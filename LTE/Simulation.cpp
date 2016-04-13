#include "commondef.h"
#include "Simulation.h"
#include "Configuration.h"
#include "RunParameters.h"
#include "Run.h"
#include "RandomManager.h"
namespace LTESim
{
Simulation::Simulation(Configuration& config)
        :m_configuration(config)   //����������
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
    int TotalRunNum = m_configuration.GetRunNum();	//�õ����д���
    for (int run = 0; run < TotalRunNum; ++run)
    {
		//��õ�ǰRun�Ĳ�������ʼ��һ��Run
		const shared_ptr<const RunParameters> pConfig(m_configuration.GetRunParams(run + 1));//��1����default��0��default�ı��
		LTESim::Run currentrun(pConfig);
        
		//����һ��Run
		currentrun.RunThis();
        
		//�����Run�Ľ��
		currentrun.ResultOutput();
    }
}
}
