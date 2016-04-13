#pragma once
#include "Fading.h"
#include "RandomManager.h"
#include "DB.h"
namespace LTESim
{
	class RayleighFading :public Fading
	{
	public:
		RayleighFading(void);
	public:
		~RayleighFading(void);
		void Update()
		{
		}
		//���˥��ֵ
		double GetFading(int nbid, int ueid, int RBIndex, int TxAntenna, int RxAntenna) const
		{
			//�õ���˹���������������ֵ0������9db����׼�����4.5db������������Ǳ�׼�
			static NORMAL_DOUBLE_GEN gen1 = RandomManager::GetRandomManager().GetNormalGen( 0.0, sqrt(dBToReal(9.0)) );	
			static NORMAL_DOUBLE_GEN gen2 = RandomManager::GetRandomManager().GetNormalGen( 0.0, sqrt(dBToReal(9.0)) );	
			double a = gen1();
			double b = gen2();
			return ( a*a + b*b );
		}
	};

}
