#pragma once
#include "Command.h"
#include "Reporter.h"
#include "NodeB.h"

namespace LTESim
{
    ///UE�ϱ�����
	class ReporterRxCommand :public LTESim::Command
	{
	public:
		typedef std::vector<double>	SINRLIST;
		typedef std::vector<MCS>		CQILIST;
        ///���캯��
		ReporterRxCommand(shared_ptr<ILTEUE> pUE, const SINRLIST & sinrlist, const CQILIST & cqilist);
	public:
        ///��������
		~ReporterRxCommand(void);
		
        ///Command::Do�ӿ�
		void Do(const Clock& clk);
	private:
		//Ҫ�ϱ���Ϣ��UE
		weak_ptr <ILTEUE> m_pUE;
        //Ҫ�ϱ���SINR
		SINRLIST m_vSINRList;

        //Ҫ�ϱ���CQI
		CQILIST m_vCQIList;
	};
}

