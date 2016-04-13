#pragma once
#include "Antenna.h"

namespace LTESim
{
    ///纯分集天线阵列
	class DiversityAntenna :
		public LTESim::Antenna
	{
	public:
        ///构造函数
        /*
        * @param antennaGain, 天线增益物理
        */
		DiversityAntenna(double antennaGain);
	public:
        ///析构函数
		~DiversityAntenna(void);

        ///天线SINR分集合并
        /*
        * 对于参数的描述参考Antenna基类，合并算法使用的是MRC直接相加
        */
		virtual void CombineSINR(const std::vector< std::vector<double> >& SINRPerRBPerTxAntenna, 
			                     std::vector<double>& SINRPerRB) const;
        ///得到天线增益
		virtual double GetAntennaGain() const;
    protected:
        double m_antennaGain;
	};


}