#include "commondef.h"
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <iterator>
#include "HARQ.h"
#include "ClockManager.h"


namespace LTESim
{
	HARQ::HARQ(shared_ptr<Service> pService, int processno, int ueid )
		: m_pService(pService), m_iProcessNo(processno), m_UEID(ueid), m_MCS( 0, SYSTEM_LTE )
	{
		m_iTranTime = 0;
		m_iMaxTransTime = 4;
		Reset();
	}

	HARQ::~HARQ(void)
	{
	}

	void HARQ::SetMCS( LTESim::MCS mcs )
	{
		m_MCS = mcs;
	}

	void HARQ::InitTrans()	//��ó�ʼ����ʱ��
	{
		//To Be Modified
		m_InitialTransClock = ClockManager::Instance().GetLTESlotClock().GetClock();
	}

	void HARQ::RecordSINROfRetranPacket( const vector<double>& vSINRList )	//��¼�ش����ݵ�SINR�б�
	{
		using namespace boost::lambda;
		m_RetranSINRList.clear();	//����ӵģ���Ϊ��ʱ�����е�SINR�Ѿ�����͹��ˣ�����else�е�����Ƕ����
		if (m_RetranSINRList.empty())
		{
			 m_RetranSINRList = vSINRList;
		}
		else
		{
			std::transform(vSINRList.begin(), vSINRList.end(),m_RetranSINRList.begin(), m_RetranSINRList.begin(), _1 + _2);
		}
	}

	//������ָʾ
	bool HARQ::GetNewPacketIndicator( void ) const	 
	{
		//�������
		return m_iTranTime == 0;
	}

	//���´������
	void HARQ::UpdateTranTime()	
	{
		m_iTranTime++;
	}

	bool HARQ::MaxTransTime() const	
	{
		//�Ƿ�Ϊ��������
		//m_iTranTime��ʾ�Ѿ�����Ĵ���
		//ֻ�д�ʱ���ڴ����ʱ��Ż����MaxTransTime���������+1����һ������Ĵ���
		// ( һ������Ĵ��� == m_iTranTime+1 == m_iMaxTransTime )�򷵻�true
		return m_iTranTime == m_iMaxTransTime - 1;
	}

	void HARQ::SINRCombining( vector<double>& RealSINRList ) 
	{
		using namespace boost::lambda;
		std::transform(m_RetranSINRList.begin(), m_RetranSINRList.end(), RealSINRList.begin(), RealSINRList.begin(), _1+_2);
	}

	void HARQ::Reset()
	{
		m_RetranSINRList.clear();				//���SINR�б�
		m_iTranTime = 0;								//������� ע�⣺2��ʾ�ش���һ��
		m_InitialTransClock = -100;				//�����ݰѴ���֡����Ϊ��Ч
		m_dRetransC2I = 0.0;
		UpdateTransStatus(true);					//׼������
	}

	void HARQ::SetRetransC2I(double dC2IValue )
	{
		m_dRetransC2I += dC2IValue;
	}

	double HARQ::GetRetransC2I() const
	{
		return m_dRetransC2I;
	}

	void HARQ::SetService(shared_ptr<Service> pService)
	{
		m_pService = pService;
	}

	shared_ptr<Service> HARQ::GetService()
	{
		return m_pService.lock();
	}

	int HARQ::GetTranTime() const	//��ô������
	{
		return m_iTranTime;
	}

	bool HARQ::BeReady() const
	{
		return m_bReady;
	}

	int HARQ::GetProcessNo() const
	{
		return m_iProcessNo;
	}

	//���½��̴���״̬�����Ⱥ�δ����ʱΪfalse������ʹ����Ϊ����ʱΪtrue
	void HARQ::UpdateTransStatus( bool bReady )
	{
		m_bReady = bReady;
	}

	//��ó�ʼ����ʱ��
	int HARQ::GetInitTransTime() const	
	{
		return m_InitialTransClock;
	}
    
	const MCS& HARQ::GetMCS()
	{
		return m_MCS;
	}

	void HARQ::SetUsedRBNum(size_t rbnum)
	{
		m_RBNUM=rbnum;
	}

	size_t  HARQ::GetUsedRBNum()const
	{
		return m_RBNUM;
	}
}

