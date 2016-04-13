#pragma once

#include "commondef.h"
#include "CMDEngine.h"
#include "WiFiUE.h"
#include "UE.h"
#include "Clock.h"
#include "SystemElement.h"
#include "ClockProcessor.h"

#include <boost/enable_shared_from_this.hpp>

namespace LTESim
{
	class ServiceComposite;

	class ILTEWiFiUE : 
		public ILTEUE,
		public IWiFiUE
	{
	public:
		typedef boost::shared_ptr<ILTEWiFiUE> PTR;
	public:
		ILTEWiFiUE(){};
		virtual ~ILTEWiFiUE(){};
	};

	class LTEWiFiUE : 
		public ClockProcessor,
		public CMDEngine,
		public ILTEWiFiUE,
		public boost::enable_shared_from_this<LTEWiFiUE>,
		private SystemElement
	{
	public:
		typedef ::boost::shared_ptr<LTEWiFiUE> PTR;							//�û�ָ��
		typedef boost::weak_ptr<NodeB> LTENODEBPTR;
		typedef boost::weak_ptr<WiFiAP> WiFiAPPTR;
		typedef std::list<shared_ptr<Service> > SERVICELIST;									//�����б�
		typedef std::map<int, shared_ptr<NodeB> > NBLIST;										//��վ�б�
		typedef std::set<shared_ptr<PhyResource> > PhyResourceList;						//��Դ�б�һ��UE����������Դ��	
		typedef std::vector<double> SINRLIST;															//������б�
		typedef std::vector<MCS> CQILIST;																//MCS��
	public:
		LTEWiFiUE( int iUEid, double dPosX, double dPosY, shared_ptr<Antenna> pAntenna);
		virtual ~LTEWiFiUE();

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
		double GetWiFiNoiseFigure() const;

		shared_ptr<Antenna> GetRxAntenna() const;
		void SetAntenna( shared_ptr<Antenna> Aptr);

		//����һ��Service
		bool AddService(shared_ptr<Service> pService);
		//����ҵ��
		shared_ptr<Service> GetService( SystemType systemType );

		shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType );
		shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType );
		void UpdateDLHARQProcess( SystemType systemType );
		shared_ptr<ARQ> GetARQ() const;

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

		bool GetBoolNear() const;
		void SetBoolNear(bool b);

#pragma endregion

#pragma region IWiFiUE Interface

	public:
		virtual shared_ptr<AMCEntity> GetWiFiAMCEntity() const;
		virtual void AddToActiveAP(const shared_ptr<WiFiAP> pAP);
		virtual shared_ptr<WiFiAP> GetServingAP() const;
		virtual const int GetBackoffTime() const;//��ȡ�˱�ʱ��
		virtual void BackoffTimeTick();//�˱�ʱ���ʱ
		virtual void UpdateBackoffTime();//������ײ�󣬸����˱�ʱ��
		virtual void UpdateCollisionNum();

		void ReceiveChannel( const shared_ptr<WiFiChannel> pChannel );
		void MakeDecision( const shared_ptr<WiFiChannel> pChannel );
		void UpdateThroughput( const shared_ptr<WiFiChannel> pChannel );

#pragma endregion

#pragma region ʱ����Ӧ������

	public:
		/*��Ӧʱ�Ӷ�ʱ�Ľӿں���*/
		virtual void OnClock(const Clock& clock);
	protected:
		//����ÿ��֡��ʼʱ��Ҫ����Ĳ���
		virtual void SubFrameProcess(const Clock& subframeclock);
		//����ÿ֡��ʼʱ��Ҫ����Ĳ���
		virtual void FrameProcess(const Clock& frameclock);
		//������
		virtual void TxProcess(const Clock& clock);
		//�������
		virtual void RxProcess(const Clock& clock);
		void SlotProcess(const Clock& slotclock);
		void WiFiClockProcess(const Clock& wificlock);

#pragma endregion


#pragma region ��Ա����

		LTENODEBPTR						m_pServingLTENodeB;	//��ǰ�����LTEϵͳNodeB
		WiFiAPPTR						m_pServingAP;		//����AP
		shared_ptr<ServiceComposite>	m_pService;			//��ǰҵ��
		shared_ptr<Antenna>				m_pRxAntenna;		//����
		//To Be Modified, ˫ģ��UE��AMC�Ƿ��ã�
		shared_ptr<AMCEntity>			m_pLTEAMCEntity;	//AMCʵ��
		shared_ptr<AMCEntity>			m_pWiFiAMCEntity;	//AMCʵ��

		shared_ptr<HARQEntity>			m_pLTEDLHARQEntity;	//LTEϵͳ�е�HARQʵ��
		shared_ptr<ARQ>					m_pARQ;				//WiFi��ARQ
		double							m_dLTENoiseFigure;		//LTEUE����ϵ��,ʵֵ
		double							m_dWiFiNoiseFigure;		//WiFiUE����ϵ��,ʵֵ
		shared_ptr<Reporter>			m_CSIReporter;		//�ŵ������ı��������

		int								m_iBackoffTime;		//�˱�ʱ�䣬��λus
		int								m_iCollisionNum;	//����ײ����

		double							m_dDin;	//���ھ���
		bool							m_bNear;	//�Ƿ����վ�ȽϽ���PacketDynamic������ѡΪLTE�û�

#pragma endregion
	};

}

