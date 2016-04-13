#pragma once

#include "commondef.h"
#include "mcs.h"
#include "ARQ.h"

namespace LTESim
{
	class ILAAUE;
	class LAANodeB;

	class LAAChannel
	{
	public:
		typedef shared_ptr<LAAChannel>	PTR;
		typedef weak_ptr<LAAChannel>		WPTR;
		typedef weak_ptr<ILAAUE>			UEPTR;
		typedef weak_ptr<LAANodeB>		NBPTR;
	public:
		LAAChannel();
		virtual ~LAAChannel();
	public:
		void	Initialization( shared_ptr<LAANodeB> pNB, shared_ptr<ILAAUE> pUE, shared_ptr<ARQ> pARQ, double dTxPower, const MCS& mcs );
		shared_ptr<ILAAUE>				GetUE()  const;
		shared_ptr<LAANodeB>			GetNB() const;
		const MCS&						GetMCS() const;
		const double					GetTxPower() const;
		const double					GetSINR() const;
		const double					GetData() const;
		shared_ptr<ARQ>			        GetARQ() const;						
		void							SetSINR( double dSINR );
		void							SetMCS( MCS mcs );
		void							SetData( double data ); 

	protected:
		UEPTR							m_pMasterUE;
		NBPTR							m_pMasterNB;
		double							m_dTxPower;
		MCS								m_MCS;
		double							m_dSINR;
		double							m_dData;	//此信道中传输的数据，bit
		weak_ptr<ARQ>					m_pARQ;
	};
}