#pragma once
#include <boost/shared_ptr.hpp>
#include <set>
#include "PhyResource.h"
using boost::shared_ptr;

namespace LTESim
{
class ResManager//��Դ����.������������Դ�Ƿ����,���������
{
public:
	typedef std::set< shared_ptr<PhyResource> > PhyResourceList;	//������Դ�б�
public:
    ResManager();
public:
    virtual ~ResManager();
	virtual bool Require( const PhyResourceList& reslit ) const;
	virtual bool Assign( const PhyResourceList& reslit );
    virtual bool Require( const shared_ptr<const PhyResource>& _phytemp ) const = 0 ;		/*������������λ��Դ�Ƿ����*/
	virtual bool Assign( const shared_ptr< const PhyResource >& _phytemp ) = 0;		/*�Ի�����λ����δռ����Դ*/
	virtual void ClearBefore( int time ) = 0;/*ɾ����ʱ�̣�֡��֮ǰ����������Դ*/
	//ͨ��ָ���������phyres�ҵ����ڵ�phyres���Ӷ�ȡ�ù���
	virtual const shared_ptr<const PhyResource> FindRes( int frame, int slot, int ap, int rb ) const = 0;


};

}
