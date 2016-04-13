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
	ResManagerImp2(size_t _SLOTPERFRAME/*ÿ֡��ʱ϶��*/, size_t _RBPERSLOTPERAP/*ÿʱ϶��RB��*/, 
		size_t antennaports/*���߶˿���*/ );
public:
	~ResManagerImp2(void);
	virtual bool Require( const shared_ptr<const PhyResource>& _phytemp ) const ;		/*������������λ��Դ�Ƿ����*/
	virtual bool Assign(  const shared_ptr< const PhyResource >& _phytemp ) ;		/*�Ի�����λ����δռ����Դ*/
	virtual const shared_ptr<const PhyResource> FindRes( int frame, int slot, int ap, int rb ) const;
	void ClearBefore( int time );
protected:
	typedef std::vector<shared_ptr<const PhyResource> > SLOTRES;	//��ʱ϶Ϊ��λ�����Դ
	typedef std::vector<shared_ptr<SLOTRES> > FRAMERES;	//֡��������Դ
	std::map<int, shared_ptr<FRAMERES> > m_resources;	
	const size_t RBPERSLOTPERAP;	/*ÿʱ϶��RB��*/
	const size_t SLOTPERFRAME;	/*ÿ֡��ʱ϶��*/
	const size_t ANTENNAPORTS;	/*���߶˿���*/
};

}
