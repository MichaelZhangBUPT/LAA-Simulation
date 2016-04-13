#pragma once

#include "commondef.h"
#include "Matrix.h"
#include "CMDEngine.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
#include "IUE.h"
#include "HARQ.h"
#include "HSPAUE.h"
#include "HSPAChannel.h"
#include "HARQEntity.h"
#include "HSPAReporter.h"
#include <boost/enable_shared_from_this.hpp>

namespace LTESim
{
	class IHSPAReporter;

	class IHSPAUE
		: virtual public IUE
	{
	public:
		typedef boost::shared_ptr<IHSPAUE> PTR;
	public:
		IHSPAUE(){};
		virtual ~IHSPAUE(){};
	public:
		virtual void AddToActiveSet( const shared_ptr<HSPANodeB> pNodeB ) = 0;
		virtual void ReceiveChannel( const shared_ptr<HSPAChannel> pChannel ) = 0;
		virtual shared_ptr<HSPANodeB> GetServingHSPANodeB() const = 0;
		virtual shared_ptr<IHSPAReporter> GetHSPAReporter() const  = 0;
		virtual void UpdateAverageThroughput( const shared_ptr<HSPAChannel> pChannel ) = 0;
		virtual void SetAntenna( shared_ptr<Antenna> Aptr) = 0;
	};


	class HSPAUE :	
		public CMDEngine, 
		public ClockProcessor, 
		public IHSPAUE,
		public boost::enable_shared_from_this<HSPAUE>,
		private SystemElement
	{
	public:
		typedef boost::shared_ptr<HSPAUE> PTR;	//�û�ָ��
		typedef std::pair<int,double> IDAndC2I;
	public:
		HSPAUE(int id, const POS& pos);
		virtual ~HSPAUE();

	public:
		//Interface SystemElement
		virtual const POS& GetPos() const;
		virtual int GetID() const;
		virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys );

		virtual SystemType GetType() const;

		//CMDEngine Interface
		virtual void OnClock(const Clock& clock);

		//ClockProcessor Interface
		virtual void FrameProcess( const Clock& FrameClock);
		virtual void SubFrameProcess( const Clock& SubFrameClock );
		virtual void SlotProcess( const Clock& SlotClock );
		virtual void TxProcess( const Clock& txclock );
		virtual void RxProcess( const Clock& rxclock );

		virtual void AddToActiveSet( const shared_ptr<HSPANodeB> pNodeB );
		virtual void ReceiveChannel( const shared_ptr<HSPAChannel> pChannel );

		virtual void Initialization();

		//��ȡ����ϵ��
		virtual double GetNoiseFigure() const;

		/*����UE������*/
		virtual void SetAntenna( shared_ptr<Antenna> Aptr );
		virtual shared_ptr<Antenna> GetRxAntenna() const;

		//ҵ�����
		//����һ��Service
		virtual bool AddService(shared_ptr<Service> pService);
		//����ҵ��
		virtual shared_ptr<Service> GetService( SystemType systemType );

		//HARQ
		virtual shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType );
		virtual shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType );
		virtual void UpdateDLHARQProcess( SystemType systemType );

		virtual shared_ptr<HSPANodeB> GetServingHSPANodeB() const;
		virtual shared_ptr<IHSPAReporter> GetHSPAReporter() const;

		virtual void UpdateAverageThroughput( const shared_ptr<HSPAChannel> pChannel );

		virtual int GetMaxTransTimeNum() const;

	protected:
		void MakeDecision( const shared_ptr<HSPAChannel> pChannel );
		void UpdateThroughput( const shared_ptr<HSPAChannel> pChannel );
		void UpdateReport( double dC2IValue, const MCS& recommendMCS, const Matrix& recommendDLWeightMatrix );
		//����MCS�ȼ�
		void UpdateMCSIndex(const shared_ptr<HSPAChannel> pChannel);
		 //����UE���ŵ�����������NodeB�˵���ȡ�ã�������ʾ�ŵ���Ϣ�Ƿ����ʱ�ӣ�delayslotnum=0��ʾ�ñ�TTI�ŵ���Ϣ���Դ�����
		void UpdateHSPACSI(int delayslotnum);

	protected:
		weak_ptr<HSPANodeB>			m_pServingNodeB;
		shared_ptr<Service>					m_pService;
		double										m_dNoiseFigure;
		int								m_iMaxTransTimeNum;

		shared_ptr<HARQEntity>			m_pDLHARQEntity;
		shared_ptr<IHSPAReporter>		m_pReporter;
		shared_ptr<AMCEntity>			m_pAMCEntity;
		shared_ptr<Antenna>				m_pAntenna;
	};

}