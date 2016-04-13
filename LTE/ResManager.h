#pragma once
#include <boost/shared_ptr.hpp>
#include <set>
#include "PhyResource.h"
using boost::shared_ptr;

namespace LTESim
{
class ResManager//资源管理.负责检测申请资源是否可用,并将其分配
{
public:
	typedef std::set< shared_ptr<PhyResource> > PhyResourceList;	//物理资源列表
public:
    ResManager();
public:
    virtual ~ResManager();
	virtual bool Require( const PhyResourceList& reslit ) const;
	virtual bool Assign( const PhyResourceList& reslit );
    virtual bool Require( const shared_ptr<const PhyResource>& _phytemp ) const = 0 ;		/*检测申请基本单位资源是否可用*/
	virtual bool Assign( const shared_ptr< const PhyResource >& _phytemp ) = 0;		/*以基本单位分配未占用资源*/
	virtual void ClearBefore( int time ) = 0;/*删除此时刻（帧）之前（含）的资源*/
	//通过指定的虚拟的phyres找到存在的phyres，从而取得功率
	virtual const shared_ptr<const PhyResource> FindRes( int frame, int slot, int ap, int rb ) const = 0;


};

}
