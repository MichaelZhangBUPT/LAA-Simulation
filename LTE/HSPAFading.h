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
	//���ɿ���˥����
	class HSPAFading
	{
	public:
		HSPAFading(void);
	public:
		virtual ~HSPAFading(void);
		///����Ϊʵֵ����ʾ˥��ķ��ȣ�Խ���ʾ�ŵ�Խ�
		virtual complex<double> GetFading( int nbid, int ueid, int pathIndex, int TxAntenna, int RxAntenna) const = 0;
		virtual void Update() = 0; 
	};



}