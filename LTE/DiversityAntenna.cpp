#include "DiversityAntenna.h"
#include <vector>
#include <numeric>

namespace LTESim
{

	DiversityAntenna::DiversityAntenna(double antennaGain)
    : m_antennaGain(antennaGain)
	{

	}

	DiversityAntenna::~DiversityAntenna(void)
	{

	}

	void DiversityAntenna::CombineSINR( const std::vector< std::vector<double> >& SINRPerRBPerTxAntenna,  
                                        std::vector<double>& SINRPerRB ) const
	{
		/*在分集的情况下，将相同RB index的不同发射天线上的SINR合并MIMO发送分集*/
		for(unsigned i = 0; i < SINRPerRBPerTxAntenna.size(); ++i)
		{
			double sinr = std::accumulate( SINRPerRBPerTxAntenna[i].begin(), SINRPerRBPerTxAntenna[i].end(), 0.0);
			SINRPerRB.push_back(sinr);
		}
	}

	double DiversityAntenna::GetAntennaGain() const
	{
		return m_antennaGain;
	}

}