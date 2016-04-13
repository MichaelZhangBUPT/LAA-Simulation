#include "commondef.h"
#include "ChannelMaker.h"
#include "Channel.h"
#include "UE.h"
#include "NodeB.h"
#include "Service.h"
#include "HARQ.h"
#include "PhyResource.h"
#include "HSPAPhyResource.h"
#include "mcs.h"
#include "HSPAChannel.h"
#include "HSPANodeB.h"
#include "HSPAUE.h"
#include "Matrix.h"
#include "WiFiChannel.h"
#include "WiFiAP.h"
#include "WiFiUE.h"
#include "LAAChannel.h"
#include "LAANodeB.h"
#include "LAAUE.h"

namespace LTESim
{
	ChannelMaker::ChannelMaker(void)
	{
	}

	ChannelMaker::~ChannelMaker(void)
	{
	}

	shared_ptr<Channel> ChannelMaker::CreateDLSCH( shared_ptr<ILTEUE> pUE, const MCS& mcstype,double TXpower/*发送功率*/,
		int FrameIndxe/*帧编号*/, int slotBegingingIndex/*起始时隙编号*/, int slotEndingIndex/*结束时隙编号*/,
		int RBBegingingIndex/*起始RB编号*/, int RBendingIndex/*结束RB编号*/ )
	{
		shared_ptr<Channel> pChannel (new Channel);
		shared_ptr<NodeB> pNodeb = pUE->GetServingNodeB();
		//注意在调用改函数保证HARQ的service更新过
		shared_ptr<Service> pService = pUE->GetService( SYSTEM_LTE );
		PhyResourceList phyresourcelist;
		int AntennaNum = pNodeb->GetTxAntenna()->GetAntennaPortNum();

		for(int i = slotBegingingIndex; i < slotEndingIndex+1; ++i)
		{
			for (int k = RBBegingingIndex; k < RBendingIndex+1; ++k)			
			{
				for (int j = 0; j < AntennaNum; ++j)
				{
					shared_ptr<PhyResource> pres(new PhyResource(FrameIndxe, i, j, k)); 
					pres->SetTxPower(TXpower);	//设定发送功率
					phyresourcelist.insert(pres);
				}
			}
		}
		pChannel->Initilization(pNodeb,pUE, pService,pUE->GetCurrentDLHARQ( SYSTEM_LTE ),phyresourcelist, mcstype);
		return pChannel;
	}

	shared_ptr<Channel> ChannelMaker::CreateDLSCH( shared_ptr<ILTEUE> pUE, const MCS& mcstype,double TXpower, int FrameIndex, int SubFrameIndex, const std::list<int>& RBS )
	{
		shared_ptr<NodeB> pNodeb = pUE->GetServingNodeB();		
		int AntennaNum = pNodeb->GetTxAntenna()->GetAntennaPortNum();
		PhyResourceList phyresourcelist;
		int slotBegingingIndex = SubFrameIndex * 2;
		int slotEndingIndex = slotBegingingIndex + 1;
		for(int i = slotBegingingIndex; i < slotEndingIndex+1; ++i)
		{
			for (std::list<int>::const_iterator k = RBS.begin(); k != RBS.end(); ++k)			
			{
				for (int j = 0; j < AntennaNum; ++j)
				{
					shared_ptr<PhyResource> pres(new PhyResource(FrameIndex, i, j, *k)); 
					pres->SetTxPower(TXpower);
					phyresourcelist.insert(pres);
				}
			}
		}
		shared_ptr<Channel> pChannel (new Channel);
		shared_ptr<Service> pService = pUE->GetService( SYSTEM_LTE );   //注意在调用改函数保证HARQ的service更新过
		pChannel->Initilization(pNodeb,pUE, pService, pUE->GetCurrentDLHARQ( SYSTEM_LTE ),phyresourcelist, mcstype);
		return pChannel;
	}

	shared_ptr<HSPAChannel> ChannelMaker::CreateHSPADLSCH(shared_ptr<IHSPAUE> pUE, const MCS& mcs, double TxPower, int iFrameIndex, int iSubFrameIndex, const std::vector<int>& refStreamVec, const std::vector<int>& refCodeVec, const Matrix& refDLWeightMatrix )
	{
		if(HSPANodeB::PTR pNodeB = pUE->GetServingHSPANodeB())
		{
			//发送该HSPA子帧的三个时隙对应的Channel
			int iBeginSlotIndex = iSubFrameIndex*3;
			int iEndSlotIndex = iSubFrameIndex*3+2;

			HSPAChannel::PTR pChannel( new HSPAChannel() );
			//准备HSPAPhyResource的列表
			std::vector<HSPAPhyResource> phyResourceVec;
			for ( std::vector<int>::const_iterator streamVecIter = refStreamVec.begin(); streamVecIter != refStreamVec.end(); ++streamVecIter )
			{
				for ( std::vector<int>::const_iterator codeVecIter = refCodeVec.begin(); codeVecIter != refCodeVec.end(); ++codeVecIter )
				{
					for ( int iSlotIndex = iBeginSlotIndex; iSlotIndex <= iEndSlotIndex; ++iSlotIndex )
					{
						//在这里只考虑1个载波的情况，所以第一个参数传入0，表示0号Carrier
						phyResourceVec.push_back( HSPAPhyResource( 0, *codeVecIter, *streamVecIter, iFrameIndex, iSlotIndex ) );
					}
				}
			}

			pChannel->Initialization( pNodeB, pUE, DOWNLINK, pUE->GetCurrentDLHARQ( SYSTEM_HSPA ), TxPower, mcs, phyResourceVec, refDLWeightMatrix );

			return pChannel;
		}
		else
			throw std::runtime_error( "HSPAUE Get Serving NodeB Failed" );
	}

	shared_ptr<WiFiChannel> ChannelMaker::CreateWiFiDLSCH(shared_ptr<IWiFiUE> pUE, const MCS& mcs, double TxPower )
	{
		if ( WiFiAP::PTR pAP = pUE->GetServingAP() )
		{
			WiFiChannel::PTR pChannel( new WiFiChannel() );
			pChannel->Initialization( pAP, pUE, pUE->GetARQ(), TxPower, mcs );
			return pChannel;
		}
		else
			throw std::runtime_error( "WiFi UE Get Serving AP Failed" );
	}

	shared_ptr<LAAChannel> ChannelMaker::CreateLAADLSCH(shared_ptr<ILAAUE> pUE, const MCS& mcs, double TxPower )
	{
		if ( LAANodeB::PTR pNB = pUE->GetServingNB() )
		{
			LAAChannel::PTR pChannel( new LAAChannel() );
			pChannel->Initialization( pNB, pUE, pUE->GetARQ(), TxPower, mcs );
			return pChannel;
		}
		else
			throw std::runtime_error( "LAA UE Get Serving eNB Failed" );
	}
}

