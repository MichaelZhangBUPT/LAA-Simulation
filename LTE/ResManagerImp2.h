#pragma once
#include <map>
#include <vector>
#include "ResManager.h"
namespace LTESim
{
class ResManagerImp2 :
	public LTESim::ResManager
{
public:
	ResManagerImp2(size_t _SLOTPERFRAME/*每帧的时隙数*/, size_t _RBPERSLOTPERAP/*每时隙的RB数*/, 
		size_t antennaports/*天线端口数*/ );
public:
	~ResManagerImp2(void);
	virtual bool Require( const shared_ptr<const PhyResource>& _phytemp ) const ;		/*检测申请基本单位资源是否可用*/
	virtual bool Assign(  const shared_ptr< const PhyResource >& _phytemp ) ;		/*以基本单位分配未占用资源*/
	virtual const shared_ptr<const PhyResource> FindRes( int frame, int slot, int ap, int rb ) const;
	void ClearBefore( int time );
protected:
	typedef std::vector<shared_ptr<const PhyResource> > SLOTRES;	//以时隙为单位存放资源
	typedef std::vector<shared_ptr<SLOTRES> > FRAMERES;	//帧的物理资源
	std::map<int, shared_ptr<FRAMERES> > m_resources;	
	const size_t RBPERSLOTPERAP;	/*每时隙的RB数*/
	const size_t SLOTPERFRAME;	/*每帧的时隙数*/
	const size_t ANTENNAPORTS;	/*天线端口数*/
};

}
