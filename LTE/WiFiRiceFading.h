#pragma once
#include "WiFiFading.h"
#include "RandomManager.h"
#include "DB.h"
namespace LTESim
{
	class WiFiRiceFading : public WiFiFading
	{
	public:
		WiFiRiceFading();
		~WiFiRiceFading();
	public:
		void Update();
		double GetFading( int nbid, int ueid, int TxAntenna, int RxAntenna, double distance )const;
	protected:
		double m_dK;
	};
}