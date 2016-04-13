#pragma once
#include "commondef.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include "RandomManager.h"
#include "Fading.h"
#include "System.h"
namespace LTESim
{

	class SlowFading : public Fading
	{
	public:
		SlowFading ();
	public:
		~SlowFading (void);
		void InitialSlowFading (void );	//慢衰初始化
		void Update (void);	//更新慢衰值
		double GetFading(int nbid/*NodeB标示*/, int ueid/*UE标示*/, int RBIndex/*RB号*/, 
			int TxAntenna/*发送天线号*/, int RxAntenna/*接收天线号*/) const;	//获得慢衰数值
	protected:
		double m_SlowFadingSigmaDB;	//慢衰标准差db
		vector< vector<double> > m_SlowFadingGroup;		//存储SlowFadingValue，实值
	};
}