#pragma once
#include "Command.h"
#include "Reporter.h"
#include "NodeB.h"

namespace LTESim
{
    ///UE上报命令
	class ReporterRxCommand :public LTESim::Command
	{
	public:
		typedef std::vector<double>	SINRLIST;
		typedef std::vector<MCS>		CQILIST;
        ///构造函数
		ReporterRxCommand(shared_ptr<ILTEUE> pUE, const SINRLIST & sinrlist, const CQILIST & cqilist);
	public:
        ///析构函数
		~ReporterRxCommand(void);
		
        ///Command::Do接口
		void Do(const Clock& clk);
	private:
		//要上报信息的UE
		weak_ptr <ILTEUE> m_pUE;
        //要上报的SINR
		SINRLIST m_vSINRList;

        //要上报的CQI
		CQILIST m_vCQIList;
	};
}

