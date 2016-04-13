#pragma once 
#include "LAANodeB.h"
#include "LAAUE.h"
#include "RunParameters.h"
#include "Pathloss.h"
#include "LAAChannel.h"

namespace LTESim
{
	class LAAChannel;

	class LAASINRCalculator
	{
	public:
		static LAASINRCalculator * Instance();
	public:
		~LAASINRCalculator();
		//在这里会将该计算的该Channel的SINR信息存到该Channel中
		double CalculateSINROfChannel( shared_ptr<LAAChannel> pChannel ) const;
		double CalcUsefulPower( const LAANodeB& nb, const ILAAUE& ue, int TxAntennaIndex, int RevAntennaIndex ) const;
		double CalcUENoisePower( const LAANodeB& nb, const ILAAUE& ue ) const;
		
		//计算某根接收天线上承受的小区间干扰功率
		double CalcIoc(const LAANodeB& nb, const ILAAUE& ue, int TxAntennaIndex, int RevAntennaIndex) const;
        //计算某根接收天线上某个RB承受的小区内干扰功率，现在没有用到，直接返回的0
		double CalcIor() const;

		void OutputAllSINR( std::ostream& os ) const;
	protected:
		LAASINRCalculator();
	protected:
		static LAASINRCalculator * m_pInstance;
	protected:
		//map<double,double> m_mapSINR;

	};
}