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
		const int FreqNum;								//�ز���Ŀ����Ҫ�������
		void			InitialSCMFading();				//��ʼ��SCM˥���,��ȡ˥��ֵ��ƽ̨
		complex<double>	GetFading(int nbid/*NodeB��ʾ*/, int ueid/*UE��ʾ*/, int PathIndex/*�ྶ��*/, 
			int TxAntenna/*�������ߺ�*/, int RxAntenna/*�������ߺ�*/) const;
		void Update();
	private:
		void			CalcIndividualOffset(size_t nb, size_t ue);
		int				CalcAngleIndex( double angle ) const;
		//double			GetAngleUEandNodeB(shared_ptr<IHSPAUE>,shared_ptr<HSPANodeB> nodeb);	//�������UE��NodeB֮��ĽǶ�

		const static int AngleResolution = 5;


		const int FadingOper;
		const int Samples; 

		short m_fading[72][2][2][600000];	//˥���
		std::vector<size_t>  m_offset[19];  //ÿһ��ue��nodeb��һ�����˥����ʼλ�õļ��
		std::vector<int> m_angle[19];   //��ÿ��UE�����NodeB�ĽǶ�ֵ���д洢�������Ժ�ÿ�ζ�Ҫ��
		int      m_goffset;     //˥����ȫ��ƫ��
	};

}