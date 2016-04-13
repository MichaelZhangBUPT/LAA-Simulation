#include "commondef.h"
#include "LAAChannel.h"


namespace LTESim
{
	LAAChannel::LAAChannel() : m_MCS( 0, SYSTEM_LAA )
	{

	}

	LAAChannel::~LAAChannel()
	{

	}

	void LAAChannel::Initialization( shared_ptr<LAANodeB> pNB, shared_ptr<ILAAUE> pUE, shared_ptr<ARQ> pARQ,  double dTxPower, const MCS& mcs )
	{
		m_pMasterNB = pNB;
		m_pMasterUE = pUE;
		m_pARQ = pARQ;
		m_dTxPower = dTxPower;
		m_MCS = mcs;
	}

	shared_ptr<ILAAUE> LAAChannel::GetUE() const
	{
		return m_pMasterUE.lock();
	}

	shared_ptr<LAANodeB> LAAChannel::GetNB() const
	{
		return m_pMasterNB.lock();
	}

	const MCS&	LAAChannel::GetMCS() const
	{
		return m_MCS;
	}

	const double LAAChannel::GetTxPower() const
	{
		return m_dTxPower;
	}

	const double LAAChannel::GetSINR() const
	{
		return m_dSINR;
	}

	const double LAAChannel::GetData() const
	{
		return m_dData;
	}

	shared_ptr<ARQ> LAAChannel::GetARQ() const
	{
		return m_pARQ.lock();
	}

	void LAAChannel::SetSINR( double dSINR )
	{
		m_dSINR = dSINR;
	}

	void LAAChannel::SetMCS( MCS mcs )
	{
		m_MCS = mcs;
	}

	void LAAChannel::SetData( double data )
	{
		m_dData = data;
	}


}