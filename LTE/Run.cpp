#include "commondef.h"
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <fstream>
#include <string>
#include "Run.h"
#include "RunParameters.h"
#include "System.h"
#include "RunResult.h"
#include "SystemResult.h"
#include "RandomManager.h"

namespace LTESim
{
	Run::Run( const shared_ptr<const RunParameters>& parameters )
		:m_parameters(parameters), m_pResult(new RunResult)
	{

	}

	Run::~Run(void)
	{
	}

	void Run::Init()
	{
		//生成要使用的输出文件并清空文件内容
		m_pResult->SetTitle(m_parameters->GetString("Name"));
		outputfile << m_parameters->GetString("RESULT_FILE_LOCATION") << "\\"
			<< m_parameters->GetString("Name") 
			<< ".txt";
		std::ofstream ofile(outputfile.str().c_str(), std::ios::app);
		ofile.close();
		//HSPAoutputfile << m_parameters->GetString("RESULT_FILE_LOCATION") << "\\HSPA\\"
		//	<< m_parameters->GetString("Name") 
		//	<< ".txt";
		//std::ofstream ofile2(HSPAoutputfile.str().c_str(), std::ios::app);
		//ofile2.close();

		//WiFioutputfile << m_parameters->GetString("RESULT_FILE_LOCATION") << "\\"
		//	<< m_parameters->GetString("Name") << "_WiFi.txt";
		//std::ofstream ofile3(WiFioutputfile.str().c_str(), std::ios::app);
		//ofile3.close();

		int seed = m_parameters->GetInt("RUN_SEED");
		if (seed >= 0)
		{
            RandomManager::GetRandomManager().Seed( (unsigned int) seed);
		}
	}

	void Run::RunThis()
	{
		//初始化Run
		Init();
		std::cout << "Start Run " << m_pResult->GetTitle() << std::endl;
		for ( int Drop = 0; Drop < m_parameters->GetDrops(); ++Drop )
		{
			//运行指定Drop
			cout << endl << "Start Drop " << Drop + 1  << "/" << m_parameters->GetDrops() <<  endl;
			RunDrop(Drop);
			cout << endl << "End of Drop " << Drop + 1  << "/" << m_parameters->GetDrops() <<  endl;
		}
		std::cout << "End of Run " << m_pResult->GetTitle() << std::endl;
	}

	void Run::RunDrop( int Drop )
	{
		//跑一个Drop

		//初始化一个System
		shared_ptr<System> pSystem(new System(m_parameters));
		pSystem->Init();

		//运行若干帧
		pSystem->Run( m_parameters->GetDropTime() );

		pSystem->End();

		//将结果加入Run
		AddResult( Drop, pSystem->GetResult());
	}

	void Run::AddResult( int drop, shared_ptr<SystemResult> dropresult )
	{
		//将一个Drop的结果加入到Run中
		std::stringstream ss;
		ss << "Drop "<< drop;
		dropresult->SetTitle(ss.str());
		m_pResult->AddResult(*dropresult);
		//m_pResult->AddWiFiUEThr(*dropresult);

		std::ofstream ofile(outputfile.str().c_str(), std::ios::app);
		dropresult->WriteTo(ofile);
		//std::ofstream wifiofile(WiFioutputfile.str().c_str(), std::ios::app);
		//dropresult->WriteWiFiTo(wifiofile);

		return;
	}

	void Run::ResultOutput()
	{
		//输出该Run的结果
		std::ofstream ofile(outputfile.str().c_str(), std::ios::app);
		m_pResult->WriteTo(ofile);
		//std::ofstream wifiofile(WiFioutputfile.str().c_str(), std::ios::app);
		//m_pResult->WriteWiFiTo(wifiofile);
	}

}
