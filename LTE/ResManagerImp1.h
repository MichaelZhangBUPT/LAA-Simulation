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
	virtual bool Require( const shared_ptr<const PhyResource>& _phytemp ) const ;		/*������������λ��Դ�Ƿ����*/
	virtual bool Assign( shared_ptr< PhyResource >& _phytemp );		/*�Ի�����λ����δռ����Դ*/
	//virtual bool RequireForAllFrames( const shared_ptr< const PhyResource >& _phytemp ) const ;/*����Ƿ�ɳ���ռ����Դ*/
	//virtual bool AssignForAllFrames( const shared_ptr< const PhyResource >& _phytemp );/*���䳤����Դ*/
	virtual const shared_ptr<const PhyResource> FindRes( int frame, int slot, int ap, int rb ) const;
protected:
	 //const shared_ptr<const PhyResource>& FindInLongTermRes( const PhyResource &_phytemp ) const;
	 const shared_ptr<const PhyResource>& FindInTempRes( const PhyResource &_phytemp ) const;
	 bool CheckOneInOne( const shared_ptr<const PhyResource>& _phytemp ) const ;	/*�Ӽ�⺯��,����Require��*/
	// bool CheckAllInOne( const shared_ptr<const PhyResource>& _phytemp ) const ;	/*�Ӽ�⺯��,����RequireForAllFrames��*/
	// bool CheckAllInAll( const shared_ptr<const PhyResource>& _phytemp ) const ;	/*�Ӽ�⺯��,����RequireForAllFrames��*/
	 bool CheckOneInAll( const shared_ptr<const PhyResource>& _phytemp ) const ; /*�Ӽ�⺯��,����Require��*/
protected:
	std::vector< shared_ptr<const PhyResource> > m_TempoPhyRes;		/*�����ռ�õĵ�λ��Դ,����ĳ��֡ ĳ��RB ĳ�����߽ӿڵ�һ��ʱ϶*/
	std::vector< shared_ptr<const PhyResource> > m_LongTermPhyRes; /*����Ѿ�ռ�õĳ�����Դ ������ĳ��֡Ϊ��ʼ��֮������֡ĳ��RB ĳ�����߽ӿ��ϵ�ĳ��ʱ϶*/

};

};
