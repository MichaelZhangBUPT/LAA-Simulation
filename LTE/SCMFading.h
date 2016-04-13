#pragma once
#include <vector>
#include "Fading.h"
#include "RandomManager.h"
namespace LTESim
{
	class SCMFading :
		public Fading
	{
	public:
		SCMFading();
	public:
		~SCMFading(void);
		void  Update();
		void InitialSCMFading();

        double GetFading(int nbid/*NodeB标示*/, int ueid/*UE标示*/, int RBIndex/*RB号*/, 
			int TxAntenna/*发送天线号*/, int RxAntenna/*接收天线号*/)const;
    private:
        void CalcIndividualOffset(size_t nb, size_t ue);

        int CalcAngleIndex( double angle ) const;

	protected:
        const static int AngleResolution = 5;
		const int Samples; 
		const int RxAntennaNum;
		const int TxAntennaNum;
		short m_fading[72][2][2][1000000];	//衰落表
		std::vector<size_t>  m_offset[7];  //每一个ue和nodeb有一个相对衰落表初始位置的间隔
		std::vector<int> m_angle[7];   //把每个UE相对于NodeB的角度值进行存储，以免以后每次都要算
	    int      m_goffset;     //衰落表的全局偏置
		int		 m_iRBNum;
	};
}


