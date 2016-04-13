#pragma once
#include "commondef.h"
#include "AMCEntity.h"
#include "Service.h"
#include "mcs.h"

#include <map>
#include <vector>

namespace LTESim
{
	class HARQ
	{
	public:
		typedef std::map <int, vector<double> > SINRLIST;					//�ش����ݵ�SINRֵ��FristΪ��������secondΪSINRֵ
		HARQ(shared_ptr<Service> pService, int processno/*���̱��*/, int ueid);
		HARQ(){};
		virtual ~HARQ(void);
	public:
		const MCS&						GetMCS();
		void							SetMCS(MCS mcs);

		void							RecordSINROfRetranPacket(const vector<double>& vSINRList);	//��¼�ش����ݵ�SINR�б�
		void							SINRCombining(vector<double>& RealSINRList   ) ;

		void							InitTrans();									//���δ����ʱ����ã���¼���δ���ʱ��
		void							UpdateTranTime();						//���´������
		bool							MaxTransTime() const;					//
		int								GetTranTime()const;

		void							SetService(shared_ptr<Service> pService);
		shared_ptr<Service>				GetService();

		bool							GetNewPacketIndicator(void) const;
		void							UpdateTransStatus(bool bReady);	//���½��̴���״̬�����Ⱥ�δ����ʱΪfalse������ʹ����Ϊ����ʱΪtrue
		bool							BeReady() const;							//�жϸý����Ƿ��Ѿ������ڴ����û�з���
		int								GetProcessNo() const;
		int								GetInitTransTime() const;				//�õ�����Я����packet�ĳ�ʼ����ʱ�̣�ΪHARQEntity����HARQ����ѡ��ʹ��
		
		void							SetUsedRBNum(size_t rbnum);		//���ñ���������Ҫ��RB��Ŀ
		size_t						GetUsedRBNum()const;				//�õ������RB��Ŀ
		void							Reset();

		void							SetRetransC2I( double dC2IValue );
		double						GetRetransC2I() const;
/*
		bool							IsNewData();
		void							CombineCQIVector( std::vector<double>& o_CQIVec );
		void							RecordCQIVectorOfRetranPacket( std::vector<double>& i_CQIVec );
		void							RecordMCSOfRetranPacket( MCS& refMCS );
		MCS							GetMCSOfRetransPacket();

		//�����ش��Ĵ���
		void							IncreaseRetransTime();
		int								GetRetransTime();
		bool							IsMaxRetransTime();

		//������ȷ��֮�󣬽���״̬������
		void							Reset();
*/


		//void			    SetProcessNo(int processno);
		//void				SetTranTime(int trantime);
		//void				SetUEID(int ueid);
		//void				SetNewPacketIndicator(bool packindicator);
		//vector	<double>    GetSINROfRetranPacket(int processnum);
		//void				SINRListErase(int processnum);
		//void				SetReady();//��־��HARQ�ķ����Ѿ�����������ڵ���

	protected:
		int							m_iMaxTransTime;										//��������
		int							m_iProcessNo;												//HARQ������
		MCS							m_MCS;														//MCS�ȼ���				
		int							m_iTranTime;												//������� ע�⣺2��ʾ�ش���һ��
		int							m_UEID;														//�û�ID		
		weak_ptr<Service>	m_pService;												//��HARQ���̶�Ӧ��ҵ��
		
		//�ش��������
		vector<double>		m_RetranSINRList;									    //�ش����ݵ�SINRֵ��FristΪ��������secondΪSINRֵ
		bool							m_bReady;													//�Ƿ�ɵ��ȣ������Ѿ���������δ�����Ľ���Ϊfalse����ʼΪtrue
		int							m_InitialTransClock;										//��ʼ�����ʱ��(SYSCLOCK)
        size_t						m_RBNUM;													//��¼��ʹ�õ�RB��Ŀ

		double						m_dRetransC2I;											//��¼ǰ�δ�����󣬼�¼����ʷC2I�������´��ش���ʱ�����CQI�ϲ�
	};
}
