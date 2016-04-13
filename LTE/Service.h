#pragma once

#include "commondef.h"
#include <vector>

namespace LTESim
{
	class IUE;
	class ServiceStatistic;

	//业务流量
	typedef double DATASIZE;
	
	typedef double DATADELAY;
	//优先级
	typedef double PRIORITY;

	//业务类
	class Service
	{
	public:
		Service(shared_ptr<IUE> pUE);
	public:
		virtual ~Service(void);
		//对应的UE
		shared_ptr<IUE> GetUE();
		//当前的buffer状态
		virtual DATASIZE	GetData() const;
		void SetData( DATASIZE dDataSize );

		double			GetInterval() const;
		DATASIZE	GetThroughput() const;
		DATASIZE	GetIdealThroughput() const;
		DATASIZE	GetAverageThroughput();
		void				SetAverageThroughput( DATASIZE thr);
		DATASIZE	GetDiscardThroughput() const;

		//获得MCS等级 YRM
		const vector<double>& GetMCSIndex() const;

		//获得业务包的传输和读取时间
		std::vector<double> GetPacketTransTime() const;
		std::vector<double> GetPacketReadingTime() const;
		double	GetPacketStartTime() const;
		void	SetPacketStartTime( double starttime );
		double	GetPacketEndTime() const;
		void	SetPacketEndTime( double endtime );

		//获得一个数据包总共的传输次数和错误传输次数
		void	UpdatePacketTransTimes()	{ m_iPacketTransTimes++; }
		int		GetPacketTransTimes()	{ return m_iPacketTransTimes; }
		void	UpdatePacketFailedTransTimes()	{ m_iPacketFailedTransTimes++; }
		int		GetPacketFailedTransTimes()	{ return m_iPacketFailedTransTimes; }
		void	ResetPacketTransTimes()	{ m_iPacketTransTimes = 0; }
		void	ResetPacketFailedTransTimes()	{ m_iPacketFailedTransTimes = 0; }
		
		//FullBuffer时，统计传输间隔
		void SetLastTransTime( double time ) { m_dLastTransTime = time; }
		void SetThisTransTime( double time ) { m_dThisTransTime = time; }
		double GetLastTransTime() { return m_dLastTransTime; }
		double GetThisTransTime() { return m_dThisTransTime; }
		std::vector<double> GetTransInterval(); 
		void UpdateTransInterval();



		//传输数据时延
		DATADELAY GetPacketDelay() const;

		//业务优先级
		PRIORITY GetQoS() const;
		void SetQoS(PRIORITY p);
		
		//是否被调度上标识
		bool GetIsScheduled() const;
		void SetIsScheduled(bool scheduled);

		//当前业务是否结束
		bool GetIsEnd() const;
		void SetIsEnd(bool ending);
		
		//获得最后调度包大小
		double GetLastPacketsize()const;
		void   SetLastPacketsize( double size);
		//根据业务流分布更新业务buffer
		virtual void UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/) = 0;
		//生成读取时间
		virtual void UpdateGenerateInterval(void) = 0;	

		virtual void UpdateThroughput(DATASIZE throughput) = 0;

		virtual void UpdateDiscardThroughput(DATASIZE throughput) = 0;

		virtual void UpdateMCSIndex(int mcsindex) = 0;

		//void UpdatePacketTime( double time );

		/*从buffer中取出data个单位的数据发送*/
		virtual void PopData(DATASIZE data);

		virtual void Accept( ServiceStatistic& visit ) = 0;
		//VoIP业务更新
		virtual void UpdateService() {}
		//丢包率
		int GetPacketNum();
		int GetDiscardPacketNum();
		double GetChannelUtilization();

		ServiceType GetServiceType();
	protected:
		/*往buffer中加入data个单位的新数据*/
		virtual void PutBuffer(DATASIZE data);
	protected:
		DATASIZE						m_dBufferData;
		double							m_iGenInterval;					//包产生时间间隔
		PRIORITY						m_priority;						//优先级
		bool							m_IsScheduled;					//是否被调度上
		bool							m_IsEnd;						//当前业务是否结束
		double							m_lastblocksize;				//记录最后一个包大小(PF算法需要）
		weak_ptr<IUE>					m_pUE;							//所属的UE
		DATASIZE						m_SumIdealThroughput;			//理想状态下，可以传输的吞吐量，用于计算信道利用率
		DATASIZE						m_SumThroughput;				//累积传输成功的吞吐量
		DATASIZE						m_SumDiscardThroughput;			//丢弃数据包的吞吐量
		DATADELAY						m_SumPacketDelay;				//累积包传输时延
		DATASIZE						m_AverageThroughput;			//平均吞吐量
		std::vector<double>				m_MCSIndex;						//MCS等级  YRM
		//时间统计
		double							m_dPacketStartTime;				//生成一个包的起始时间
		double							m_dPacketEndTime;				//一个数据包传输结束的时间
		std::vector<double>				m_vecPacketTransTime;			//每个包的传输时间
		std::vector<double>				m_vecPacketReadingTime;			//每个包的读取时间
		int								m_iPacketTransTimes;			//一个数据包总共的传输次数
		int								m_iPacketFailedTransTimes;		//一个数据包总共错误的传输次数
		//FullBuffer业务时，统计一个用户两次传输之间的时延
		double							m_dLastTransTime;				//上一次传输时间
		double							m_dThisTransTime;				//本次传输时间
		std::vector<double>				m_vecTransInterval;				//传输间隔
		int								m_iPacketNum;	//总共传输的数据包的个数,用来统计丢包率
		int								m_iDiscardPacketNum;	//达到最大传输次数的数据包的个数
		ServiceType						m_ServiceType;
};
}