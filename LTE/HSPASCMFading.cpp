#include "HSPASCMFading.h"

namespace LTESim
{   
	using namespace std;
	class IHSPAUE;
	class HSPANodeB;
	class System;
	class RunParameters;
	class Math;

	HSPASCMFading::HSPASCMFading(void):Samples(50000),RxAntennaNum(2),TxAntennaNum(2),m_goffset(0)
		,FadingOper(10),FreqNum(4)
	{
		InitialSCMFading();
	}

	HSPASCMFading::~HSPASCMFading(void)
	{
	}

	int HSPASCMFading::CalcAngleIndex( double angle ) const
	{
		while(angle < 0)
		{
			angle += 360;
		}
		assert(angle >= 0 && angle <= 360);
		//��������ȡ��
		return static_cast<int>( angle / AngleResolution + 0.5 ) % (360/AngleResolution);
	}



	void HSPASCMFading::InitialSCMFading()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		std::string FadingLocation = pSystem->GetConfig().GetString("FADING_TABLE_LOCATION");
		FadingLocation += "HSPASCMFadingResults3.bin";

		ifstream FastFadingFile(FadingLocation.c_str(), ios::binary);	//�������ļ��ж�ȡ

		if(FastFadingFile.fail())
		{
			throw std::logic_error("The file cannot be opened.\n");
		}

		for (int angle = 0; angle < 360 / AngleResolution; ++angle)
		{
			for (int RxAn = 0;RxAn < RxAntennaNum;++RxAn)
			{
				for (int TxAn = 0;TxAn < TxAntennaNum;++TxAn)
				{   
					FastFadingFile.read((char*)m_fading[angle][RxAn][TxAn], sizeof(m_fading[angle][RxAn][TxAn]));
				}
			}
		}

		size_t nbnum = pSystem->GetHSPANBList().size();
		size_t uenum = pSystem->GetUEList().size() + pSystem->GetHSPAUEList().size() + pSystem->GetDualModeLTEHSPAUEList().size();
		for(size_t nb = 0;nb < nbnum;++nb)
		{
			m_offset[nb].resize(uenum);
			m_angle[nb].resize(uenum);
			for(size_t ue = 0; ue < uenum; ue++)
			{
				CalcIndividualOffset(nb, ue);

				IUE::PTR RxUE = pSystem->GetBaseUE(ue);	      //��ý��ܶ�
				HSPANodeB::PTR TxNodeB = pSystem->GetHSPANodeBFromOffsetID(nb);	//��÷��Ͷ�

				double angle= ::Angle(TxNodeB->GetPos(), RxUE->GetPos());	//��ýǶ�

				int iangle = CalcAngleIndex(angle);      
				m_angle[nb][ue]=iangle;
			}
		}

	}

	complex<double> HSPASCMFading::GetFading( int nbid/*NodeB��ʾ*/, int ueid/*UE��ʾ*/, int PathIndex/*�ྶ��Ϊ0-5*/, int TxAntenna/*�������ߺ�*/, int RxAntenna/*�������ߺ�*/) const
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		size_t sumoffset = (m_goffset + m_offset[nbid][ueid] + PathIndex * 2) % (Samples * PATHNUMBER * 2);
		int angle = m_angle[nbid][ueid];
		//��Ϊ���д洢����DB����1000�Ժ��ֵ(��Ӹ�����Ҫ������ĳ�˥��
		double RealValue = (double)pow(10.0,(m_fading[angle][RxAntenna][TxAntenna][sumoffset])/1000.0/10);
		double ImagValue = (double)pow(10.0,(m_fading[angle][RxAntenna][TxAntenna][sumoffset + 1])/1000.0/10);
		complex<double> HSPASCMFadingValue((RealValue - FadingOper),(ImagValue - FadingOper));
		//return ;
		return HSPASCMFadingValue;
	}

	void HSPASCMFading::Update()
	{
		m_goffset += (PATHNUMBER * 2);
		m_goffset %= (Samples * PATHNUMBER * 2);
	}


	void HSPASCMFading::CalcIndividualOffset( size_t nb, size_t ue )
	{
		RandomManager& randman = RandomManager::GetRandomManager();
		//ÿ���û����ÿ����վ���λ����һ�����ƫ��
		m_offset[nb][ue] = PATHNUMBER * 2 * (nb * randman.GetUniformIntGen(0, 100)() + ue);
	}

}