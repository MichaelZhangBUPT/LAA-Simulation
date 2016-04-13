#include <boost/shared_ptr.hpp>
#include <iostream>
#include "Simulation.h"
#include "Configuration.h"
#include <windows.h>
using boost::shared_ptr;
int main(int argc, char* argv[])
{
	std::string configfile;
	if (argc == 1)
	{
		configfile = "../Initialization.ini";
	}
	else if (argc == 2)
	{
		configfile = argv[1];
	}
	else
	{
		std::cout << "Usage: LTEMain ConfigFileFullPathAndName\n" ;
		return 1;
	}
	using LTESim::Configuration;
	using LTESim::Simulation;
	shared_ptr<Configuration> pConfig(new Configuration(configfile.c_str()));
	shared_ptr<Simulation> pSimulation(new Simulation(*pConfig));
	pSimulation->Init();
	pSimulation->Run();
	return 0;
}