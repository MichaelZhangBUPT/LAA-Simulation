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
	
		//����һ�����ȣ�m�����ó�һ��·���dbֵ����Ϊ����
		virtual double operator()(double distance) const = 0;
		virtual double operator() (double distance,double din) const {return 0.0;}
	};
	
	class UMi_NLOS : public Pathloss
	{
	public:
		UMi_NLOS(double freq/*�ز�Ƶ��*/);
	public:
		virtual ~UMi_NLOS(void);
		virtual double operator() (double distance) const;	//��ʽ����ת��,�����û���·��
		virtual double operator() (double distance,double din) const;	//��ʽ����ת��,�����û���·��,distance=din+dout
	protected:
		//basic path-loss
		DB PLb(double distance) const;
	protected:
		//����
		double m_freq;	//����Ƶ��MHz
	};

	class UMa_NLOS : public Pathloss
	{
	public:
		UMa_NLOS(double freq/*�ز�Ƶ��*/);
	public:
		virtual ~UMa_NLOS(void);
		virtual double operator() (double distance) const;	//��ʽ����ת��
		virtual double operator() (double distance,double din) const;	//��ʽ����ת��
	protected:
		//����
		double m_freq;	//����Ƶ��MHz
	};
	
	class Cost231_Hata : public Pathloss
	{
	public:
		Cost231_Hata(double freq/*�ز�Ƶ��*//*, double hte*//*����������Ч�߶�*//*, double hre*//*����������Ч�߶�*//*, bool UseCM=false*//*�Ƿ�ʹ�ô�������Ľ���3db*/);
	public:
		virtual ~Cost231_Hata(void);
		virtual double operator() (double distance) const;	//��ʽ����ת��
		virtual double operator() (double distance,double din) const;
	protected:
		//�������߽�������a(hre)
		//DB a(double hre) const;
	protected:
		//����
		double m_freq;	//����Ƶ��MHz
		//double m_hte;	//����������Ч�߶�m
		//double m_hre;	//����������Ч�߶�m
		//bool m_useCm;	//�Ƿ�ʹ�ô�������Ľ���
	};
	
	class HSPACost231_Hata : public Pathloss
	{
	public:
		HSPACost231_Hata();
		virtual ~HSPACost231_Hata();
	public:
		virtual double operator()(double distance) const;
	};

	//HFR ��ǰ��wifipathloss�㷨
	class WiFiPathLoss : public Pathloss
	{
	public:
		WiFiPathLoss( double freq/*�ز�Ƶ��*/ );
		virtual ~WiFiPathLoss();
		virtual double operator()(double distance) const;
	protected:
		virtual double FreeSpaceLoss(double distance) const;
	protected:
		double  m_dBP;	//ת�۵����m
		double m_dFreq;	//����Ƶ��MHz
	};

	//class WiFiPathLoss : public Pathloss
	//{
	//public:
	//	WiFiPathLoss( double freq/*�ز�Ƶ��*/ );
	//	virtual ~WiFiPathLoss();
	//	virtual double operator()(double distance) const;
	//protected:
	//	double m_dFreq;	//����Ƶ��MHz
	//};

	class LAAPathloss : public Pathloss
	{
	public:
		LAAPathloss( double freq/*�ز�Ƶ��*/ );
		virtual ~LAAPathloss();
		virtual double operator()(double distance) const;
	protected:
		double m_dFreq;	//����Ƶ��MHz
	};
}
