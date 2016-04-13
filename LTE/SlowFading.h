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
		void InitialSlowFading (void );	//��˥��ʼ��
		void Update (void);	//������˥ֵ
		double GetFading(int nbid/*NodeB��ʾ*/, int ueid/*UE��ʾ*/, int RBIndex/*RB��*/, 
			int TxAntenna/*�������ߺ�*/, int RxAntenna/*�������ߺ�*/) const;	//�����˥��ֵ
	protected:
		double m_SlowFadingSigmaDB;	//��˥��׼��db
		vector< vector<double> > m_SlowFadingGroup;		//�洢SlowFadingValue��ʵֵ
	};
}