#pragma once
#include"UE.h"
#include "NodeB.h"
#include "PhyResource.h"
#include "Service.h"
#include <set>
#include <vector>

namespace LTESim
{
	class Channel														//传输信道
	{
	public:
		//物理信道资源，RB资源(包含天线，帧号 天线端口 时隙号 RB号, 所占的发射功率)
		typedef std::set< shared_ptr<PhyResource> > PhyResourceList;	
        
        ///构造函数
		Channel(void);
	public:
        ///析构函数
		virtual ~Channel(void);
		
		///信道初始化
        /**
        * 初始化信道
        * @param nodeb, 信道的nodeb端
        * @param ue，信道的ue端
        * @param pService, 信道携带的业务
        * @param pHARQ, 信道携带的HARQ进程
        * @param phyResourceList, 所使用的物理资源，一维列表，
        * 大小为天线端口数*使用的RB数*使用的时隙数
        * 按照天线端口、RB、时隙的次序增加。
        * @param mcstype，使用的MCS
        */
		void Initilization(shared_ptr<NodeB> nodeb, shared_ptr<ILTEUE> ue, shared_ptr<Service> pService, 
			shared_ptr<HARQ> pHARQ, const PhyResourceList& phyResourceList,const MCS& mcstype);
		
        ///取得MCS列表
		const MCS& GetMCS() const;
		
		//得到占用的RB总数
        /**
        * 现在所使用的天线都没有复用，如果需要加天线复用的话函数内部需要修改
        * @return 时隙数目*RB数目*天线复用阶数
        */
		int GetRBNum() const;
		
        //得到某个时隙上的RB总数
        /**
        * 现在所使用的天线都没有复用，如果需要加天线复用的话函数内部需要修改
        * 该函数假定每个时隙分配的RB数一致，如果不一致需要修改函数实现
        * @param slot, 0~19的时隙号
        * @return 当前时隙RB数目*天线复用阶数
        */
        int GetRBNum(int slot) const;

		///得到信道所属于的UE
		shared_ptr<ILTEUE>		GetActiveUE() const;                             

        ///得到信道所属于的NodeB
		shared_ptr<NodeB>			GetActiveNodeB() const; 

        ///得到携带的Service
		shared_ptr<Service>			GetActiveService() ;

        ///得到携带的HARQ
        shared_ptr<HARQ>			GetHARQ();							

		///取得信道的最后一个时隙，因为channel不会跨帧
        /**
        * @return 0~SLOTPERFRAME-1的slot index
        */
		int GetLastSlot(void) const;						        

        ///取得第一个时隙，因为channel不会跨帧
        /**
        * @return 0~SLOTPERFRAME-1的slot index
        */
		int GetFirstSlot(void) const;	

        ///channel所占用的时隙数
		std::size_t GetSlotNum(void) const;	

        ///取得使用的物理资源
        /**
        * @return PhyResourceList对象，所使用的物理资源，一维列表，
        * 大小为天线端口数*使用的RB数*使用的时隙数
        * 按照天线端口、RB、时隙的次序增加。
        */
		const PhyResourceList &GetPhyResourceList()const;

        ///取得占用的时隙
        /**
        * @return set<int>, 一个带有各个占用时隙index的表
        */
		const std::set<int>&			GetSlotList() const;
  
        ///判断时隙是否被使用
        /**
        * @param slot, 0~SLOTPERFRAME-1的index
        * @return true代表被使用
        */
		bool							UsedSlot(int slot) const;			

        ///更新信道的SINR
        /*
        * 在多个时隙的情况下，需要信道将各个时隙的SINR连成一列保存下来最后判决
        * @param SINRList, 当前时隙的经过天线合并后的SINR列表，将用作EESM
        * @param FrameNo, 当前帧号， 0起始到终止之间的一个frame 号，保留未用（当前不可能跨帧)
        * @param SlotNo, 0~SLOTPERFRAME-1，当前时隙号
        */
		void							UpdateSINRListofSlot(vector<double>& SINRlist, int FrameNo, int SlotNo);			//更新该信道每个RB每个时隙的SINR值

        ///得到SINR
        /**
        * @return 所有时隙的SINR连成一列的表
        */
		vector<double>&		GetSINRList();


	
	protected:
		MCS										m_MCSType;								//该信道MCS等级
		weak_ptr<ILTEUE>				m_pActiveUE;								//该信道属于的用户
		weak_ptr<NodeB>				m_pAcitiveNodeB;						//该信道所属基站 

		//物理信道资源，RB资源(包含天线，RB号 时隙号 帧号 天线端口)
		PhyResourceList					m_PhyResourceList;
		//物理信道资源，first:发射功率，second:RB资源(包含天线，RB号 时隙号 子帧号 帧号 天线端口)
		std::set<int>							m_vSlotList;									//该信道占用时隙
		weak_ptr<Service>				m_Service;									//该信道对应的业务
		vector <double>					m_vSINRListOfRBAndSlot;			//每个RB在改信道时隙的SINR值
		weak_ptr<HARQ>				m_pHARQ;									//该信道传输HARQ信息
	};
};

