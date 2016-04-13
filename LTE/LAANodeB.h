#pragma once

#include "commondef.h"
#include <boost/enable_shared_from_this.hpp>
#include "LAAUE.h"
#include "LAAChannel.h"

#include "Clock.h"
#include "CMDEngine.h"
#include "POS.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
#include "Antenna.h"

namespace LTESim
{
	class LAANodeB: 
		public CMDEngine, 
		public ClockProcessor, 
		public SystemElement, 
		public boost::enable_shared_from_this<LAANodeB>
	{
	public:
		typedef boost::shared_ptr<LAANodeB> PTR;
		typedef weak_ptr<ILAAUE> LAAUEPTR;
		typedef std::map<int, LAAUEPTR> LAAUELIST;
	public:
		LAANodeB(int id, const POS& pos, double maxtxpower, shared_ptr<Antenna> pAntenna);
		virtual ~LAANodeB(void);

		void OnClock(const Clock& clock);
		void LAAClockProcess(const Clock& LAAclock);
		virtual void FrameProcess( const Clock& FrameClock){};
		virtual void SubFrameProcess( const Clock& SubFrameClock ){};
		virtual void SlotProcess( const Clock& SlotClock ){};
		virtual void TxProcess( const Clock& txclock ){};
		virtual void RxProcess( const Clock& rxclock ){};

		void AddUE( const shared_ptr<ILAAUE> pUE );
		shared_ptr<Antenna> GetTxAntenna() const;
		double GetMaxTxPower() const;
		const LAAUELIST& GetUEList() const;

		void ChannelIdleProcess();
		void ChannelBackoffProcess();//HFR
		void ChannelBusyProcess();
		void ChannelCollisionProcess();
		ChannelState GetChannelState();

		void GetCandidateUEList();
		void InitChannel();
		void TransmitChannel( const shared_ptr<LAAChannel> pChannel, int iTimeDelay );
		const int GetSumCollisionNum() const;

		virtual void UpdateCellThroughput( double thr );
		virtual double GetCellThroughput();

		virtual int GetTransmitDataTime();

		//LBT�˱ܴ���ϵ�к���HFR
		const int GetLBTBackoffTime() const;
		void LBTBackoffTimeTick();//�˱�ʱ���ʱ
		void UpdateLBTBackoffTime();//������ײ�󣬸����˱�ʱ��

		void LBTDeferTimeTick();//�ۼ�deferʱ�䳤��
		const int GetLBTDeferTime();//���ص�ǰLBTdefer��ʱ��

		void GetCCAResult();
		void GetFirstTransBackoffTime();


#pragma region ��Ա����
	protected:
		LAAUELIST						m_LAAUEList;//�����AP��WiFiUE
		LAAUELIST						m_CandidateUEList;//׼����Ҫ���շ����UE
		shared_ptr<Antenna>				m_pTxAntenna;//ҵ���ŵ���������	
		ChannelState                    m_ChannelState;//�ŵ�״̬
		double							m_dMaxTxPower;//����书�ʣ�mW
		double							m_dDataMaxLength;//һ�δ���������ݣ�bit���Ѳ�ʹ��
		int								m_TXOPLimit;//һ��ռ���ŵ������ʱ����us
		int								m_iChannelIdleTime;//�ŵ�����״̬�ĳ���ʱ�䣬us
		int								m_iChannelBusyTime;//�ŵ�æµ״̬����ʱ��, us
		int								m_iChannelScheduleTime;//�����û�ʣ��ʱ��
		bool							m_bIsChannelAvailable;//�Ƿ������ŵ�
		bool							m_bFirstTrans;//�Ƿ��ǵ�һ�δ�������
		int								m_iSumCollisionNum;//ϵͳ�ܹ���ײ����
		int								m_iWaitTime;//����RTS֮��ĵȴ�ʱ��
		double							m_dCellThroughput;//С����������ʵʱ���ϵͳ�������
		int								m_iTransmitDataTime;//�����������ݵ���ʱ�䣬���������ŵ�������,us

		bool                            m_bCCA;//eNB��ĳһʱ�̵�CCA�����ŵ��Ƿ���� 1�ǣ�0�� HFR
		int                             m_iLBTBackoffTime;//LAA eNB���˱�ʱ�䣬��λus HFR
		int                             m_iLBTCWin;//LAA eNB�����ɵľ������ڴ�С��δ�ã� HFR
		int                             m_iLBTDeferTime;//LBT�˱���deferʱ����� HFR
		int						        m_iCW;				//�������ڴ�С
#pragma endregion 
	};


}