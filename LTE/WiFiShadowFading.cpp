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
	{	//������dBֵ�����ɵ�ֵҲ��dBֵ
		double sigma = distance>10?5:3;
		NORMAL_DOUBLE_GEN gen = RandomManager::GetRandomManager().GetNormalGen(0.0,sigma);
		return dBToReal(gen());	
	}

	void WiFiShadowFading::Update()
	{

	}

}
