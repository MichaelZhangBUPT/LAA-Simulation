#include "commondef.h"
#include "HSPAChannel.h"
#include "HSPAUE.h"
#include "HSPANodeB.h"
#include "HSPAPhyResource.h"

namespace LTESim
{
	HSPAChannel::HSPAChannel() : m_MCS( 0, SYSTEM_HSPA )
	{

	}

	HSPAChannel::~HSPAChannel()
	{

	}

	void HSPAChannel::Initialization(	shared_ptr<HSPANodeB> pNodeB, shared_ptr<IHSPAUE> pUE, LinkDirection linkDirection, shared_ptr<HARQ> pHARQ, double dTxPower, const MCS& mcs, 
														const std::vector<HSPAPhyResource>& refPhyResVec, const Matrix& refDLWeightMatrix /* = Matrix */)
	{
		m_pMasterNodeB = pNodeB;
		m_pMasterUE = pUE;
		m_LinkDirection = linkDirection;
		m_pHARQ = pHARQ;

		m_dTxPower = dTxPower;
		m_MCS = mcs;
		m_PhyResourceVec = refPhyResVec;
		m_DLWeightMatrix = refDLWeightMatrix;
		
		m_iCarrierIndex = -1;
		m_iFrameIndex = -1;
		for ( std::vector<HSPAPhyResource>::const_iterator vecIter = refPhyResVec.begin(); vecIter != refPhyResVec.end(); ++vecIter )
		{
			if ( m_iCarrierIndex < 0 )
				m_iCarrierIndex = vecIter->GetCarrierIndex();
			else
				MAKE_SURE( m_iCarrierIndex == vecIter->GetCarrierIndex(), "HSPAChannel::Initialization" );

			if ( m_iFrameIndex < 0 )
				m_iFrameIndex = vecIter->GetFrame();
			else
				MAKE_SURE( m_iFrameIndex == vecIter->GetFrame(), "HSPAChannel:Initialization" );

			m_SlotSet.insert( vecIter->GetSlot() );
			m_StreamSet.insert( vecIter->GetAntennaPort() );
			m_CodeChannelSet.insert( vecIter->GetCodeChIndex() );
		}
	}

	shared_ptr<IHSPAUE> HSPAChannel::GetUE() const
	{
		return m_pMasterUE.lock();
	}

	shared_ptr<HSPANodeB> HSPAChannel::GetNodeB() const
	{
		return m_pMasterNodeB.lock();
	}

	LinkDirection HSPAChannel::GetLinkDirection() const
	{
		return m_LinkDirection;
	}

	shared_ptr<HARQ> HSPAChannel::GetHARQ() const
	{
		return m_pHARQ.lock();
	}

	bool HSPAChannel::IsLastSlot( int iSlot ) const
	{
		if( m_SlotSet.size() && ( *m_SlotSet.rbegin() == iSlot ) )
			return true;
		return false;
	}

	bool HSPAChannel::IsUsedSlot(int iSlot) const
	{
		if ( m_SlotSet.count( iSlot ) )
			return true;
		return false;
	}

	void HSPAChannel::RestoreC2I(double dC2I )
	{
		m_C2IVec.push_back( dC2I );
	}

	const std::vector<double>& HSPAChannel::GetC2IVector() const
	{
		return m_C2IVec;
	}

	const MCS&	HSPAChannel::GetMCS() const
	{
		return m_MCS;
	}

	const double HSPAChannel::GetTxPower() const
	{
		return m_dTxPower;
	}

	const set<int>& HSPAChannel::GetStreamSet() const
	{
		return m_StreamSet;
	}

	const Matrix& HSPAChannel::GetWeightDL() const
	{
		return m_DLWeightMatrix;
	}

	const set<int>& HSPAChannel::GetCodeChannelSet() const
	{
		return m_CodeChannelSet;
	}
}