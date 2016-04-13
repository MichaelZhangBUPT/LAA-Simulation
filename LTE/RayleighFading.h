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
		//获得衰落值
		double GetFading(int nbid, int ueid, int RBIndex, int TxAntenna, int RxAntenna) const
		{
			//得到高斯随机数生成器，均值0，方差9db（标准差就是4.5db）（这个参数是标准差）
			static NORMAL_DOUBLE_GEN gen1 = RandomManager::GetRandomManager().GetNormalGen( 0.0, sqrt(dBToReal(9.0)) );	
			static NORMAL_DOUBLE_GEN gen2 = RandomManager::GetRandomManager().GetNormalGen( 0.0, sqrt(dBToReal(9.0)) );	
			double a = gen1();
			double b = gen2();
			return ( a*a + b*b );
		}
	};

}
