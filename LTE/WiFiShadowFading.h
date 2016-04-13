#pragma once
#include "WiFiFading.h"
#include "RandomManager.h"
#include "DB.h"
namespace LTESim
{
	class WiFiShadowFading : public WiFiFading
	{
	public:
		WiFiShadowFading();
		~WiFiShadowFading();
	public:
		void Update();
		double GetFading( int apid, int ueid, int TxAntenna, int RxAntenna, double distance )const;
	};
}