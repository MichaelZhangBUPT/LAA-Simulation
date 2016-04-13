#pragma once 
#include "WiFiAP.h"
#include "WiFiUE.h"
#include "RunParameters.h"
#include "Pathloss.h"
#include "WiFiChannel.h"
#include "LAANodeB.h"

namespace LTESim
{
	class WiFiChannel;

	class WiFiCCACalculator
	{
	public:
		static WiFiCCACalculator * Instance();
	public:
		~WiFiCCACalculator();
		bool CalculateCCA( shared_ptr<WiFiAP> pAP ) const;
		double CalculateRxPower( const WiFiAP& ap1, const WiFiAP& ap2, int TxAntennaIndex, int RevAntennaIndex ) const;
		double CalculateRxPower( const LAANodeB& nb,const WiFiAP& ap, int TxAntennaIndex, int RevAntennaIndex ) const;
	protected:
		WiFiCCACalculator();
	protected:
		static WiFiCCACalculator * m_pInstance;
	};
}