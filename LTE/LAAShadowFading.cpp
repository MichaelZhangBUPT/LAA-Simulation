#include "commondef.h"
#include "LAAShadowFading.h"
#include "RandomManager.h"
namespace LTESim
{
	LAAShadowFading::LAAShadowFading()
	{

	}

	LAAShadowFading::~LAAShadowFading()
	{

	}

	double LAAShadowFading::GetFading( int apid, int ueid, int TxAntenna, int RxAntenna, double distance )const
	{	//参数是dB值，生成的值也是dB值
		double sigma = 3;
		NORMAL_DOUBLE_GEN gen = RandomManager::GetRandomManager().GetNormalGen(0.0,sigma);
		return dBToReal(gen());	
	}

	void LAAShadowFading::Update()
	{

	}

}
