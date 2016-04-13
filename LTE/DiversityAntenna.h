#pragma once
#include "Antenna.h"

namespace LTESim
{
    ///���ּ���������
	class DiversityAntenna :
		public LTESim::Antenna
	{
	public:
        ///���캯��
        /*
        * @param antennaGain, ������������
        */
		DiversityAntenna(double antennaGain);
	public:
        ///��������
		~DiversityAntenna(void);

        ///����SINR�ּ��ϲ�
        /*
        * ���ڲ����������ο�Antenna���࣬�ϲ��㷨ʹ�õ���MRCֱ�����
        */
		virtual void CombineSINR(const std::vector< std::vector<double> >& SINRPerRBPerTxAntenna, 
			                     std::vector<double>& SINRPerRB) const;
        ///�õ���������
		virtual double GetAntennaGain() const;
    protected:
        double m_antennaGain;
	};


}