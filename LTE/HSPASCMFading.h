#pragma once
#include <vector>
#include <complex>
#include <fstream>
#include "HSPAFading.h"
#include "RandomManager.h"
#include "commondef.h"
#include "System.h"
#include "POS.h"
#include "RunParameters.h"
#include "HSPANodeB.h"
#include "HSPAUE.h"


namespace LTESim 
{
	using namespace std;

	class HSPASCMFading :
		public HSPAFading
	{
	public:
		HSPASCMFading(void);
	public:
		~HSPASCMFading(void);
		const static int PATHNUMBER = 6;
		const int RxAntennaNum;
		const int TxAntennaNum;
		const int FreqNum;								//载波数目，需要从外界获得
		void			InitialSCMFading();				//初始化SCM衰落表,读取衰落值到平台
		complex<double>	GetFading(int nbid/*NodeB标示*/, int ueid/*UE标示*/, int PathIndex/*多径号*/, 
			int TxAntenna/*发送天线号*/, int RxAntenna/*接收天线号*/) const;
		void Update();
	private:
		void			CalcIndividualOffset(size_t nb, size_t ue);
		int				CalcAngleIndex( double angle ) const;
		//double			GetAngleUEandNodeB(shared_ptr<IHSPAUE>,shared_ptr<HSPANodeB> nodeb);	//计算给定UE和NodeB之间的角度

		const static int AngleResolution = 5;


		const int FadingOper;
		const int Samples; 

		short m_fading[72][2][2][600000];	//衰落表
		std::vector<size_t>  m_offset[19];  //每一个ue和nodeb有一个相对衰落表初始位置的间隔
		std::vector<int> m_angle[19];   //把每个UE相对于NodeB的角度值进行存储，以免以后每次都要算
		int      m_goffset;     //衰落表的全局偏置
	};

}