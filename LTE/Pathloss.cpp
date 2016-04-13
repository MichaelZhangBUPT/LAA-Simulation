#include "commondef.h"
#include <cmath>
#include "SystemElement.h"
#include "Pathloss.h"
#include "System.h"

namespace LTESim
{
#pragma region Pathloss_base

	Pathloss::Pathloss(void)
	{

	}

	Pathloss::~Pathloss(void)
	{

	}

#pragma endregion Pathloss_base

#pragma region UMi_NLOS

	UMi_NLOS::UMi_NLOS( double freq )
		:m_freq(freq)
	{

	}

	UMi_NLOS::~UMi_NLOS(void)
	{

	}

	//由距离直接返回室外用户的路损
	double UMi_NLOS::operator()( double distance ) const
	{
		return dBToReal( PLb(distance) );
	}

	//由距离和din直接返回室内用户的路损
	double UMi_NLOS::operator()( double distance, double din ) const
	{
		if ( din > 0.0 )
			return dBToReal( PLb(distance) + 20.0 + 0.5*din );
		else
			return dBToReal( PLb(distance) );
	}

	//basic path-loss
	DB UMi_NLOS::PLb( double distance ) const
	{
		return DB( 36.7*log10(distance) + 22.7 + 26*log10(m_freq/1000) );
	}

#pragma endregion UMi_NLOS

#pragma region UMa_NLOS

	UMa_NLOS::UMa_NLOS( double freq )
		:m_freq(freq)
	{

	}

	UMa_NLOS::~UMa_NLOS(void)
	{
		
	}

	//由距离直接返回室外用户的路损
	double UMa_NLOS::operator()( double distance ) const
	{
		double loss = 161.04 - 7.1*log10(20.0) + 7.5*log10(20.0) - 22.002*log10(25.0) + (43.42-3.1*log10(25.0))*(log10(distance)-3)
			+ 20*log10(m_freq/1000) - 3.2*(log10(11.75*1.5))*(log10(11.75*1.5)) + 4.97;
		return dBToReal( loss );
	}

	double UMa_NLOS::operator()( double distance, double din ) const
	{
		double loss = 161.04 - 7.1*log10(20.0) + 7.5*log10(20.0) - 22.002*log10(25.0) + (43.42-3.1*log10(25.0))*(log10(distance)-3)
			+ 20*log10(m_freq/1000) - 3.2*(log10(11.75*1.5))*(log10(11.75*1.5)) + 4.97;
		return dBToReal( loss );
	}

#pragma endregion UMa_NLOS

#pragma region COST_231_HATA

	Cost231_Hata::Cost231_Hata( double freq/*, double hte, double hre, bool UseCM*/ )
	:m_freq(freq)/*,m_hte(hte),m_hre(hre),m_useCm(UseCM)*/
	{

	}

	Cost231_Hata::~Cost231_Hata(void)
	{

	}

	//由距离直接返回路损
	double Cost231_Hata::operator()( double distance ) const
	{
		//不同的System，使用的Pathloss的路损公式不同
		return dBToReal(35.3 + 37.6 * log10(distance));	//15.3，加了20dB的穿透损耗

		//return dBToReal(
		//	46.3 + 33.9* log10(m_freq) 
		//	- 13.82 * log10(m_hte) 
		//	- a(m_hre) 
		//	+ (44.9 - 6.55 * log10(m_hte)) *( log10(distance) - 3)
		//	+ (m_useCm ? 3 : 0)
		//);
	}

	double Cost231_Hata::operator()( double distance, double din ) const
	{
		return dBToReal(35.3 + 37.6 * log10(distance));
	}

	//接受天线校正因子
	//DB Cost231_Hata::a( double hre ) const
	//{
	//	double temp = log10(1.54*hre) ;
	//	return DB(3.2 * temp * temp - 1.1);
	//}

#pragma endregion COST_231_HATA

#pragma region HSPACost231_Hata

	HSPACost231_Hata::HSPACost231_Hata()
	{

	}

	HSPACost231_Hata::~HSPACost231_Hata()
	{

	}

	double HSPACost231_Hata::operator ()(double distance) const
	{
		//cout << "db  " << dBToReal(120.0) << endl;
		//return 20;
		return dBToReal(130.9 + 37.6 * log10(distance/1000.0));
	}

#pragma endregion

#pragma region WiFiPathLoss
	
	////HFR wifi与LAA的路损模型相同

	//WiFiPathLoss::WiFiPathLoss( double freq/*载波频率*/ )
	//	: m_dFreq(freq)
	//{

	//}

	//WiFiPathLoss::~WiFiPathLoss()
	//{

	//}

	//double WiFiPathLoss::operator ()(double distance) const
	//{

	//	return 43.3*log10(distance) + 11.5 + 20*log10(m_dFreq/1000);
	//}

	WiFiPathLoss::WiFiPathLoss( double freq/*载波频率*/ )
		: m_dFreq(freq)
	{
		m_dBP = 10.0;	//信道模型是D
	}

	WiFiPathLoss::~WiFiPathLoss()
	{

	}

	double WiFiPathLoss::operator ()(double distance) const
	{
		if(distance <= m_dBP)
			return dBToReal( FreeSpaceLoss(distance) );
		else
			return dBToReal( FreeSpaceLoss(m_dBP) + 35*log10(distance/m_dBP) );
	}


	double WiFiPathLoss::FreeSpaceLoss(double distance) const
	{
		double gama = 2.0;
		//下式算出的结果已经是dB值
		return 32.4 + 10*gama*log10(m_dFreq) + 10*gama*log10(distance/1000.0);
	}

#pragma endregion

#pragma region   LAApathloss InH

	LAAPathloss::LAAPathloss( double freq/*载波频率*/ )
		: m_dFreq(freq)
	{
		
	}

	LAAPathloss::~LAAPathloss()
	{

	}

	double LAAPathloss::operator ()(double distance) const
	{
		
		return 43.3*log10(distance) + 11.5 + 20*log10(m_dFreq/1000);
	}
#pragma endregion   

}
