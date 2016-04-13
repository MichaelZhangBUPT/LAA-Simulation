#pragma once
#include <vector>
#include <complex>
#include "Fading.h"
#include "RandomManager.h"
#include "commondef.h"
#include "System.h"

namespace LTESim
{
	using namespace std;
	//生成快慢衰的类
	class HSPAFading
	{
	public:
		HSPAFading(void);
	public:
		virtual ~HSPAFading(void);
		///返回为实值，表示衰落的幅度（越大表示信道越差）
		virtual complex<double> GetFading( int nbid, int ueid, int pathIndex, int TxAntenna, int RxAntenna) const = 0;
		virtual void Update() = 0; 
	};



}