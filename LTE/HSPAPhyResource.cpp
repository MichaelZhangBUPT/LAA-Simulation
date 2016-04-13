#include "HSPAPhyResource.h"
#include <iostream>

namespace LTESim
{

	HSPAPhyResource::HSPAPhyResource( int CarrierIndex, int CodeChIndex, int AntennaPortIndex, int iFrameIndex, int iSlotIndex )
	{
		m_CarrierIndex = CarrierIndex;
		m_CodeChIndex = CodeChIndex;
		m_AntennaPortIndex = AntennaPortIndex;
		m_TxPower = 0;
		m_iFrameIndex = iFrameIndex;
		m_iSlotIndex = iSlotIndex;
	}

	HSPAPhyResource::HSPAPhyResource()
	{

	}

	HSPAPhyResource::~HSPAPhyResource()
	{
	}

	int HSPAPhyResource::GetCarrierIndex() const
	{
		return m_CarrierIndex;
	}

	int HSPAPhyResource::GetCodeChIndex() const
	{
		return m_CodeChIndex;
	}

	int HSPAPhyResource::GetAntennaPort() const
	{
		return m_AntennaPortIndex;
	}

	int HSPAPhyResource::GetFrame() const
	{
		return m_iFrameIndex;
	}

	int HSPAPhyResource::GetSlot() const
	{
		return m_iSlotIndex;
	}

	bool HSPAPhyResource::operator == ( const HSPAPhyResource& res2 ) const
	{
		return (res2.m_AntennaPortIndex == m_AntennaPortIndex && res2.m_CarrierIndex == m_CarrierIndex && res2.m_CodeChIndex == m_CodeChIndex);
	}

	//÷ÿ‘ÿ°∞<°±°£“™◊¢“‚À≥–Ú ‘ÿ≤®±‡∫≈,¬Îµ¿±‡∫≈,ÃÏœﬂ∂Àø⁄
	bool HSPAPhyResource::operator < (const HSPAPhyResource& res ) const
	{
		if (GetCarrierIndex() < res.GetCarrierIndex())
		{
			return true;
		}
		else if(GetCarrierIndex() > res.GetCarrierIndex())
		{
			return false;
		}

		if (GetCodeChIndex() < res.GetCodeChIndex())
		{
			return true;
		}
		else if (GetCodeChIndex() > res.GetCodeChIndex())
		{
			return false;
		}

		if (GetAntennaPort() < res.GetAntennaPort())
		{
			return true;
		}
		return false;
	}

	std::ostream& operator<<(std::ostream & os ,const HSPAPhyResource& res)
	{
		os<<res.GetAntennaPort()<<" "<<res.GetCarrierIndex()<<" "<<res.GetCodeChIndex() <<std::endl;
		return os;
	}

	void HSPAPhyResource::SetTxPower(double txpower)
	{
		m_TxPower = txpower;
	}

	double HSPAPhyResource::GetTxPower() const
	{
		return m_TxPower;
	}
}