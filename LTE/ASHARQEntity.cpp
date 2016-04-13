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

	void ASHARQEntity::UpdateHARQProcessNo()	//���½�����
	{
		m_iCurrentProcessNo = GetMaxPriorityHARQProcessNo();
		//����������ݣ���ôѭ�����̵ı�־��һ
		if ( m_iCurrentProcessNo == m_iLoopHARQProcessNo && m_vHARQList[m_iCurrentProcessNo]->GetNewPacketIndicator()) 
		{
			m_iLoopHARQProcessNo++;
			m_iLoopHARQProcessNo %= m_iMaxProcessNum;
		}
	}

	double ASHARQEntity::CalcPriorityOfHARQProcess( const shared_ptr<HARQ> & pHARQ )
	{
		//TBC
		int sysclk = ClockManager::Instance().GetSystemClock().GetClock();	//���ϵͳʱ��
		return ( pHARQ->GetNewPacketIndicator() ? 0 : (sysclk - pHARQ->GetInitTransTime()) ) * 100/*����100��Ϊ���ش���Ȩֵ��һ��*/
					+ ( ( pHARQ->GetProcessNo() == m_iLoopHARQProcessNo ) ? 1 : 0 );
		return ( pHARQ->GetTranTime() ? (sysclk - pHARQ->GetInitTransTime()) : 0) * 100/*����100��Ϊ���ش���Ȩֵ��һ��*/
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
		//�첽HARQ���½�����ԭ��1���ش�����������ȼ��� 2�������ش�ʱ�ӵ����ȼ���
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
		if (maxPriority < 0)//˵����ʱ���е�HARQ���̶������ã����ڴ�����δ����״̬��
		{
			throw std::logic_error("HARQ���������ͷ���ʱ�Ӳ�ƥ�䣬Ӧ������HARQ������");
		}
		return processno;
	}
	shared_ptr <HARQ> LTESim::ASHARQEntity::GetHARQ()
	{
		return m_vHARQList[m_iCurrentProcessNo];
	}
}
