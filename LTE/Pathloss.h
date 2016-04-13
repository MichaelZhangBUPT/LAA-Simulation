#pragma once
#include <vector>

#include "DB.h"
namespace LTESim
{
	class SystemElement;
	class Pathloss
	{
	public:
		Pathloss(void);
	public:
		virtual ~Pathloss(void);
	
		//给入一个长度（m），得出一个路损的db值（恒为正）
		virtual double operator()(double distance) const = 0;
		virtual double operator() (double distance,double din) const {return 0.0;}
	};
	
	class UMi_NLOS : public Pathloss
	{
	public:
		UMi_NLOS(double freq/*载波频率*/);
	public:
		virtual ~UMi_NLOS(void);
		virtual double operator() (double distance) const;	//显式类型转换,室外用户的路损
		virtual double operator() (double distance,double din) const;	//显式类型转换,室内用户的路损,distance=din+dout
	protected:
		//basic path-loss
		DB PLb(double distance) const;
	protected:
		//数据
		double m_freq;	//中心频率MHz
	};

	class UMa_NLOS : public Pathloss
	{
	public:
		UMa_NLOS(double freq/*载波频率*/);
	public:
		virtual ~UMa_NLOS(void);
		virtual double operator() (double distance) const;	//显式类型转换
		virtual double operator() (double distance,double din) const;	//显式类型转换
	protected:
		//数据
		double m_freq;	//中心频率MHz
	};
	
	class Cost231_Hata : public Pathloss
	{
	public:
		Cost231_Hata(double freq/*载波频率*//*, double hte*//*发射天线有效高度*//*, double hre*//*接收天线有效高度*//*, bool UseCM=false*//*是否使用大城市中心矫正3db*/);
	public:
		virtual ~Cost231_Hata(void);
		virtual double operator() (double distance) const;	//显式类型转换
		virtual double operator() (double distance,double din) const;
	protected:
		//接收天线矫正因子a(hre)
		//DB a(double hre) const;
	protected:
		//数据
		double m_freq;	//中心频率MHz
		//double m_hte;	//发射天线有效高度m
		//double m_hre;	//接收天线有效高度m
		//bool m_useCm;	//是否使用大城市中心矫正
	};
	
	class HSPACost231_Hata : public Pathloss
	{
	public:
		HSPACost231_Hata();
		virtual ~HSPACost231_Hata();
	public:
		virtual double operator()(double distance) const;
	};

	//HFR 以前的wifipathloss算法
	class WiFiPathLoss : public Pathloss
	{
	public:
		WiFiPathLoss( double freq/*载波频率*/ );
		virtual ~WiFiPathLoss();
		virtual double operator()(double distance) const;
	protected:
		virtual double FreeSpaceLoss(double distance) const;
	protected:
		double  m_dBP;	//转折点距离m
		double m_dFreq;	//中心频率MHz
	};

	//class WiFiPathLoss : public Pathloss
	//{
	//public:
	//	WiFiPathLoss( double freq/*载波频率*/ );
	//	virtual ~WiFiPathLoss();
	//	virtual double operator()(double distance) const;
	//protected:
	//	double m_dFreq;	//中心频率MHz
	//};

	class LAAPathloss : public Pathloss
	{
	public:
		LAAPathloss( double freq/*载波频率*/ );
		virtual ~LAAPathloss();
		virtual double operator()(double distance) const;
	protected:
		double m_dFreq;	//中心频率MHz
	};
}
