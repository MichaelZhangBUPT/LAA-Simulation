#pragma once

namespace LTESim
{

class SystemResult;
class RunResult
{
public:
	RunResult(void);
	~RunResult(void);
public:
	void SetTitle( const std::string& title );
	const std::string& GetTitle() const;
	void AddResult( const SystemResult& DropResult );
	void WriteTo( std::ostream& out ) const;
	//void WriteWiFiTo( std::ostream& os ) const;
protected:
	void AddUEThr( const vector<double> &uethr );
	void AddNBThr( const vector<double> &nbthr );
	void AddHAPAUEThr( const vector<double>& rHSPAUEThrVec );
	void AddHSPANBThr( const vector<double>& rHSPANBThrVec );
	void AddWiFiUEThr( const vector<double>& rWiFiUEThrVec );
	void AddLAAUEThr( const vector<double>& rLAAUEThrVec );
	void AddDualModeUEThr( const vector<double>& rDualModeUEThrVec, SystemType systemType );
	void AddLTEWiFiUEThr( const vector<double>& rLTEWiFiUEThrVec, SystemType systemType );
	void AddWiFiCollisionNum( const vector<int>& collisionnum );
	void AddLAACollisionNum( const vector<int>& collisionnum );

	void AddLTEPacketTime( const vector<double>& time );
	void AddWiFiPacketTime( const vector<double>& time );
	void AddLAAPacketTime( const vector<double>& time );
	void AddLTEWiFiPacketTime( const vector<double>& time, SystemType systemType );

	void AddLTEPacketLoss( const vector<double>& loss );
	void AddWiFiPacketLoss( const vector<double>& loss );
	void AddLAAPacketLoss( const vector<double>& loss );
	void AddLTEWiFiPacketLoss( const vector<double>& loss, SystemType systemType );

	void AddLTEChannelUtilization( const vector<double>& cu );
	void AddWiFiChannelUtilization( const vector<double>& cu );
	void AddLTEWiFiChannelUtilization( const vector<double>& cu, SystemType systemType );

	void AddChannelUtilization( const vector<double>& cu );

protected:
	std::string m_title;
	std::vector<double> m_uethr;
	std::vector<double> m_nbthr;
	std::vector<double> m_HSPAUEThroughputVec;
	std::vector<double> m_HSPANBThroughputVec;
	std::vector<double> m_DualModeUELTEPartThroughputVec;
	std::vector<double> m_DualModeUEHSPAPartThroughputVec;
	std::vector<double> m_WiFiUEThroughputVec;
	std::vector<double> m_LAAUEThroughputVec;
	std::vector<double> m_LTEWiFiUELTEPartThroughputVec;
	std::vector<double> m_LTEWiFiUEWiFiPartThroughputVec;

	std::vector<double> m_LTEPacketTime;
	std::vector<double> m_WiFiPacketTime;
	std::vector<double> m_LAAPacketTime;
	std::vector<double> m_LTEWiFiPacketTimeLTEPart;
	std::vector<double> m_LTEWiFiPacketTimeWiFiPart;

	//vector中有两个变量，第一个是丢弃的数据包，第二个是总共的数据包
	std::vector<double> m_LTEPacketLoss;
	std::vector<double> m_WiFiPacketLoss;
	std::vector<double> m_LAAPacketLoss;
	std::vector<double> m_LTEWiFiPacketLossLTEPart;
	std::vector<double> m_LTEWiFiPacketLossWiFiPart;

	//vector中有两个变量，第一个是总的吞吐量，第二个是理想的吞吐量
	std::vector<double> m_LTEChannelUtilization;
	std::vector<double> m_WiFiChannelUtilization;
	std::vector<double> m_LTEWiFiChannelUtilizationLTEPart;
	std::vector<double> m_LTEWiFiChannelUtilizationWiFiPart;

	//vector中有Drop数个变量，分别是每个Drop的系统的信道利用率
	std::vector<double> m_LTECU;
	std::vector<double> m_WiFiCU;

	std::vector<int> m_WiFiCollisionNum;
	std::vector<int> m_LAACollisionNum;
};

}
