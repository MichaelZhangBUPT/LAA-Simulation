#pragma once
#include "commondef.h"
#include "AMCEntity.h"
#include "Service.h"
#include "mcs.h"

#include <map>
#include <vector>

namespace LTESim
{
	class ARQ
	{
	public:
		ARQ(shared_ptr<Service> pService);
		ARQ(){};
		virtual ~ARQ(void);
	public:
		void							SetService(shared_ptr<Service> pService);
		shared_ptr<Service>				GetService();

		void							UpdateTransStatus(bool bReady);	//更新进程传输状态，调度后未反馈时为false，可以使用作为调度时为true
		bool							BeReady() const;							//判断该进程是否已经在用于传输而没有返回
	
		void							UpdateTranTime();						//更新传输次数
		bool							MaxTransTime() const;					//
		int								GetTranTime()const;
		bool							GetNewPacketIndicator(void) const;
		
		const MCS&						GetMCS();
		void							SetMCS(MCS mcs);

		void							Reset();


	protected:
		weak_ptr<Service>	m_pService;												//该HARQ进程对应的业务
		
		//重传相关数据
		bool							m_bReady;													//是否可调度，对于已经调度上尚未反馈的进程为false，初始为true
		int							m_iMaxTransTime;										//最大传输次数
		int							m_iTranTime;								//传输次数 注意：2表示重传了一次
		MCS							m_MCS;											//MCS等级数				

	};
}
