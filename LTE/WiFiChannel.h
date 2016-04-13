#pragma once

#include "commondef.h"
#include "mcs.h"
//#include "WiFiUE.h"
#include "ARQ.h"

namespace LTESim
{
	class IWiFiUE;
	class WiFiAP;

	class WiFiChannel
	{
	public:
		typedef shared_ptr<WiFiChannel>	PTR;
		typedef weak_ptr<WiFiChannel>		WPTR;
		typedef weak_ptr<IWiFiUE>			UEPTR;
		typedef weak_ptr<WiFiAP>		APPTR;
	public:
		WiFiChannel();
		virtual ~WiFiChannel();
	public:
		void	Initialization( shared_ptr<WiFiAP> pAP, shared_ptr<IWiFiUE> pUE, shared_ptr<ARQ> pARQ, double dTxPower, const MCS& mcs );
		shared_ptr<IWiFiUE>				GetUE()  const;
		shared_ptr<WiFiAP>				GetAP() const;
		const MCS&						GetMCS() const;
		const double					GetTxPower() const;
		const double					GetSINR() const;
		const double					GetData() const;
		shared_ptr<ARQ>			GetARQ() const;						
		void							SetSINR( double dSINR );
		void							SetMCS( MCS mcs );
		void							SetData( double data ); 

	protected:
		UEPTR							m_pMasterUE;
		APPTR							m_pMasterAP;
		double							m_dTxPower;
		MCS								m_MCS;
		double							m_dSINR;
		double							m_dData;	//此信道中传输的数据，bit
		weak_ptr<ARQ>					m_pARQ;
	};
}