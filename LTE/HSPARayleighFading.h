#pragma once

#include "commondef.h"
#include "HSPAFading.h"
#include "RandomManager.h"
#include "DB.h"
#include <complex>
namespace LTESim
{
	class HSPARayleighFading :public HSPAFading
	{
	public:
		HSPARayleighFading(void);
	public:
		~HSPARayleighFading(void);
		void Update()
		{
		}
		//获得衰落值
		std::complex<double> GetFading(int nbid, int ueid, int pathIndex, int TxAntenna, int RxAntenna) const
		{
			//得到高斯随机数生成器，均值0，方差9db
			static NORMAL_DOUBLE_GEN gen1 = RandomManager::GetRandomManager().GetNormalGen(0,3);	
			static NORMAL_DOUBLE_GEN gen2 = RandomManager::GetRandomManager().GetNormalGen(0,3);	
			double a = gen1();
			double b = gen2();
			double c = std::sqrt( a*a + b*b );
			double fading = LTESim::dBToReal( c );
			return std::complex<double>(fading, 0);
		}
	};

}
