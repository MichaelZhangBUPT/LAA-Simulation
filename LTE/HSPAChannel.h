#pragma once

#include "commondef.h"
#include "mcs.h"
#include "Matrix.h"

namespace LTESim
{
	class IHSPAUE;
	class HSPANodeB;
	class HSPAPhyResource;
	class HARQ;

	class HSPAChannel
	{
	public:
		typedef shared_ptr<HSPAChannel>	PTR;
		typedef weak_ptr<HSPAChannel>		WPTR;
		typedef weak_ptr<IHSPAUE>			UEPTR;
		typedef weak_ptr<HSPANodeB>		NBPTR;
		typedef weak_ptr<HARQ>				HARQPTR;
	public:
		//���캯���л���Ҫ��Ӷ���ı������г�ʼ��
		HSPAChannel();
		virtual ~HSPAChannel();
	public:
		void										Initialization( shared_ptr<HSPANodeB> pNodeB, shared_ptr<IHSPAUE> pUE, LinkDirection linkDirection, shared_ptr<HARQ> pHARQ, double dTxPower, const MCS& mcs, const std::vector<HSPAPhyResource>& refPhyResVec, const Matrix& refDLWeightMatrix = Matrix() );
		shared_ptr<IHSPAUE>			GetUE()  const;
		shared_ptr<HSPANodeB>	GetNodeB() const;
		LinkDirection						GetLinkDirection()  const;
		shared_ptr<HARQ>				GetHARQ() const;
		bool										IsLastSlot( int iSlot ) const;
		bool										IsUsedSlot( int iSlot )  const;
		void										RestoreC2I( double dC2I );
		const std::vector<double>&	GetC2IVector() const;
		const MCS&							GetMCS() const;
		const double							GetTxPower() const;
		const set<int>&						GetStreamSet() const;
		const Matrix&						GetWeightDL() const;
		const set<int>&						GetCodeChannelSet() const;

	protected:
		UEPTR							m_pMasterUE;
		NBPTR							m_pMasterNodeB;
		LinkDirection					m_LinkDirection;
		HARQPTR						m_pHARQ;						//����Channel��������ʱ�������ھ�����ʱ���ո�Channel��ʱ��Ӧ�ø��µ������Channel��ص�HARQ

		std::set<int>						m_StreamSet;
		int										m_iCarrierIndex;
		std::set<int>						m_CodeChannelSet;
		int										m_SFVec;
		int										m_iFrameIndex;
		std::set<int>						m_SlotSet;
		std::vector<double>			m_C2IVec;		//ʵֵ����dBֵ
		std::vector<HSPAPhyResource>	m_PhyResourceVec;
		Matrix								m_DLWeightMatrix;

		double								m_dTxPower;
		MCS									m_MCS;
	};
}