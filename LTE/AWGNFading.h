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
		double GetFading(int TxNode/*���Ͷ˱�ʾ*/, int RxNode/*���ܶ˱�ʾ*/, int RBIndex/*RB��*/, 
			int TxAntenna/*�������ߺ�*/, int RxAntenna/*�������ߺ�*/ ) const;
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
