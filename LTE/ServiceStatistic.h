#pragma once
namespace LTESim
{
	//数据统计采用Visitor模式
	class Service;
	class DLFTPService;
	class ServiceStatistic
	{
	public:
		ServiceStatistic(void);
	public:
		virtual ~ServiceStatistic(void);
		virtual void Visit(Service& service) = 0;								//数据统计接口	
		virtual const vector<double>& GetStatistic() const = 0;	//获得每个用户的统计数据
		virtual const vector<double>& GetAllStatistic() { vector<double> temp(1); return temp; }	//获得原始数据
		virtual double GetSumStatistic() const =0;						//获得系统的统计数据
	};

	//吞吐量统计 
	class ThroughputStatistic : public ServiceStatistic
	{
	public:
		ThroughputStatistic(void);

	public:
		~ThroughputStatistic(void);
		void Visit(Service& service);	//数据统计接口

		const vector<double>& GetStatistic(void) const;	//获得用户FTP业务吞吐量统计表	
		double GetSumStatistic(void) const;	//获得系统的总吞吐量

	protected:
		vector<double> m_DLFTPthroughput;	//存储每个用户FTP业务吞吐量
		double DLFTPSumThroughput;	//系统每个Drop的FTP总吞吐量

	};
	//丢弃数据统计
	class DiscardThroughputStatistic : public ServiceStatistic
	{
	public:
		DiscardThroughputStatistic(void);

	public:
		~DiscardThroughputStatistic(void);
		void Visit(Service& service);	//数据统计接口

		const vector<double>& GetStatistic(void) const;	//获得用户FTP业务丢弃包的吞吐量统计表	
		double GetSumStatistic(void) const;	//获得系统的总丢弃包的吞吐量

	protected:
		vector<double> m_DLFTPDiscardThroughput;	//存储每个用户FTP业务丢弃包的吞吐量
		double DLFTPSumDiscardThroughput;	//系统每个Drop的FTP总丢弃包的吞吐量
	};
	//传输时延统计
	class PacketDelayStatistic : public ServiceStatistic
	{
	public:
		PacketDelayStatistic(void);

	public:
		~PacketDelayStatistic(void);
		void Visit(Service& service);	//数据统计接口

		const vector<double>& GetStatistic(void) const;	//获得用户FTP业务的时延统计表
		double GetSumStatistic(void) const;	//获得系统的总时延

	protected:
		vector<double> m_DLFTPPacketDelay;	//存储每个用户FTP业务的时延
		double DLFTPSumPacketDelay;	//系统每个Drop的FTP总时延
	};
	//统计MCS等级
	class MCSStatistic : public ServiceStatistic
	{
	public:
		MCSStatistic( /*SystemType systemtype*/ );

	public:
		~MCSStatistic(void);
		void Visit(Service& service); //MCS等级统计接口

		const vector<double>& GetStatistic(void) const; //获得系统的MCS等级统计
		double GetSumStatistic(void) const;
	protected:
		vector< vector<double> > m_MCS;
		vector<double> DLFTPSumMCS;
	};

	//业务包传输时间统计
	class PacketTimeStatistic : public ServiceStatistic
	{
	public:
		PacketTimeStatistic(void);

	public:
		~PacketTimeStatistic(void);
		void Visit(Service& service);	//数据统计接口

		const vector<double>& GetStatistic(void) const;	//返回每个用户的平均值
		const vector<double>& GetAllStatistic(void) ;	//返回所有用户的所有的值
		double GetSumStatistic(void) const;		//返回系统一个Drop的平均值
	protected:
		vector<double> m_UEAverPacketTime;	//每个用户的平均值
		vector<double> m_AllPacketTime;	//所有用户的所有的值，用来计算系统的平均值
	};

	//FullBuffer 传输时间间隔统计
	class TransIntervalStatistic : public ServiceStatistic
	{
	public:
		TransIntervalStatistic(void);

	public:
		~TransIntervalStatistic(void);
		void Visit(Service& service);	//数据统计接口

		const vector<double>& GetStatistic(void) const;	//
		double GetSumStatistic(void) const;
	protected:
		std::vector<double> m_DLFTPAverTransInterval;	//
	};
	//业务丢包率统计
	class PacketLossStatistic : public ServiceStatistic
	{
	public:
		PacketLossStatistic(void);
	public:
		~PacketLossStatistic(void);
		void Visit(Service& service);	//数据统计接口
		const vector<double>& GetStatistic(void) const;	//返回每个用户的丢包率
		const vector<double>& GetAllStatistic(void) ;	//返回系统总的丢包数和总的数据包
		double GetSumStatistic(void) const;	//返回系统一个Drop的丢包率
	protected:
		vector<double> m_UEPacketLoss;	//每个用户的丢包率
		int	m_PacketNum;	//系统总的数据包个数
		int m_DiscardPacketNum;		//总的丢弃的数据包个数
		vector<double> m_DiscardAndSum;
	};
	//信道利用率统计,不再使用
	class ChannelUtilizationStatistic : public ServiceStatistic
	{
	public:
		ChannelUtilizationStatistic(void);
	public:
		~ChannelUtilizationStatistic(void);
		void Visit(Service& service);	//数据统计接口
		const vector<double>& GetStatistic(void) const;	//返回每个用户的信道利用率
		const vector<double>& GetAllStatistic(void) ;	//返回系统的吞吐量和理想的吞吐量
		double GetSumStatistic(void) const;	//返回系统一个Drop的信道利用率
	protected:
		vector<double> m_UEChannelUti;	//每个用户的信道利用率
		double m_SumThroughput;
		double m_SumIdealThroughput;
		vector<double> m_SumAndIdeal;
	};


}

