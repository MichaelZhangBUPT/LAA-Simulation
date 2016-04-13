#pragma once
#include "commondef.h"
#include "AMCEntity.h"
#include "Service.h"
#include "mcs.h"

#include <map>
#include <vector>

namespace LTESim
{
	class HARQ
	{
	public:
		typedef std::map <int, vector<double> > SINRLIST;					//重传数据的SINR值，Frist为进程数，second为SINR值
		HARQ(shared_ptr<Service> pService, int processno/*进程编号*/, int ueid);
		HARQ(){};
		virtual ~HARQ(void);
	public:
		const MCS&						GetMCS();
		void							SetMCS(MCS mcs);

		void							RecordSINROfRetranPacket(const vector<double>& vSINRList);	//记录重传数据的SINR列表
		void							SINRCombining(vector<double>& RealSINRList   ) ;

		void							InitTrans();									//初次传输的时候调用，记录初次传输时刻
		void							UpdateTranTime();						//更新传输次数
		bool							MaxTransTime() const;					//
		int								GetTranTime()const;

		void							SetService(shared_ptr<Service> pService);
		shared_ptr<Service>				GetService();

		bool							GetNewPacketIndicator(void) const;
		void							UpdateTransStatus(bool bReady);	//更新进程传输状态，调度后未反馈时为false，可以使用作为调度时为true
		bool							BeReady() const;							//判断该进程是否已经在用于传输而没有返回
		int								GetProcessNo() const;
		int								GetInitTransTime() const;				//得到进程携带的packet的初始传输时刻，为HARQEntity进行HARQ进程选择使用
		
		void							SetUsedRBNum(size_t rbnum);		//设置本进程所需要的RB数目
		size_t						GetUsedRBNum()const;				//得到所需的RB数目
		void							Reset();

		void							SetRetransC2I( double dC2IValue );
		double						GetRetransC2I() const;
/*
		bool							IsNewData();
		void							CombineCQIVector( std::vector<double>& o_CQIVec );
		void							RecordCQIVectorOfRetranPacket( std::vector<double>& i_CQIVec );
		void							RecordMCSOfRetranPacket( MCS& refMCS );
		MCS							GetMCSOfRetransPacket();

		//增加重传的次数
		void							IncreaseRetransTime();
		int								GetRetransTime();
		bool							IsMaxRetransTime();

		//传输正确了之后，进行状态的重置
		void							Reset();
*/


		//void			    SetProcessNo(int processno);
		//void				SetTranTime(int trantime);
		//void				SetUEID(int ueid);
		//void				SetNewPacketIndicator(bool packindicator);
		//vector	<double>    GetSINROfRetranPacket(int processnum);
		//void				SINRListErase(int processnum);
		//void				SetReady();//标志该HARQ的反馈已经到达，可以用于调度

	protected:
		int							m_iMaxTransTime;										//最大传输次数
		int							m_iProcessNo;												//HARQ进程数
		MCS							m_MCS;														//MCS等级数				
		int							m_iTranTime;												//传输次数 注意：2表示重传了一次
		int							m_UEID;														//用户ID		
		weak_ptr<Service>	m_pService;												//该HARQ进程对应的业务
		
		//重传相关数据
		vector<double>		m_RetranSINRList;									    //重传数据的SINR值，Frist为进程数，second为SINR值
		bool							m_bReady;													//是否可调度，对于已经调度上尚未反馈的进程为false，初始为true
		int							m_InitialTransClock;										//初始传输的时刻(SYSCLOCK)
        size_t						m_RBNUM;													//记录所使用的RB数目

		double						m_dRetransC2I;											//记录前次传输错误，记录的历史C2I，用于下次重传的时候进行CQI合并
	};
}
