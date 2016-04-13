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

		void							UpdateTransStatus(bool bReady);	//���½��̴���״̬�����Ⱥ�δ����ʱΪfalse������ʹ����Ϊ����ʱΪtrue
		bool							BeReady() const;							//�жϸý����Ƿ��Ѿ������ڴ����û�з���
	
		void							UpdateTranTime();						//���´������
		bool							MaxTransTime() const;					//
		int								GetTranTime()const;
		bool							GetNewPacketIndicator(void) const;
		
		const MCS&						GetMCS();
		void							SetMCS(MCS mcs);

		void							Reset();


	protected:
		weak_ptr<Service>	m_pService;												//��HARQ���̶�Ӧ��ҵ��
		
		//�ش��������
		bool							m_bReady;													//�Ƿ�ɵ��ȣ������Ѿ���������δ�����Ľ���Ϊfalse����ʼΪtrue
		int							m_iMaxTransTime;										//��������
		int							m_iTranTime;								//������� ע�⣺2��ʾ�ش���һ��
		MCS							m_MCS;											//MCS�ȼ���				

	};
}
