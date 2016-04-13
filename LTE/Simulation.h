#pragma once
namespace LTESim
{
class Configuration;
class Simulation
{
public:
	//初始化Simulation,读入配置文件和外部数据文件
    Simulation( Configuration& config);
public:
    virtual ~Simulation(void);
	//随机种子初始化
    virtual void Init();

	//系统运行
    virtual void Run();
protected:
	//配置对象
    Configuration& m_configuration;
};

}
