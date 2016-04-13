#include "HSPAAWGNFading.h"
#include <complex>

namespace LTESim
{

	HSPAAWGNFading::HSPAAWGNFading()
		: HSPAFading()
	{
		
	}

	HSPAAWGNFading::~HSPAAWGNFading()
	{

	}

	std::complex<double> HSPAAWGNFading::GetFading( int nbid, int ueid, int pathIndex, int TxAntenna, int RxAntenna) const
	{
		return std::complex<double>(1.0, 0);
	}

	void HSPAAWGNFading::Update()
	{

	}

}