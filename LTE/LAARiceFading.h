#pragma once
#include "LAAFading.h"
#include "RandomManager.h"
#include "DB.h"
namespace LTESim
{
	class LAARiceFading : public LAAFading
	{
	public:
		LAARiceFading();
		~LAARiceFading();
	public:
		void Update();
		double GetFading( int nbid, int ueid, int TxAntenna, int RxAntenna, double distance )const;
	protected:
		double m_dK;
	};
}