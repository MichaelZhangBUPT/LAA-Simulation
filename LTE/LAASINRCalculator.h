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
		//������Ὣ�ü���ĸ�Channel��SINR��Ϣ�浽��Channel��
		double CalculateSINROfChannel( shared_ptr<LAAChannel> pChannel ) const;
		double CalcUsefulPower( const LAANodeB& nb, const ILAAUE& ue, int TxAntennaIndex, int RevAntennaIndex ) const;
		double CalcUENoisePower( const LAANodeB& nb, const ILAAUE& ue ) const;
		
		//����ĳ�����������ϳ��ܵ�С������Ź���
		double CalcIoc(const LAANodeB& nb, const ILAAUE& ue, int TxAntennaIndex, int RevAntennaIndex) const;
        //����ĳ������������ĳ��RB���ܵ�С���ڸ��Ź��ʣ�����û���õ���ֱ�ӷ��ص�0
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