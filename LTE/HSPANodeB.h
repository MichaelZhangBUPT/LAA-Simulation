#pragma once

#include "commondef.h"
#include "CMDEngine.h"
#include "SystemElement.h"
#include "ClockProcessor.h"
#include "POS.h"
#include "HSPATDScheduler.h"
#include "HSPAFDScheduler.h"
#include "Antenna.h"
#include <boost/enable_shared_from_this.hpp>

namespace LTESim
{
	class HSPANBReport;

	class HSPANodeB : 
		public CMDEngine, 
		public ClockProcessor, 
		public SystemElement, 
		public boost::enable_shared_from_this<HSPANodeB>
	{
	public:
		typedef boost::shared_ptr<HSPANodeB> PTR;
		typedef weak_ptr<IHSPAUE> UEPTR;
		typedef std::map<int, UEPTR> UELIST;
	public:
		HSPANodeB( int id, const POS& pos, double dMaxTxPower, shared_ptr<Antenna> pAntenna );
		virtual ~HSPANodeB();
	public:
		//CMDEngine Interface
		virtual void OnClock(const Clock& clock);

		//ClockProcessor Interface
		virtual void FrameProcess( const Clock& FrameClock);
		virtual void SubFrameProcess( const Clock& SubFrameClock );
		virtual void SlotProcess( const Clock& SlotClock );
		virtual void TxProcess( const Clock& txclock );
		virtual void RxProcess( const Clock& rxclock );
		
	public:
		const UELIST& GetUEList() const;
		void AddUE( const shared_ptr<IHSPAUE> pUE );
		void SetTDDownlinkScheduler( const shared_ptr<ITDScheduler> pScheduler );
		void SetFDDownlinkScheduler( const shared_ptr<IFDScheduler> pScheduler );

		void TransmitChannel( const shared_ptr<HSPAChannel> pChannel, int iTimeDelay );

		double GetTransmitPower() const;

		shared_ptr<Antenna> GetAntenna() const;

		vector<shared_ptr<IHSPAUE>> GetScheduledUE();

		void UpdateReportToHighLayer();

	protected:
		UELIST									m_UEList;
		shared_ptr<Antenna>				m_pAntenna;
		shared_ptr<ITDScheduler>		m_pTDScheduler;
		shared_ptr<IFDScheduler>		m_pFDScheduler;
		double										m_dMaxTxPower;
	};
}