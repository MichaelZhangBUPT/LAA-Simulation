#include "commondef.h"
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <iterator>
#include "ARQ.h"
#include "ClockManager.h"


namespace LTESim
{
	ARQ::ARQ(shared_ptr<Service> pService)
		: m_pService(pService)
	{
		m_iTranTime = 0;
		m_iMaxTransTime = 4;
		Reset();
	}

	ARQ::~ARQ(void)
	{
	}

	void ARQ::UpdateTranTime()	
	{
		m_iTranTime++;
	}

	bool ARQ::MaxTransTime() const	
	{
		//�Ƿ�Ϊ��������
		//m_iTranTime��ʾ�Ѿ�����Ĵ���
		//ֻ�д�ʱ���ڴ����ʱ��Ż����MaxTransTime���������+1����һ������Ĵ���
		// ( һ������Ĵ��� == m_iTranTime+1 == m_iMaxTransTime )�򷵻�true
		return m_iTranTime == m_iMaxTransTime - 1;
	}

	int ARQ::GetTranTime() const	//��ô������
	{
		return m_iTranTime;
	}

	void ARQ::Reset()
	{
		m_iTranTime = 0;							//������� ע�⣺2��ʾ�ش���һ��
		UpdateTransStatus(true);					//׼������
	}

	void ARQ::SetService(shared_ptr<Service> pService)
	{
		m_pService = pService;
	}

	shared_ptr<Service> ARQ::GetService()
	{
		return m_pService.lock();
	}


	bool ARQ::BeReady() const
	{
		return m_bReady;
	}

	//���½��̴���״̬�����Ⱥ�δ����ʱΪfalse������ʹ����Ϊ����ʱΪtrue
	void ARQ::UpdateTransStatus( bool bReady )
	{
		m_bReady = bReady;
	}

	void ARQ::SetMCS( LTESim::MCS mcs )
	{
		m_MCS = mcs;
	}

	const MCS& ARQ::GetMCS()
	{
		return m_MCS;
	}

	//������ָʾ
	bool ARQ::GetNewPacketIndicator( void ) const	 
	{
		return m_iTranTime == 0;
	}

}

