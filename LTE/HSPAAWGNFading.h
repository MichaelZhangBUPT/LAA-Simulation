#pragma once

#include "commondef.h"
#include "HSPAFading.h"
#include <complex>

namespace LTESim
{

	class HSPAAWGNFading
		: public HSPAFading
	{
	public:
		HSPAAWGNFading(void);
	public:
		virtual ~HSPAAWGNFading(void);
		///返回为实值，表示衰落的幅度（越大表示信道越差）
		virtual std::complex<double> GetFading( int nbid, int ueid, int pathIndex, int TxAntenna, int RxAntenna) const;
		virtual void Update();
	};

}