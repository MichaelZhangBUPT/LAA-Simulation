#pragma once

#include "commondef.h"
#include "HSPAFading.h"
#include <complex>

namespace LTESim
{

	class HSPAAWGNFading
		: public HSPAFading
	{
	public:
		HSPAAWGNFading(void);
	public:
		virtual ~HSPAAWGNFading(void);
		///����Ϊʵֵ����ʾ˥��ķ��ȣ�Խ���ʾ�ŵ�Խ�
		virtual std::complex<double> GetFading( int nbid, int ueid, int pathIndex, int TxAntenna, int RxAntenna) const;
		virtual void Update();
	};

}