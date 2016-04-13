#pragma once
#include "ResManager.h"
namespace LTESim
{

class ResManagerImp1 :
	public LTESim::ResManager
{
public:
	ResManagerImp1(void);
public:
	~ResManagerImp1(void);
public:
	virtual bool Require( const shared_ptr<const PhyResource>& _phytemp ) const ;		/*检测申请基本单位资源是否可用*/
	virtual bool Assign( shared_ptr< PhyResource >& _phytemp );		/*以基本单位分配未占用资源*/
	//virtual bool RequireForAllFrames( const shared_ptr< const PhyResource >& _phytemp ) const ;/*检测是否可长期占用资源*/
	//virtual bool AssignForAllFrames( const shared_ptr< const PhyResource >& _phytemp );/*分配长期资源*/
	virtual const shared_ptr<const PhyResource> FindRes( int frame, int slot, int ap, int rb ) const;
protected:
	 //const shared_ptr<const PhyResource>& FindInLongTermRes( const PhyResource &_phytemp ) const;
	 const shared_ptr<const PhyResource>& FindInTempRes( const PhyResource &_phytemp ) const;
	 bool CheckOneInOne( const shared_ptr<const PhyResource>& _phytemp ) const ;	/*子检测函数,用于Require中*/
	// bool CheckAllInOne( const shared_ptr<const PhyResource>& _phytemp ) const ;	/*子检测函数,用于RequireForAllFrames中*/
	// bool CheckAllInAll( const shared_ptr<const PhyResource>& _phytemp ) const ;	/*子检测函数,用于RequireForAllFrames中*/
	 bool CheckOneInAll( const shared_ptr<const PhyResource>& _phytemp ) const ; /*子检测函数,用于Require中*/
protected:
	std::vector< shared_ptr<const PhyResource> > m_TempoPhyRes;		/*存放已占用的单位资源,既是某个帧 某个RB 某个天线接口的一个时隙*/
	std::vector< shared_ptr<const PhyResource> > m_LongTermPhyRes; /*存放已经占用的长期资源 既是以某个帧为起始的之后所有帧某个RB 某个天线接口上的某个时隙*/

};

};
