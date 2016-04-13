#include "commondef.h"
#include "LAARiceFading.h"
#include "RandomManager.h"
namespace LTESim
{
	LAARiceFading::LAARiceFading()
	{
		m_dK = 12.0;	//Channel D£¬20MHz
	}

	LAARiceFading::~LAARiceFading()
	{

	}

	double LAARiceFading::GetFading( int nbid, int ueid, int TxAntenna, int RxAntenna, double distance )const
	{
		int A = 1;
		double sigma = sqrt(A*A/m_dK/2);
		NORMAL_DOUBLE_GEN gen = RandomManager::GetRandomManager().GetNormalGen(0.0,sigma);	
		double A1 = gen();
		double A2 = gen();
		double A3 = sqrt( (A+A1) * (A+A1) + A2*A2 );//ÊµÖµ
		//return 10*log10(A3);//DBÖµ
		return A3;
	}

	void LAARiceFading::Update()
	{

	}

}
