#pragma once
#include"UE.h"
#include "NodeB.h"
#include "PhyResource.h"
#include "Service.h"
#include <set>
#include <vector>

namespace LTESim
{
	class Channel														//�����ŵ�
	{
	public:
		//�����ŵ���Դ��RB��Դ(�������ߣ�֡�� ���߶˿� ʱ϶�� RB��, ��ռ�ķ��书��)
		typedef std::set< shared_ptr<PhyResource> > PhyResourceList;	
        
        ///���캯��
		Channel(void);
	public:
        ///��������
		virtual ~Channel(void);
		
		///�ŵ���ʼ��
        /**
        * ��ʼ���ŵ�
        * @param nodeb, �ŵ���nodeb��
        * @param ue���ŵ���ue��
        * @param pService, �ŵ�Я����ҵ��
        * @param pHARQ, �ŵ�Я����HARQ����
        * @param phyResourceList, ��ʹ�õ�������Դ��һά�б�
        * ��СΪ���߶˿���*ʹ�õ�RB��*ʹ�õ�ʱ϶��
        * �������߶˿ڡ�RB��ʱ϶�Ĵ������ӡ�
        * @param mcstype��ʹ�õ�MCS
        */
		void Initilization(shared_ptr<NodeB> nodeb, shared_ptr<ILTEUE> ue, shared_ptr<Service> pService, 
			shared_ptr<HARQ> pHARQ, const PhyResourceList& phyResourceList,const MCS& mcstype);
		
        ///ȡ��MCS�б�
		const MCS& GetMCS() const;
		
		//�õ�ռ�õ�RB����
        /**
        * ������ʹ�õ����߶�û�и��ã������Ҫ�����߸��õĻ������ڲ���Ҫ�޸�
        * @return ʱ϶��Ŀ*RB��Ŀ*���߸��ý���
        */
		int GetRBNum() const;
		
        //�õ�ĳ��ʱ϶�ϵ�RB����
        /**
        * ������ʹ�õ����߶�û�и��ã������Ҫ�����߸��õĻ������ڲ���Ҫ�޸�
        * �ú����ٶ�ÿ��ʱ϶�����RB��һ�£������һ����Ҫ�޸ĺ���ʵ��
        * @param slot, 0~19��ʱ϶��
        * @return ��ǰʱ϶RB��Ŀ*���߸��ý���
        */
        int GetRBNum(int slot) const;

		///�õ��ŵ������ڵ�UE
		shared_ptr<ILTEUE>		GetActiveUE() const;                             

        ///�õ��ŵ������ڵ�NodeB
		shared_ptr<NodeB>			GetActiveNodeB() const; 

        ///�õ�Я����Service
		shared_ptr<Service>			GetActiveService() ;

        ///�õ�Я����HARQ
        shared_ptr<HARQ>			GetHARQ();							

		///ȡ���ŵ������һ��ʱ϶����Ϊchannel�����֡
        /**
        * @return 0~SLOTPERFRAME-1��slot index
        */
		int GetLastSlot(void) const;						        

        ///ȡ�õ�һ��ʱ϶����Ϊchannel�����֡
        /**
        * @return 0~SLOTPERFRAME-1��slot index
        */
		int GetFirstSlot(void) const;	

        ///channel��ռ�õ�ʱ϶��
		std::size_t GetSlotNum(void) const;	

        ///ȡ��ʹ�õ�������Դ
        /**
        * @return PhyResourceList������ʹ�õ�������Դ��һά�б�
        * ��СΪ���߶˿���*ʹ�õ�RB��*ʹ�õ�ʱ϶��
        * �������߶˿ڡ�RB��ʱ϶�Ĵ������ӡ�
        */
		const PhyResourceList &GetPhyResourceList()const;

        ///ȡ��ռ�õ�ʱ϶
        /**
        * @return set<int>, һ�����и���ռ��ʱ϶index�ı�
        */
		const std::set<int>&			GetSlotList() const;
  
        ///�ж�ʱ϶�Ƿ�ʹ��
        /**
        * @param slot, 0~SLOTPERFRAME-1��index
        * @return true����ʹ��
        */
		bool							UsedSlot(int slot) const;			

        ///�����ŵ���SINR
        /*
        * �ڶ��ʱ϶������£���Ҫ�ŵ�������ʱ϶��SINR����һ�б�����������о�
        * @param SINRList, ��ǰʱ϶�ľ������ߺϲ����SINR�б�������EESM
        * @param FrameNo, ��ǰ֡�ţ� 0��ʼ����ֹ֮���һ��frame �ţ�����δ�ã���ǰ�����ܿ�֡)
        * @param SlotNo, 0~SLOTPERFRAME-1����ǰʱ϶��
        */
		void							UpdateSINRListofSlot(vector<double>& SINRlist, int FrameNo, int SlotNo);			//���¸��ŵ�ÿ��RBÿ��ʱ϶��SINRֵ

        ///�õ�SINR
        /**
        * @return ����ʱ϶��SINR����һ�еı�
        */
		vector<double>&		GetSINRList();


	
	protected:
		MCS										m_MCSType;								//���ŵ�MCS�ȼ�
		weak_ptr<ILTEUE>				m_pActiveUE;								//���ŵ����ڵ��û�
		weak_ptr<NodeB>				m_pAcitiveNodeB;						//���ŵ�������վ 

		//�����ŵ���Դ��RB��Դ(�������ߣ�RB�� ʱ϶�� ֡�� ���߶˿�)
		PhyResourceList					m_PhyResourceList;
		//�����ŵ���Դ��first:���书�ʣ�second:RB��Դ(�������ߣ�RB�� ʱ϶�� ��֡�� ֡�� ���߶˿�)
		std::set<int>							m_vSlotList;									//���ŵ�ռ��ʱ϶
		weak_ptr<Service>				m_Service;									//���ŵ���Ӧ��ҵ��
		vector <double>					m_vSINRListOfRBAndSlot;			//ÿ��RB�ڸ��ŵ�ʱ϶��SINRֵ
		weak_ptr<HARQ>				m_pHARQ;									//���ŵ�����HARQ��Ϣ
	};
};

