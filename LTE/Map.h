#pragma once
#include "POS.h"
#include "RandomManager.h"

namespace LTESim
{
	class NodeB;
	class ILTEUE;
	class HSPANodeB;
	class IHSPAUE;
	class IWiFiUE;
	class WiFiAP;
	class LAANodeB;
	class ILAAUE;
	//Map,��׼�ѿ�������ϵ
class Map
{
public:
	Map(void);
public:
	virtual ~Map(void);

	//����NodeB ID�����ض�Ӧ������
	virtual const POS GetNBPos(size_t nbid) const
	{
		return POS(m_nbx[nbid], m_nby[nbid]);
	}
	//����С���뾶����һ��UE���������
	virtual const POS GenerateUEPOS(/*UNIFORM_REAL_GEN& UEAngGen, UNIFORM_REAL_GEN& UEAmpGen,*/double radius/*С���뾶*/) const = 0;

	//��Ӧue���굽nb����ľ��룬����ӳ�䣬��CenterNodeBΪ����С��
	double Distance(const NodeB& CenterNodeB, const ILTEUE& ue) const;

	double Distance(const HSPANodeB& CenterNodeB, const IHSPAUE& ue) const;
	
	double Distance(const WiFiAP& ap, const IWiFiUE& ue) const;

	double Distance(const LAANodeB& nb, const ILAAUE& ue) const;
//HFR
	double Distance(const LAANodeB& nb1, const LAANodeB& nb2) const;

	double Distance(const LAANodeB& nb,const WiFiAP& ap) const;

	double Distance(const WiFiAP& ap1,const WiFiAP& ap2) const;

	double Distance(const WiFiAP& ap1,const ILAAUE& ue) const;

	double Distance(const LAANodeB& nb,const IWiFiUE& ue) const;
//HFR
	//��Ӧue���굽nb����ĽǶ�0~360������ӳ�䣬��CenterNodeBΪ����С��
	virtual double Angle( const NodeB& CenterNodeB, const ILTEUE& ue) const;;
	
	//��Ҫ����NodeB��NodeB��ӳ�䡢UE��UE��ӳ�䣬 ȱ����Distance��Angle������
 protected:
 	virtual const POS WrapAroundMap(const NodeB& CenterNodeB, const ILTEUE& ue) const = 0;
 	virtual const POS WrapAroundMap(const HSPANodeB& CenterNodeB, const IHSPAUE& ue) const = 0;
protected:
	std::vector<double> m_nbx;
	std::vector<double> m_nby;
    
};

 
class SevenCells:public Map
{
public:
	//���ɻ�վ���꣬����7С��
	SevenCells();
public:
	//�����ƶ�̨���꣬���ȷֲ�
	const POS GenerateUEPOS(/*UNIFORM_REAL_GEN& UEAngGen, UNIFORM_REAL_GEN& UEAmpGen,*/double radius) const ;
	const POS WrapAroundMap(const NodeB& CenterNodeB, const ILTEUE& ue) const;
	const POS WrapAroundMap(const HSPANodeB& CenterNodeB, const IHSPAUE& ue) const;
	const double GetNBRadius() const;
protected:
	double NBRadius;
	/*mutable UNIFORM_REAL_GEN m_UEAngleGen;
    mutable UNIFORM_REAL_GEN m_UEAmpGen;*/
};
 
class CircleCell:public Map
{
public:
	CircleCell();
	~CircleCell();
public:
	//�����ƶ�̨���꣬���ȷֲ�
	const POS GenerateUEPOS(double radius) const ;
	const double GetCircleRadius() const;
	const POS WrapAroundMap(const NodeB& CenterNodeB, const ILTEUE& ue) const;
	const POS WrapAroundMap(const HSPANodeB& CenterNodeB, const IHSPAUE& ue) const;

protected:
	double CircleRadius;
};

class Indoor:public Map
{
public:
    Indoor();
public:
    const POS GenerateUEPOS(double radius) const ;
    const double GetIndoorRadius() const;

    const POS WrapAroundMap(const NodeB& CenterNodeB, const ILTEUE& ue) const;
    const POS WrapAroundMap(const HSPANodeB& CenterNodeB, const IHSPAUE& ue) const;

protected:
    double IndoorRadius;
};
}
