#pragma once
namespace LTESim
{
class Configuration;
class Simulation
{
public:
	//��ʼ��Simulation,���������ļ����ⲿ�����ļ�
    Simulation( Configuration& config);
public:
    virtual ~Simulation(void);
	//������ӳ�ʼ��
    virtual void Init();

	//ϵͳ����
    virtual void Run();
protected:
	//���ö���
    Configuration& m_configuration;
};

}
