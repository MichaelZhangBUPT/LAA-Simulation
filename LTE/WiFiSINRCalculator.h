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

	class WiFiSINRCalculator
	{
	public:
		static WiFiSINRCalculator * Instance();
	public:
		~WiFiSINRCalculator();
		//在这里会将该计算的该Channel的SINR信息存到该Channel中
		double CalculateSINROfChannel( shared_ptr<WiFiChannel> pChannel ) const;
		double CalcUsefulPower( const WiFiAP& ap, const IWiFiUE& ue, int TxAntennaIndex, int RevAntennaIndex ) const;
		double CalcUENoisePower( const WiFiAP& ap, const IWiFiUE& ue ) const;
		double CalcIoc( const WiFiAP& ap, const IWiFiUE& ue, int TxAntennaIndex, int RevAntennaIndex ) const;
		double CalcIor()const;
		void OutputAllSINR( std::ostream& os ) const;
	protected:
		WiFiSINRCalculator();
	protected:
		static WiFiSINRCalculator * m_pInstance;
	protected:
		//map<double,double> m_mapSINR;

	};
}