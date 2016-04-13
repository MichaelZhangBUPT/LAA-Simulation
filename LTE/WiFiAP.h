#pragma once

//#include <map>
//#include <set>
#include "commondef.h"
#include <boost/enable_shared_from_this.hpp>
#include "WiFiUE.h"
#include "WiFiChannel.h"

#include "Clock.h"
#include "CMDEngine.h"
#include "POS.h"
#include "ClockProcessor.h"
#include "SystemElement.h"
#include "Antenna.h"



namespace LTESim
{
	class WiFiAP : 
		public CMDEngine, 
		public ClockProcessor, 
		public SystemElement, 
		public boost::enable_shared_from_this<WiFiAP>  //��һ���� A ��������ϣ��ʹ�� shared_ptr �������������ʱ��
		                                               //������Ҫ���Լ�����ĺ����������� shared_ptr ������������ʱ��
													   //�������� A �� enable_shared_from_this �̳�
	{
	public:
		typedef boost::shared_ptr<WiFiAP> PTR;
		typedef weak_ptr<IWiFiUE> WiFiUEPTR;
		typedef std::map<int, WiFiUEPTR> WiFiUELIST;
	public:
		WiFiAP(int id, const POS& pos, double maxtxpower, shared_ptr<Antenna> pAntenna);
		virtual ~WiFiAP(void);

		void OnClock(const Clock& clock);
		void WiFiClockProcess(const Clock& wificlock);
		virtual void FrameProcess( const Clock& FrameClock){};
		virtual void SubFrameProcess( const Clock& SubFrameClock ){};
		virtual void SlotProcess( const Clock& SlotClock ){};
		virtual void TxProcess( const Clock& txclock ){};
		virtual void RxProcess( const Clock& rxclock ){};

		void AddUE( const shared_ptr<IWiFiUE> pUE );
		shared_ptr<Antenna> GetTxAntenna() const;
		double GetMaxTxPower() const;
		const WiFiUELIST& GetUEList() const;

		void ChannelIdleProcess();
		void ChannelBusyProcess();
		void ChannelCollisionProcess();

		void GetCandidateUEList();
		void InitChannel();
		void TransmitChannel( const shared_ptr<WiFiChannel> pChannel, int iTimeDelay );
		const int GetSumCollisionNum() const;

		virtual void UpdateCellThroughput( double thr );
		virtual double GetCellThroughput();

		virtual int GetTransmitDataTime();
		ChannelState GetChannelState();


#pragma region ��Ա����
	protected:
		//WiFiUE							m_APItself;	//AP�������ڴ�������ҵ��
		WiFiUELIST						m_WiFiUEList;//�����AP��WiFiUE
		WiFiUELIST						m_CandidateUEList;//׼����Ҫ���շ����UE
		shared_ptr<Antenna>				m_pTxAntenna;//ҵ���ŵ���������
		ChannelState				m_ChannelState;//�ŵ�״̬
		double							m_dMaxTxPower;//����书�ʣ�mW
		double							m_dDataMaxLength;//һ�δ���������ݣ�bit���Ѳ�ʹ��
		int								m_TXOPLimit;//һ��ռ���ŵ������ʱ����us
		int								m_iChannelIdleTime;//�ŵ�����״̬�ĳ���ʱ�䣬us
		int								m_iChannelBusyTime;//�ŵ�æµ״̬��ʣ��ʱ��, us
		bool							m_bIsChannelAvailable;//�Ƿ������ŵ�
		bool							m_bFirstTrans;//�Ƿ��ǵ�һ�δ�������
		int								m_iSumCollisionNum;//ϵͳ�ܹ���ײ����
		int								m_iWaitTime;//����RTS֮��ĵȴ�ʱ��
		double							m_dCellThroughput;//С����������ʵʱ���ϵͳ�������
		int								m_iTransmitDataTime;//�����������ݵ���ʱ�䣬���������ŵ�������,us
	    bool                            cs;
#pragma endregion 
	};


}