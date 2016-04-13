#include "commondef.h"
#include "WiFiChannel.h"
//#include "WiFiUE.h"
//#include "WiFiAP.h"

namespace LTESim
{
	WiFiChannel::WiFiChannel() : m_MCS( 0, SYSTEM_WIFI )
	{

	}

	WiFiChannel::~WiFiChannel()
	{

	}

	void WiFiChannel::Initialization( shared_ptr<WiFiAP> pAP, shared_ptr<IWiFiUE> pUE, shared_ptr<ARQ> pARQ,  double dTxPower, const MCS& mcs )
	{
		m_pMasterAP = pAP;
		m_pMasterUE = pUE;
		m_pARQ = pARQ;
		m_dTxPower = dTxPower;
		m_MCS = mcs;
	}

	shared_ptr<IWiFiUE> WiFiChannel::GetUE() const
	{
		return m_pMasterUE.lock();
	}

	shared_ptr<WiFiAP> WiFiChannel::GetAP() const
	{
		return m_pMasterAP.lock();
	}

	const MCS&	WiFiChannel::GetMCS() const
	{
		return m_MCS;
	}

	const double WiFiChannel::GetTxPower() const
	{
		return m_dTxPower;
	}

	const double WiFiChannel::GetSINR() const
	{
		return m_dSINR;
	}

	const double WiFiChannel::GetData() const
	{
		return m_dData;
	}

	shared_ptr<ARQ> WiFiChannel::GetARQ() const
	{
		return m_pARQ.lock();
	}

	void WiFiChannel::SetSINR( double dSINR )
	{
		m_dSINR = dSINR;
	}

	void WiFiChannel::SetMCS( MCS mcs )
	{
		m_MCS = mcs;
	}

	void WiFiChannel::SetData( double data )
	{
		m_dData = data;
	}


}