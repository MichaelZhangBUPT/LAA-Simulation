#include "commondef.h"
#include <vector>
#include "ASHARQEntity.h"
#include "UE.h"
#include "HARQEntity.h"
#include "ClockManager.h"
namespace LTESim
{
	ASHARQEntity::ASHARQEntity(shared_ptr<IUE> pUE, SystemType systemType ): HARQEntity(pUE, systemType)
	{
		m_iLoopHARQProcessNo = 0;
	}

	ASHARQEntity::~ASHARQEntity(void)
	{
	}

	void ASHARQEntity::UpdateHARQProcessNo()	//更新进程数
	{
		m_iCurrentProcessNo = GetMaxPriorityHARQProcessNo();
		//如果是新数据，那么循环进程的标志加一
		if ( m_iCurrentProcessNo == m_iLoopHARQProcessNo && m_vHARQList[m_iCurrentProcessNo]->GetNewPacketIndicator()) 
		{
			m_iLoopHARQProcessNo++;
			m_iLoopHARQProcessNo %= m_iMaxProcessNum;
		}
	}

	double ASHARQEntity::CalcPriorityOfHARQProcess( const shared_ptr<HARQ> & pHARQ )
	{
		//TBC
		int sysclk = ClockManager::Instance().GetSystemClock().GetClock();	//获得系统时间
		return ( pHARQ->GetNewPacketIndicator() ? 0 : (sysclk - pHARQ->GetInitTransTime()) ) * 100/*乘以100是为了重传的权值大一点*/
					+ ( ( pHARQ->GetProcessNo() == m_iLoopHARQProcessNo ) ? 1 : 0 );
		return ( pHARQ->GetTranTime() ? (sysclk - pHARQ->GetInitTransTime()) : 0) * 100/*乘以100是为了重传的权值大一点*/
			+ ( ( pHARQ->GetProcessNo() == m_iLoopHARQProcessNo ) ? 1 : 0 );
		throw UnFinishedFunc();
	}

	shared_ptr<HARQ> ASHARQEntity::GetNextHARQ()
	{
		int processno = GetMaxPriorityHARQProcessNo();
		return m_vHARQList[processno];
	}

	int ASHARQEntity::GetMaxPriorityHARQProcessNo()
	{
		//异步HARQ更新进程数原则：1：重传次数多的优先级高 2：满足重传时延的优先级高
		vector< shared_ptr<HARQ> > ::iterator iter;
		int processno = -1;
		double maxPriority = -1;
		for (iter = m_vHARQList.begin(); iter != m_vHARQList.end(); ++iter)
		{
			if (!(*iter)->BeReady())
			{
				continue;
			}
			double ProcessPriority = CalcPriorityOfHARQProcess(*iter);
			if (ProcessPriority > maxPriority)
			{
				processno = (*iter)->GetProcessNo();
				maxPriority = ProcessPriority;
			}
		}
		if (maxPriority < 0)//说明此时所有的HARQ进程都不可用（都在传输尚未反馈状态）
		{
			throw std::logic_error("HARQ进程总数和反馈时延不匹配，应当增大HARQ进程数");
		}
		return processno;
	}
	shared_ptr <HARQ> LTESim::ASHARQEntity::GetHARQ()
	{
		return m_vHARQList[m_iCurrentProcessNo];
	}
}
