#pragma once

#include "commondef.h"
#include "CMDEngine.h"
#include "HSPAUE.h"
#include "UE.h"
#include "Clock.h"
#include "SystemElement.h"
#include "ClockProcessor.h"
#include "HSPANodeB.h"

#include <boost/enable_shared_from_this.hpp>

namespace LTESim
{
	class ServiceComposite;

	class IDualModeLTEHSPAUE : 
		public ILTEUE,
		public IHSPAUE
	{
	public:
		typedef boost::shared_ptr<IDualModeLTEHSPAUE> PTR;
	public:
		IDualModeLTEHSPAUE(){};
		virtual ~IDualModeLTEHSPAUE(){};
	};

	class DualModeLTEHSPAUE : 
		public ClockProcessor,
		public CMDEngine,
		public IDualModeLTEHSPAUE,
		public boost::enable_shared_from_this<DualModeLTEHSPAUE>,
		private SystemElement
	{
	public:
		typedef ::boost::shared_ptr<DualModeLTEHSPAUE> PTR;							//�û�ָ��
		typedef boost::weak_ptr<NodeB> LTENODEBPTR;
		typedef boost::weak_ptr<HSPANodeB> HSPANODEBPTR;
		typedef std::list<shared_ptr<Service> > SERVICELIST;									//�����б�
		typedef std::map<int, shared_ptr<NodeB> > NBLIST;										//��վ�б�
		typedef std::set<shared_ptr<PhyResource> > PhyResourceList;						//��Դ�б�һ��UE����������Դ��	
		typedef std::vector<double> SINRLIST;															//������б�
		typedef std::vector<MCS> CQILIST;																//MCS��
	public:
		DualModeLTEHSPAUE( int iUEid, double dPosX, double dPosY, shared_ptr<Antenna> pAntenna);
		virtual ~DualModeLTEHSPAUE();

#pragma region IUE Interface

	public:
		virtual const POS& GetPos() const;
		virtual int GetID() const;
		virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys );

		virtual SystemType GetType() const;

		//UE��ʼ��
		void Initialization();
		//��ȡ����ϵ��
		double GetNoiseFigure() const;
		/*����UE�����ߣ���LTEϵͳ��ʹ��*/
		shared_ptr<Antenna> GetRxAntenna() const;
		void SetAntenna( shared_ptr<Antenna> Aptr);

		//����һ��Service
		bool AddService(shared_ptr<Service> pService);
		//����ҵ��
		shared_ptr<Service> GetService( SystemType systemType );

		shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType );
		shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType );
		void UpdateDLHARQProcess( SystemType systemType ); 

#pragma endregion

#pragma region ILTE Interface

	public:
		//NodeB��ѡ��
		const NBLIST& GetCandidateNodeBSet() const;
		/*�õ���ǰ�����NodeB*/
		shared_ptr<NodeB> GetServingNodeB() const;
		/*��NodeB���뼤�*/
		void AddToActiveSet(const shared_ptr<NodeB> pNodeB);

		//����һ���ŵ���ֻ����ǰʱ϶�Ľ��գ�����ںϲ��о�
		void ReceiveChannel( const shared_ptr<Channel> pRxChannel );
		void MakeDecision( const shared_ptr<Channel> pRxChannel, int iframeno, int isubframeno);

		//��������RB��SINR�Լ�CQI�����ڴ���ȥ�Ĳ������� 
		void MeasureSINRAndCQIofAllTheRB(SINRLIST &sinrlist, CQILIST &cqilist);

		void UpdateAverageThroughput( const shared_ptr<Channel> pChannel );
		//�õ���ʱ϶���û���RB�б�
		void	GetRBListofSlot(const PhyResourceList &SourceRBlist, PhyResourceList& DesRBList, int frame, int slot);
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
		//����din,���ھ���
		double GetDin() const;
		void SetDin(double din);

#pragma endregion

#pragma region IHSPAUE Interface

	public:
		virtual void AddToActiveSet( const shared_ptr<HSPANodeB> pNodeB );
		virtual void ReceiveChannel( const shared_ptr<HSPAChannel> pChannel );

		virtual shared_ptr<HSPANodeB> GetServingHSPANodeB() const;
		virtual shared_ptr<IHSPAReporter> GetHSPAReporter() const;

		virtual void UpdateAverageThroughput( const shared_ptr<HSPAChannel> pChannel );

	protected:
		void MakeDecision( shared_ptr<HSPAChannel> pChannel );
		void UpdateThroughput(const shared_ptr<HSPAChannel> pChannel );
		void UpdateReport( double dC2IValue, const MCS& recommendMCS, const Matrix& recommendDLWeightMatrix );

#pragma endregion

#pragma region ʱ����Ӧ������

	public:
		/*��Ӧʱ�Ӷ�ʱ�Ľӿں���*/
		virtual void OnClock(const Clock& clock);
	protected:
		//����ÿʱ϶��ʼʱ��Ҫ����Ĳ����������������TxProcess����RxProcess
		virtual void SlotProcess(const Clock& slotclock);
		//����ÿ��֡��ʼʱ��Ҫ����Ĳ���
		virtual void SubFrameProcess(const Clock& subframeclock);
		//����ÿ֡��ʼʱ��Ҫ����Ĳ���
		virtual void FrameProcess(const Clock& frameclock);
		//������
		virtual void TxProcess(const Clock& clock);
		//�������
		virtual void RxProcess(const Clock& clock);

#pragma endregion


#pragma region ��Ա����

		LTENODEBPTR									m_pServingLTENodeB;					//��ǰ�����LTEϵͳNodeB
		HSPANODEBPTR								m_pServingHSPANodeB;				//��ǰ�����HSPAϵͳNodeB
		shared_ptr<ServiceComposite>		m_pService;									//��ǰҵ��
		shared_ptr<Antenna>						m_pRxAntenna;							//����

		//To Be Modified, ˫ģ��UE��AMC�Ƿ��ã�
		shared_ptr<AMCEntity>					m_pLTEAMCEntity;						//AMCʵ��
		shared_ptr<AMCEntity>					m_pHSPAAMCEntity;

		shared_ptr<HARQEntity>					m_pLTEDLHARQEntity;					//LTEϵͳ�е�HARQʵ��
		shared_ptr<HARQEntity>					m_pHSPADLHARQEntity;				//HSPAϵͳ�е�HARQʵ��

		double												m_dNoiseFigure;							//UE����ϵ��

		shared_ptr<Reporter>						m_CSIReporter;								//�ŵ������ı��������
		shared_ptr<IHSPAReporter>				m_pHSPAReporter;
		double									m_dDin;	//���ھ���


#pragma endregion
	};

}

