#pragma once
namespace LTESim
{
	class SystemResult 
	{
	public:
		SystemResult(void);
	public:
		virtual ~SystemResult(void);

		//数值传递
		void SetTitle(const std::string& title);
		const std::string& GetTitle() const;

		//以ms为单位的统计时长
		void SetSimDuration(double duration);

		void AddUEThr( const vector<double>& uethr );
		void AddNBThr( const vector<double>& nbthr );

		void AddHSPAUEThr( const vector<double>& rHSPAUEThrVec );
		void AddHSPANBThr( const vector<double>& rHSPANBThrVec );

		void AddDualModeUEThr( const vector<double>& rDualModeUEThrVec, SystemType systemType );

		void AddNBMCS ( const vector<vector<double> >& nbmcs );
		void AddHSPANBMCS( const vector<vector<double> >& HSPAnbmcs );

		//void AddHSPAPacketTime( const vector<double>& time );

		void AddWiFiTransInterval( const vector<double>& time );
		void AddLAATransInterval( const vector<double>& time );
		void AddLTETransInterval( const vector<double>& time );

		void AddLTEPacketTime( const vector<double>& uetime, const vector<double>& alltime );
		void AddWiFiPacketTime( const vector<double>& uetime, const vector<double>& alltime );
		void AddLAAPacketTime( const vector<double>& uetime, const vector<double>& alltime );
		void AddLTEWiFiPacketTime( const vector<double>& uetime, const vector<double>& alltime, SystemType systemType );

		void AddLTEPacketLoss( const vector<double>& ueloss, const vector<double>& loss );
		void AddWiFiPacketLoss( const vector<double>& ueloss, const vector<double>& loss );
		void AddLAAPacketLoss( const vector<double>& ueloss, const vector<double>& loss );
		void AddLTEWiFiPacketLoss( const vector<double>& ueloss, const vector<double>& loss, SystemType systemType );

		void AddLTEChannelUtilization( const vector<double>& uecu, const vector<double>& cu );
		void AddWiFiChannelUtilization( const vector<double>& uecu, const vector<double>& cu );
		void AddLTEWiFiChannelUtilization( const vector<double>& uecu, const vector<double>& cu, SystemType systemType );

		void AddChannelUtilization(double lte, double wifi);	

		//void SetWiFiSimDuration( double duration );
		void AddWiFiUEThr( const vector<double>& rWiFiUEThrVec );
		void AddWiFiCollisionNum( const vector<int>& collisionnum );
		//virtual void WriteWiFiTo(std::ostream& os ) const;
		void AddLAAUEThr( const vector<double>& rLAAUEThrVec );
		void AddLAACollisionNum( const vector<int>& collisionnum );

		void AddLTEWiFiUEThr( const vector<double>& rLTEWiFiUEThrVec, SystemType systemType );


		//获得UE的吞吐量
		const std::vector<double>& GetUEThroughput() const;
		//获得NodeB的吞吐量
		const std::vector<double>& GetNBThroughput() const;
		const std::vector<double>& GetHSPAUEThroughput() const;
		const std::vector<double>& GetHSPANBThrougput() const;
		const std::vector<double>& GetDualModeUEThroughput( SystemType systemType ) const;
		const std::vector<double>& GetLTEWiFiUEThroughput( SystemType systemType ) const;
		const std::vector<double>& GetWiFiUEThroughput() const;
		const std::vector<double>& GetLAAUEThroughput() const;
		const std::vector<int>& GetWiFiCollisionNum() const;
		const std::vector<int>& GetLAACollisionNum() const;

		const std::vector<double>& GetLTEPacketTime() const;
		const std::vector<double>& GetWiFiPacketTime() const;
		const std::vector<double>& GetLAAPacketTime() const;
		const std::vector<double>& GetLTEWiFiPacketTime( SystemType systemType ) const;

		const std::vector<double>& GetLTEPacketLoss() const;
		const std::vector<double>& GetWiFiPacketLoss() const;
		const std::vector<double>& GetLAAPacketLoss() const;
		const std::vector<double>& GetLTEWiFiPacketLoss( SystemType systemType ) const;
	
		const std::vector<double>& GetLTEChannelUtilization() const;
		const std::vector<double>& GetWiFiChannelUtilization() const;
		const std::vector<double>& GetLTEWiFiChannelUtilization( SystemType systemType ) const;

		const std::vector<double>& GetChannelUtilization() const;

		virtual void WriteTo( std::ostream& os ) const;
	protected:
		std::string m_title;																//Result的标题
		double m_simduration;														//仿真时长ms
		std::vector<double> m_uethr;											//各UE统计吞吐量，bit/ms
		std::vector<double> m_nbthr;											//各NodeB的吞吐量，bit/ms
		std::vector<vector<double>> m_nbmcs;							//各NodeB的MCS等级， /ms
		std::vector<vector<double>> m_HSPAnbmcs;						//各HSPA NodeB的MCS等级， /ms
		std::vector<double> m_HSPAUEThrVec;
		std::vector<double> m_HSPANBThrVec;
		std::vector<double> m_DualModeUELTEPartThrVec;
		std::vector<double> m_DualModeUEHSPAPartThrVec;
		//std::vector<double> m_HSPAPacketTime;
		std::vector<double> m_WiFiUEThrVec;
		std::vector<double> m_LAAUEThrVec;
		//double m_dWiFiSimDuration;				//WiFi仿真时长，单位us
		std::vector<double> m_LTEWiFiUELTEPartThrVec;
		std::vector<double> m_LTEWiFiUEWiFiPartThrVec;
		std::vector<double> m_WiFiTransInterval;
		std::vector<double> m_LAATransInterval;
		std::vector<double> m_LTETransInterval;

		//每个用户对应的包传输时间、丢包率、信道利用率
		std::vector<double> m_LTEUEPacketTime;
		std::vector<double> m_WiFiUEPacketTime;
		std::vector<double> m_LAAUEPacketTime;
		std::vector<double> m_LTEWiFiUEPacketTimeLTEPart;
		std::vector<double> m_LTEWiFiUEPacketTimeWiFiPart;
		std::vector<double> m_LTEUEPacketLoss;
		std::vector<double> m_WiFiUEPacketLoss;
		std::vector<double> m_LAAUEPacketLoss;
		std::vector<double> m_LTEWiFiUEPacketLossLTEPart;
		std::vector<double> m_LTEWiFiUEPacketLossWiFiPart;
		std::vector<double> m_LTEUEChannelUtilization;
		std::vector<double> m_WiFiUEChannelUtilization;
		std::vector<double> m_LTEWiFiUEChannelUtilizationLTEPart;
		std::vector<double> m_LTEWiFiUEChannelUtilizationWiFiPart;
		//所有用户的所有包传输时间
		vector<double> m_LTEPacketTime;
		vector<double> m_WiFiPacketTime;
		vector<double> m_LAAPacketTime;
		vector<double> m_LTEWiFiPacketTimeLTEPart;
		vector<double> m_LTEWiFiPacketTimeWiFiPart;
		//vector中有两个变量，第一个是丢弃的数据包，第二个是总共的数据包
		vector<double> m_LTEPacketLoss;
		vector<double> m_WiFiPacketLoss;
		vector<double> m_LAAPacketLoss;
		vector<double> m_LTEWiFiPacketLossLTEPart;
		vector<double> m_LTEWiFiPacketLossWiFiPart;
		//vector中有两个变量，第一个是总的吞吐量，第二个是理想的吞吐量
		vector<double> m_LTEChannelUtilization;
		vector<double> m_WiFiChannelUtilization;
		vector<double> m_LTEWiFiChannelUtilizationLTEPart;
		vector<double> m_LTEWiFiChannelUtilizationWiFiPart;
		//vector中有两个变量，第一个是LTE的信道利用率，第二个WiFi的信道利用率
		vector<double> m_ChannelUtilization;

		std::vector<int> m_WiFiCollisionNum;
		std::vector<int> m_LAACollisionNum;
	};

	//重载“<<”，方便输出
	std::ostream& operator << (std::ostream& os, const SystemResult& result);
}