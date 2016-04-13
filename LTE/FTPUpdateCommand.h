#pragma once
#include "Command.h"
#include "UE.h"
//#include "Service.h"
//
namespace LTESim
{
	/*更新ftp buffer & 间隔时间*/
	class ILTEUE;
	class DLFTPService;
	
	class FTPUpdateCommand :public Command
	{
	public:
		FTPUpdateCommand(shared_ptr<IUE> & pUE);
	public:
		~FTPUpdateCommand(void);
		void Do(const Clock& clk );
	protected:
		weak_ptr<IUE> m_pUE;
	};
}