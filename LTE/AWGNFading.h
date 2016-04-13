#pragma once
#include "Fading.h"

namespace LTESim
{
	class AWGNFading :
		public Fading
	{
	public:
		AWGNFading(void);
	public:
		~AWGNFading(void);
		void Update();
		double GetFading(int TxNode/*发送端标示*/, int RxNode/*接受端标示*/, int RBIndex/*RB号*/, 
			int TxAntenna/*发送天线号*/, int RxAntenna/*接收天线号*/ ) const;
	};


	inline void AWGNFading::Update()
	{
		return;
	}

	inline double AWGNFading::GetFading( int TxNode, int RxNode, int RBIndex, int TxAntenna, int RxAntenna ) const
	{
		return 1.0;
	}
}
