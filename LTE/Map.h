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
	//Map,标准笛卡尔坐标系
class Map
{
public:
	Map(void);
public:
	virtual ~Map(void);

	//给出NodeB ID，返回对应的坐标
	virtual const POS GetNBPos(size_t nbid) const
	{
		return POS(m_nbx[nbid], m_nby[nbid]);
	}
	//给出小区半径生成一个UE的相对坐标
	virtual const POS GenerateUEPOS(/*UNIFORM_REAL_GEN& UEAngGen, UNIFORM_REAL_GEN& UEAmpGen,*/double radius/*小区半径*/) const = 0;

	//对应ue坐标到nb坐标的距离，考虑映射，以CenterNodeB为中心小区
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
	//对应ue坐标到nb坐标的角度0~360，考虑映射，以CenterNodeB为中心小区
	virtual double Angle( const NodeB& CenterNodeB, const ILTEUE& ue) const;;
	
	//需要加入NodeB到NodeB的映射、UE到UE的映射， 缺两组Distance和Angle函数。
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
	//生成基站坐标，采用7小区
	SevenCells();
public:
	//生成移动台坐标，均匀分布
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
	//生成移动台坐标，均匀分布
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
