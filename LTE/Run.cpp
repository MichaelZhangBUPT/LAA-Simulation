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
		//����Ҫʹ�õ�����ļ�������ļ�����
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
		//��ʼ��Run
		Init();
		std::cout << "Start Run " << m_pResult->GetTitle() << std::endl;
		for ( int Drop = 0; Drop < m_parameters->GetDrops(); ++Drop )
		{
			//����ָ��Drop
			cout << endl << "Start Drop " << Drop + 1  << "/" << m_parameters->GetDrops() <<  endl;
			RunDrop(Drop);
			cout << endl << "End of Drop " << Drop + 1  << "/" << m_parameters->GetDrops() <<  endl;
		}
		std::cout << "End of Run " << m_pResult->GetTitle() << std::endl;
	}

	void Run::RunDrop( int Drop )
	{
		//��һ��Drop

		//��ʼ��һ��System
		shared_ptr<System> pSystem(new System(m_parameters));
		pSystem->Init();

		//��������֡
		pSystem->Run( m_parameters->GetDropTime() );

		pSystem->End();

		//���������Run
		AddResult( Drop, pSystem->GetResult());
	}

	void Run::AddResult( int drop, shared_ptr<SystemResult> dropresult )
	{
		//��һ��Drop�Ľ�����뵽Run��
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
		//�����Run�Ľ��
		std::ofstream ofile(outputfile.str().c_str(), std::ios::app);
		m_pResult->WriteTo(ofile);
		//std::ofstream wifiofile(WiFioutputfile.str().c_str(), std::ios::app);
		//m_pResult->WriteWiFiTo(wifiofile);
	}

}
