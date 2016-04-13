#include "commondef.h"
#include "FDPFDownlinkScheduler.h"
#include "NodeB.h"
#include "ClockManager.h"
#include "Service.h"
#include "UE.h"
namespace LTESim
{


FDPFDownlinkScheduler::FDPFDownlinkScheduler( shared_ptr<NodeB>& pNodeB )
        :FDDownlinkScheduler(pNodeB)
{
 
}
FDPFDownlinkScheduler::~FDPFDownlinkScheduler(void)
{

}
void FDPFDownlinkScheduler::CalculateServicePriority( shared_ptr<Service> pService,int RBindex )
{
	shared_ptr<IUE> pIUE = pService->GetUE();
	shared_ptr<ILTEUE> ue = dynamic_pointer_cast<ILTEUE>(pIUE);
	if ( ue == 0 )
		throw std::runtime_error( "��ƥ���UE����" );
	double sinr = ue->GetReporter()->GetSINR(RBindex);
	ServicePriority servicePriority;
	servicePriority.pService = pService;
	servicePriority.priority = sinr/pService->GetAverageThroughput();
	servicePriority.RBindex = RBindex;
	m_servicePriorityList.push_back(servicePriority);
	//֡��Ϊ0����һ���������ڸ���һ��
	//if ( frame==0 && RBindex==0)
	//{   
	//	FDPFDownlinkScheduler::SERVICELIST::iterator iter=CandidateServices.begin();
	//	for ( ;iter != CandidateServices.end(); ++iter)
	//	{
	//		AverageThr.insert(std::make_pair(*iter,5));//��ʼ��ƽ��������Ϊһ��ֵ
	//		shared_ptr<UE> ue =(*iter)->GetUE();
	//	}
	//}
	//else
	//{   //һ����������ƽ���������Ÿ���һ��
	//	if ( RBindex==0 )
	//	{
	//		FDPFDownlinkScheduler::SERVICELIST::iterator iter=CandidateServices.begin();
	//		for ( ; iter != CandidateServices.end(); ++iter)
	//		{
	//			//DATASIZE thr=(*iter)->GetThroughput();
	//			if ( (*iter)->GetIsScheduled()==true)
	//			{
	//				AverageThr[*iter]=(1-1/Tc)*AverageThr[*iter]+(1/Tc)*(*iter)->GetLastPacketsize();
	//			} 
	//			else
	//			{
	//				AverageThr[*iter]=(1-1/Tc)*AverageThr[*iter];
	//			}
	//		}
	//	}
	//}
	//FDPFDownlinkScheduler::SERVICELIST::iterator iter=CandidateServices.begin();
	//for ( ; iter != CandidateServices.end(); ++iter)
	//{		
	//		shared_ptr<UE> ue =(*iter)->GetUE();
	//		double sinr = ue->GetReporter().GetSINR(RBindex);//��ȡ��RB�ϵ�SINR
	//		double AverageThr = (*iter)->GetAverageThroughput();
	//		(*iter)->SetQoS(sinr/AverageThr);
	//}
	//PFDownlinkScheduler::SERVICELIST::iterator iter=CandidateServices.begin();
	//for ( ; iter != CandidateServices.end(); ++iter)
	//{
	//	double thr = (*iter)->GetThroughput();
	//	if ( thr<0.01)
	//	{
 //         thr = 0.1;
	//	  shared_ptr<UE> ue =(*iter)->GetUE();
	//	  double sinr = ue->GetReporter().GetSINR(RBindex);//��ȡ��RB�ϵ�SINR
	//	  (*iter)->SetQoS(sinr/pow(thr,alpha));
	//	}
	//	else
	//	{
	//		shared_ptr<UE> ue =(*iter)->GetUE();
	//		double sinr = ue->GetReporter().GetSINR(RBindex);//��ȡ��RB�ϵ�SINR
	//		(*iter)->SetQoS(sinr/pow(thr,alpha));

	//	}
	//	
	//}
}
}

