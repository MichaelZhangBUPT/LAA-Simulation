#include "AuxiliaryService.h"
#include "ServiceStatistic.h"
#include "ClockManager.h"
#include "LTEWiFiUE.h"

namespace LTESim
{
	AuxiliaryService::AuxiliaryService( shared_ptr<IUE> pUE, shared_ptr<Service> pCompService ) : Service( pUE ), m_pMasterCompService( pCompService )
	{
		m_IsEnd = true;
		m_dThrInAPacket = 0.0;
		m_ServiceType = Auxiliary;
	}

	AuxiliaryService::~AuxiliaryService()
	{

	}

	void AuxiliaryService::UpdateThroughput( DATASIZE throughput ) //����������
	{
		shared_ptr<Service> pMasterCompService = m_pMasterCompService.lock();
		//����Auxiliary Service��ֻ��Ҫ������������Buffer�е������������Buffer�е�����Ϊ�գ��򽫸�Service��ΪEnd״̬
		if ( m_dBufferData <= throughput )
		{
			//int ID = dynamic_pointer_cast<LTEWiFiUE>(pMasterCompService->GetUE()) -> GetID();
			SetIsEnd( true );
			m_dPacketEndTime = ClockManager::Instance().GetCurrentTime();
			m_vecPacketTransTime.push_back( m_dPacketEndTime - m_dPacketStartTime );
			m_SumThroughput += m_dBufferData;
			pMasterCompService->UpdateThroughput( m_dBufferData );
			m_dBufferData = 0.0;
			m_dThrInAPacket = 0.0;
		}
		else
		{
			m_dBufferData -= throughput;
			m_SumThroughput += throughput;
			m_dThrInAPacket += throughput;
			pMasterCompService->UpdateThroughput( throughput );
		}
		m_SumIdealThroughput += throughput;
		++m_iPacketNum;
	}

	void AuxiliaryService::UpdateBuffer(unsigned int timespan/*������ʱ����ms*/)
	{

	}

	void AuxiliaryService::UpdateDiscardThroughput( DATASIZE throughput )		//���¶������ݰ���������
	{
		m_SumDiscardThroughput += throughput;
		++m_iPacketNum;
		++m_iDiscardPacketNum;
	}

	void AuxiliaryService::Accept( ServiceStatistic& visit )										//����ͳ�Ʒ��ʽӿ�
	{
		visit.Visit(*this);
	}

	void AuxiliaryService::UpdateMCSIndex( int mcsindex )									//����MCS�ȼ�
	{
		++m_MCSIndex[mcsindex];
	}

	void AuxiliaryService::UpdateGenerateInterval( void )
	{
		
	}

	double AuxiliaryService::GetThrInAPacket( void )
	{
		return m_dThrInAPacket;
	}

}