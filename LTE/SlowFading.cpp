#include"SlowFading.h"
#include "Configuration.h"
#include "RunParameters.h"
#include "DB.h"
namespace LTESim
{
	class System;
	SlowFading::SlowFading(  )
	{
		InitialSlowFading( );
	}

	SlowFading::~SlowFading( void )
	{

	}

	void SlowFading::InitialSlowFading(void )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_SlowFadingSigmaDB = pSystem-> GetConfig().GetDouble("Standard_Deviation_of_Slow_Fading");		//慢衰标准差


		//size_t nbnum = System::GetCurrentSystem()->GetNBList().size();
		size_t nbnum = pSystem->GetNBList().size() + pSystem->GetHSPANBList().size();
		size_t uenum = pSystem->GetUEList().size() + pSystem->GetHSPAUEList().size() + pSystem->GetDualModeLTEHSPAUEList().size() + pSystem->GetLTEWiFiUEList().size();

		m_SlowFadingGroup.resize(uenum);
		//生成初始慢衰表
		for (size_t ue = 0; ue < uenum; ue++)
		{
			m_SlowFadingGroup[ue].resize(nbnum);

			NORMAL_DOUBLE_GEN GenForNearFading = RandomManager::GetRandomManager().GetNormalGen(0.0, (double)m_SlowFadingSigmaDB);
			NORMAL_DOUBLE_GEN GenForFarFading = RandomManager::GetRandomManager().GetNormalGen(0.0, (double)m_SlowFadingSigmaDB);
			double Near = 0.0;	//一个UE一个时刻近场衰落只需要一个值
			do
			{
				Near = GenForNearFading();
			}while(fabs(Near) > 10.0);

			vector<double> Far(nbnum);	//一个UE一个时刻对应每个基站都有一个远场衰落值
			for (size_t nb = 0; nb< nbnum; nb++)
			{
				do 
				{
					Far[nb] = GenForFarFading();
				} while (fabs(Far[nb]) > 10.0);
				m_SlowFadingGroup[ue][nb] = dBToReal((Near+Far[nb])/sqrt(2.0));
			}
		}
	}

	double SlowFading::GetFading(int nbid, int ueid, int RBIndex, int TxAntenna, int RxAntenna)const
	{
		//return 1.0;
       return m_SlowFadingGroup[ueid][nbid];
	}

	void SlowFading::Update(void)
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		//size_t nbnum = pSystem->GetNBList().size();
		size_t nbnum = pSystem->GetNBList().size() + pSystem->GetHSPANBList().size();
		size_t uenum = pSystem->GetUEList().size() + pSystem->GetHSPAUEList().size() + pSystem->GetDualModeLTEHSPAUEList().size() + pSystem->GetLTEWiFiUEList().size();
		
		for (size_t ue = 0; ue < uenum; ue++)
		{
			NORMAL_DOUBLE_GEN GenForNearFading = RandomManager::GetRandomManager().GetNormalGen(0.0, (double)m_SlowFadingSigmaDB);
			NORMAL_DOUBLE_GEN GenForFarFading = RandomManager::GetRandomManager().GetNormalGen(0.0, (double)m_SlowFadingSigmaDB);
			double Near = 0.0;	//一个UE一个时刻近场衰落只需要一个值
			do
			{
				Near = GenForNearFading();
			}while(fabs(Near) > 5.0);

			vector<double> Far(nbnum);	//一个UE一个时刻对应每个基站都有一个远场衰落值
			for (size_t nb = 0; nb< nbnum; nb++)
			{
				do 
				{
					Far[nb] = GenForFarFading();
				} while (fabs(Far[nb]) > 5.0);
				m_SlowFadingGroup[ue][nb] = dBToReal((Near+Far[nb])/sqrt(2.0));
			}
		}
	}
}
