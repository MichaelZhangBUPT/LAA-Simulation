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

        double GetFading(int nbid/*NodeB��ʾ*/, int ueid/*UE��ʾ*/, int RBIndex/*RB��*/, 
			int TxAntenna/*�������ߺ�*/, int RxAntenna/*�������ߺ�*/)const;
    private:
        void CalcIndividualOffset(size_t nb, size_t ue);

        int CalcAngleIndex( double angle ) const;

	protected:
        const static int AngleResolution = 5;
		const int Samples; 
		const int RxAntennaNum;
		const int TxAntennaNum;
		short m_fading[72][2][2][1000000];	//˥���
		std::vector<size_t>  m_offset[7];  //ÿһ��ue��nodeb��һ�����˥����ʼλ�õļ��
		std::vector<int> m_angle[7];   //��ÿ��UE�����NodeB�ĽǶ�ֵ���д洢�������Ժ�ÿ�ζ�Ҫ��
	    int      m_goffset;     //˥����ȫ��ƫ��
		int		 m_iRBNum;
	};
}


