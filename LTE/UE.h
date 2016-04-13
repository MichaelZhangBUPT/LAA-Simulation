#pragma once
#include <set>
#include <list>
#include <boost/enable_shared_from_this.hpp>
#include "commondef.h"
#include "Clock.h"
#include "CMDEngine.h"
#include "POS.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
#include "Antenna.h"
#include "HARQ.h"
#include "PhyResource.h"
#include "Channel.h"
#include "IUE.h"

namespace LTESim
{
	class System;
	class NodeB;
	class Channel;
	class Service;
	class ReporterImp;
	class Reporter;
	class HARQEntity;
	typedef std::list<shared_ptr<Service> > SERVICELIST;

	class ILTEUE : 
		virtual public IUE
	{
	public:
		typedef weak_ptr<NodeB> NODEBPTR;
		typedef ::boost::shared_ptr<ILTEUE> PTR;							//�û�ָ��
		typedef std::map<int, shared_ptr<NodeB> > NBLIST;				//��վ�б�
		typedef std::vector<double> SINRLIST;									//������б�
		typedef std::vector<MCS> CQILIST;										//MCS��
	public:
		ILTEUE(){};
		virtual ~ILTEUE(){};
	public:
		//NodeB��ѡ��
		virtual const NBLIST& GetCandidateNodeBSet() const = 0;
		/*�õ���ǰ�����NodeB*/
		virtual shared_ptr<NodeB> GetServingNodeB() const = 0;
		/*��NodeB���뼤�*/
		virtual void AddToActiveSet(const shared_ptr<NodeB> pNodeB) = 0;

		//Interface Reporter
		//���SINR��CQI�ı���
		virtual shared_ptr<Reporter> GetReporter() const = 0;
		//����SINR��CQI�ı���
		virtual void UpdateReport( const SINRLIST& sinrlist, const CQILIST& cqilist) = 0;

		virtual void ReceiveChannel( const shared_ptr<Channel> pRxChannel ) = 0;

		virtual void UpdateAverageThroughput( const shared_ptr<Channel> pChannel ) = 0;
		//���din�����ھ���
		virtual double GetDin() const = 0;
		virtual void SetDin( double din ) = 0;
	};

	class UE : 
		public CMDEngine, 
		public ClockProcessor,
		public ILTEUE,
		public boost::enable_shared_from_this<UE>,
		private SystemElement
	{

	public:
		typedef std::list<shared_ptr<Service> > SERVICELIST;				//�����б�
		typedef weak_ptr<NodeB> NODEBPTR;
		typedef boost::shared_ptr<UE> PTR;										//�û�ָ��
		typedef std::map<int, shared_ptr<NodeB> > NBLIST;					//��վ�б�
		typedef std::set<shared_ptr<PhyResource> > PhyResourceList;	//��Դ�б�һ��UE����������Դ��	
		typedef std::vector<double> SINRLIST;										//������б�
		typedef std::vector<MCS> CQILIST;											//MCS��

#pragma region ��������
	public:
		UE(int ueid/*UE��ʾ*/, const POS& pos/*UEλ��*/, shared_ptr<Antenna> pRxAntenna);
		virtual ~UE(void);
#pragma endregion 

		//Interface SystemElement
		virtual const POS& GetPos() const;
		virtual int GetID() const;
		virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys );

		virtual SystemType GetType() const;

		//UE��ʼ��
		void Initialization();
		//��ȡ����ϵ��
		double GetNoiseFigure() const;
		/*����UE������*/
		shared_ptr<Antenna> GetRxAntenna() const;

		//����һ��Service
		bool AddService(shared_ptr<Service> pService);
		//����ҵ��
		shared_ptr<Service> GetService( SystemType systemType );
		//����din,���ھ���
		double GetDin() const;
		void SetDin(double din);

#pragma region Relative eNodeB
		//NodeB��ѡ��
		const NBLIST& GetCandidateNodeBSet() const;
		/*�õ���ǰ�����NodeB*/
		shared_ptr<NodeB> GetServingNodeB() const;
		/*��NodeB���뼤�*/
		void AddToActiveSet(const shared_ptr<NodeB> pNodeB);
#pragma endregion

#pragma region ����
		//����һ���ŵ���ֻ����ǰʱ϶�Ľ��գ�����ںϲ��о�
		void ReceiveChannel( const shared_ptr<Channel> pRxChannel );
		void MakeDecision( const shared_ptr<Channel> pRxChannel, int iframeno, int isubframeno);
		void UpdateAverageThroughput( const shared_ptr<Channel> pChannel );
#pragma endregion 

#pragma region ����SINR
	public:
		void MeasureSINRAndCQIofAllTheRB(SINRLIST &sinrlist, CQILIST &cqilist);//��������RB��SINR�Լ�CQI�����ڴ���ȥ�Ĳ�������
#pragma endregion 

#pragma region ����
	public:
		//�õ���ʱ϶���û���RB�б�
		void GetRBListofSlot(const PhyResourceList &SourceRBlist, PhyResourceList& DesRBList, int frame, int slot);
		//ͳ��������
		void UpdateThroughput(const shared_ptr<Channel> pchannel);
		//ͳ�ƶ������ݰ���������
		void UpdateDiscardThroughput(const shared_ptr<Channel> pchannel);
		//����UE���ŵ�����������NodeB�˵���ȡ�ã�������ʾ�ŵ���Ϣ�Ƿ����ʱ�ӣ�delayslotnum=0��ʾ�ñ�TTI�ŵ���Ϣ���Դ�����
		void UpdateCSI(int delayslotnum);
		//����MCS�ȼ�  YRM
		void UpdateMCSIndex(const shared_ptr<Channel> pchannel);

		//����SINR��CQI�ı���
		void UpdateReport( const SINRLIST & sinrlist, const CQILIST& cqilist);
		//���SINR��CQI�ı���
		shared_ptr<Reporter> GetReporter() const;
#pragma endregion 

#pragma region HARQ
	public:
		shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType );
		shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType );
		void UpdateDLHARQProcess( SystemType systemType );
#pragma endregion 

#pragma region ʱ����Ӧ������
	public:
		/*��Ӧʱ�Ӷ�ʱ�Ľӿں���*/
		void OnClock(const Clock& clock);
	protected:
		//����ÿʱ϶��ʼʱ��Ҫ����Ĳ����������������TxProcess����RxProcess
		void SlotProcess(const Clock& slotclock);
		//����ÿ��֡��ʼʱ��Ҫ����Ĳ���
		void SubFrameProcess(const Clock& subframeclock);
		//����ÿ֡��ʼʱ��Ҫ����Ĳ���
		void FrameProcess(const Clock& frameclock);
		//������
		void TxProcess(const Clock& clock);
		//�������
		void RxProcess(const Clock& clock);
#pragma endregion

#pragma region ��Ա����
	protected:
		NODEBPTR						m_pServingNodeB;				//��ǰ����С��
		shared_ptr<Service>				m_pService;							//��ǰҵ��
		shared_ptr<Antenna>			m_pRxAntenna;					//����

		shared_ptr<Reporter>			m_CSIReporter;				    //�ŵ������ı��������
		shared_ptr<AMCEntity>		m_AMCEntity;						//AMCʵ��
		shared_ptr<HARQEntity>		m_DLHARQEntity;				//HARQʵ��

		double									m_dNoiseFigure;						//UE����ϵ��
		double						m_dDin;	//���ھ���
#pragma endregion 
	};

}


