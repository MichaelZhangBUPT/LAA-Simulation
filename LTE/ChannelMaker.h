#pragma once
#include <set>
#include <list>
#include "Matrix.h"

namespace LTESim
{
	class MCS;
	class ILTEUE;
	class IHSPAUE;
	class HSPANodeB;
	class Channel;
	class HSPAChannel;
	class PhyResource;
	class HSPAPhyResource;
	class IWiFiUE;
	class ILAAUE;
	class WiFiChannel;
	class LAAChannel;


	class ChannelMaker
	{
	public:
		typedef std::set < shared_ptr<PhyResource> > PhyResourceList;	//资源列表
	public:
		ChannelMaker(void);
		~ChannelMaker(void);
		shared_ptr<Channel>				CreateDLSCH(shared_ptr<ILTEUE> pUE, const MCS& mcstype,double TXpower, int FrameIndex, int slotBegingingIndex, int slotEndingIndex, int RBBegingingIndex, int RBendingIndex);
		shared_ptr<Channel>				CreateDLSCH(shared_ptr<ILTEUE> pUE, const MCS& mcstype,double TXpower, int FrameIndex/*0~N-1*/, int SubFrameIndex/*0~9*/, const std::list<int>& RBS/*0~N-1*/);
		shared_ptr<HSPAChannel>		CreateHSPADLSCH(shared_ptr<IHSPAUE> pUE, const MCS& mcs, double TxPower, int iFrameIndex, int iSubFrameIndex, const std::vector<int>& refStreamVec, const std::vector<int>& refCodeVec, const Matrix& refDLWeightMatrix );
		shared_ptr<WiFiChannel>		CreateWiFiDLSCH(shared_ptr<IWiFiUE> pUE, const MCS& mcs, double TxPower );
		shared_ptr<LAAChannel>		CreateLAADLSCH(shared_ptr<ILAAUE> pUE, const MCS& mcs, double TxPower );
	};
}