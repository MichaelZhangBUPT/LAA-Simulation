#include "commondef.h"
#include "RandomManager.h"
#include "TDmaxC2IDownlinkScheduler.h"
#include "NodeB.h"
#include "ClockManager.h"
#include "Service.h"
#include "UE.h"
namespace LTESim
{


	TDmaxC2IDownlinkScheduler::TDmaxC2IDownlinkScheduler( shared_ptr<NodeB>& pNodeB )
		:TDDownlinkScheduler(pNodeB)
	{

	}
	TDmaxC2IDownlinkScheduler::~TDmaxC2IDownlinkScheduler(void)
	{

	}
	void TDmaxC2IDownlinkScheduler::CalculateServicePriority( SERVICELIST& CandidateServices )
	{
		TDmaxC2IDownlinkScheduler::SERVICELIST::iterator iter=CandidateServices.begin();
		for ( ; iter != CandidateServices.end(); ++iter)
		{
			shared_ptr<IUE> pIUE = (*iter)->GetUE();
			shared_ptr<ILTEUE> ue = dynamic_pointer_cast<ILTEUE>(pIUE);
			if ( ue == 0 )
				throw std::runtime_error( "不匹配的UE类型" );
			double sinrsum = 0.0;
			for ( int RBindex = 0 ; RBindex < m_iRBNum ; RBindex++)
			{
				sinrsum += ue->GetReporter()->GetSINR(RBindex);//获取RB上的SINR
			}
			double sinr = sinrsum / m_iRBNum;//该slot上的平均SINR
			(*iter)->SetQoS(sinr);

		}
	}	
}