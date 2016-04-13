#include "commondef.h"
#include "PhyResource.h"
namespace LTESim
{

	PhyResource::PhyResource(  int FrameIndex, int SlotIndex, int AntennaPortIndex , int RBIndex  )
	{
		Frames = FrameIndex;
		slot = SlotIndex;
		AntennaPort = AntennaPortIndex;
		RB = RBIndex;
		m_TxPower = 0;
	}

	PhyResource::PhyResource()
	{

	}

	PhyResource::~PhyResource()
	{
	}

	int PhyResource::GetRB() const
	{
		return RB;
	}

	int PhyResource::GetSlot() const
	{
		return slot;
	}

	int PhyResource::GetFrames() const
	{
		return Frames;
	}

	int PhyResource::GetAntennaPort() const
	{
		return AntennaPort;
	}

	bool PhyResource::operator==( const PhyResource& res2 ) const
	{
		return (res2.AntennaPort == AntennaPort && res2.Frames == Frames && res2.slot == slot && res2.RB == RB);
	}

	ostream& operator<<(ostream & os ,const PhyResource& res)
	{
	   os<<res.AntennaPort<<" "<<res.GetFrames()<<" "<<res.GetSlot() <<" "<<res.GetRB()<<endl;
		return os;
	}

	void PhyResource::SetTxPower(double txpower)
	{
		m_TxPower = txpower;
	}
	
	double PhyResource::GetTxPower() const
	{
		return m_TxPower;
	}
}

