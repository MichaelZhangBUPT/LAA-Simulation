#pragma once 
#include "LAANodeB.h"
#include "WiFiAP.h"
#include "LAAUE.h"
#include "RunParameters.h"
#include "Pathloss.h"
#include "LAAChannel.h"

namespace LTESim
{

	class LAANodeB;
	class WiFiAP;

	class LAACCACalculator
	{
	public:
		static LAACCACalculator * Instance();
	public:
		~LAACCACalculator();
		//������Ὣ�ü���ĸ�eNB��CCA��Ϣ�浽��eNB��
		bool CalculateCCA( shared_ptr<LAANodeB> pLAANodeB ) const;
		//bool CalculateCCA( shared_ptr<WiFiAP> pWiFiAP ) const;
		double CalculateRxPower( const LAANodeB& nb1, const LAANodeB& nb2, int TxAntennaIndex, int RevAntennaIndex ) const;
		double CalculateRxPower( const LAANodeB& nb,const WiFiAP& ap, int TxAntennaIndex, int RevAntennaIndex ) const;
		//double CalculateRxPower( const WiFiAP& ap1,const WiFiAP& ap2, int TxAntennaIndex, int RevAntennaIndex ) const;
	protected:
		LAACCACalculator();
	protected:
		static LAACCACalculator * m_pInstance;
	
	};
}


	