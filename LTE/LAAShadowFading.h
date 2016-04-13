#pragma once
#include "LAAFading.h"
#include "RandomManager.h"
#include "DB.h"
namespace LTESim
{
	class LAAShadowFading : public LAAFading
	{
	public:
		LAAShadowFading();
		~LAAShadowFading();
	public:
		void Update();
		double GetFading( int apid, int ueid, int TxAntenna, int RxAntenna, double distance )const;
	};
}