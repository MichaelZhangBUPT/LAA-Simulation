#include "commondef.h"
#include <algorithm>
#include <complex>
#include <iostream>
#include <fstream>
#include <string>
#include "SCMFading.h"
#include "Configuration.h"
#include "RunParameters.h"
#include "System.h"
#include "Map.h"
#include "POS.h"
#include "UE.h"
#include "NodeB.h"
#include "RandomManager.h"


namespace LTESim
{   
	using namespace std;
	class ILTEUE;
	class NodeB;
	class System;
	class RunParameters;
	class Math;
	SCMFading::SCMFading():Samples(20000),RxAntennaNum(2),TxAntennaNum(2),m_goffset(0)
	{
		InitialSCMFading();
	}

	SCMFading::~SCMFading(void)
	{
	}

	void SCMFading::InitialSCMFading()
	{   
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_iRBNum = pSystem->GetConfig().GetInt("RB_Number");
		std::string FadingLocation = pSystem->GetConfig().GetString("FADING_TABLE_LOCATION");
		FadingLocation += "FadingResults.bin";
		ifstream FastFadingFile(FadingLocation.c_str(), ios::binary);	//�������ļ��ж�ȡ
		if(FastFadingFile.fail())
		{
			throw std::logic_error("The file cannot be opened.\n");
		}
		for (int angle = 0; angle < 360 / AngleResolution; ++angle)
		{
			for (int RxAn=0;RxAn<RxAntennaNum;++RxAn)
			{
				for (int TxAn=0;TxAn<TxAntennaNum;++TxAn)
				{   
					FastFadingFile.read((char*)m_fading[angle][RxAn][TxAn], sizeof(m_fading[angle][RxAn][TxAn]));
				}
			}
		}
		size_t nbnum = pSystem->GetNBList().size();
		size_t uenum = pSystem->GetUEList().size() + pSystem->GetHSPAUEList().size() + pSystem->GetDualModeLTEHSPAUEList().size() + pSystem->GetLTEWiFiUEList().size();
		for ( size_t nb = 0; nb < nbnum; nb++)
		{   
			m_offset[nb].resize(uenum);
			m_angle[nb].resize(uenum);
			for (size_t ue = 0; ue < uenum; ue++) 
			{
                CalcIndividualOffset(nb, ue);

				IUE::PTR RxUE = pSystem->GetBaseUE((int)ue);	      //��ý��ܶ�
				NodeB::PTR TxNodeB = pSystem->GetNodeB((int)nb);	//��÷��Ͷ�
				
				double angle= ::Angle(TxNodeB->GetPos(), RxUE->GetPos());	//��ýǶ�
            
                int iangle = CalcAngleIndex(angle);      
				m_angle[nb][ue]=iangle;
			}
		}
	}

	//�õ�Fadingֵ Ҫ�����߱����0~1��RB�����0~49
	double SCMFading::GetFading/*_db*/(int nbid, int ueid, int RBIndex, int TxAntenna, int RxAntenna)const
	{   
		size_t sumoffset = (m_goffset + m_offset[nbid][ueid] + RBIndex) % (Samples*m_iRBNum);
		int angle = m_angle[nbid][ueid];
		//��Ϊ���д洢����DB����1000�Ժ��ֵ(��Ӹ�����Ҫ������ĳ�˥��
		//return 1.0;
		return (double)pow(10.0,-m_fading[angle][RxAntenna][TxAntenna][sumoffset]/1000.0/10);
	}

	void SCMFading::Update()//����˥��ֵ�ڱ��е�λ��
	{  
		m_goffset += m_iRBNum;
        m_goffset %= (Samples*m_iRBNum);
	}

    int SCMFading::CalcAngleIndex( double angle ) const
    {
        while(angle < 0)
        {
            angle += 360;
        }
        assert(angle >= 0 && angle <= 360);
         //��������ȡ��
        return static_cast<int>( angle / AngleResolution + 0.5 ) % (360/AngleResolution);
    }

    void SCMFading::CalcIndividualOffset(size_t nb, size_t ue)
    {
        RandomManager& randman = RandomManager::GetRandomManager();
        m_offset[nb][ue] = m_iRBNum * (nb * randman.GetUniformIntGen(0, 100)() + ue); //ÿ���û����ÿ����վ���λ����һ�����ƫ��
    }
}
