#include "commondef.h"
#include "WiFiShadowFading.h"
#include "RandomManager.h"
namespace LTESim
{
	WiFiShadowFading::WiFiShadowFading()
	{

	}

	WiFiShadowFading::~WiFiShadowFading()
	{

	}

	double WiFiShadowFading::GetFading( int apid, int ueid, int TxAntenna, int RxAntenna, double distance )const
	{	//参数是dB值，生成的值也是dB值
		double sigma = distance>10?5:3;
		NORMAL_DOUBLE_GEN gen = RandomManager::GetRandomManager().GetNormalGen(0.0,sigma);
		return dBToReal(gen());	
	}

	void WiFiShadowFading::Update()
	{

	}

}
