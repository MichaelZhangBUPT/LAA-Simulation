#include "commondef.h"
#include "RunResult.h"
#include "SystemResult.h"
#include "System.h"
//#include "Configuration.h"
#include "RunParameters.h"
#include <numeric>
#include <string>
#include <iomanip>

namespace LTESim
{
	RunResult::RunResult(void)
	{
	}

	RunResult::~RunResult(void)
	{
	}

	void RunResult::SetTitle(const std::string& title)
	{
		m_title  = title;
	};

	const std::string& RunResult::GetTitle() const
	{
		return m_title;
	}

	void RunResult::AddResult( const SystemResult& DropResult )
	{
		AddUEThr(DropResult.GetUEThroughput());
		AddNBThr(DropResult.GetNBThroughput());
		AddHAPAUEThr(DropResult.GetHSPAUEThroughput());
		AddHSPANBThr(DropResult.GetHSPANBThrougput());
		AddWiFiUEThr(DropResult.GetWiFiUEThroughput());
		AddLAAUEThr(DropResult.GetLAAUEThroughput());
		AddDualModeUEThr(DropResult.GetDualModeUEThroughput( SYSTEM_LTE ), SYSTEM_LTE);
		AddDualModeUEThr(DropResult.GetDualModeUEThroughput( SYSTEM_HSPA ), SYSTEM_HSPA);
		AddLTEWiFiUEThr(DropResult.GetLTEWiFiUEThroughput( SYSTEM_LTE ), SYSTEM_LTE);
		AddLTEWiFiUEThr(DropResult.GetLTEWiFiUEThroughput( SYSTEM_WIFI ), SYSTEM_WIFI);
		AddWiFiCollisionNum(DropResult.GetWiFiCollisionNum());
		AddLAACollisionNum(DropResult.GetLAACollisionNum());

		AddLTEPacketTime( DropResult.GetLTEPacketTime() );
		AddWiFiPacketTime( DropResult.GetWiFiPacketTime() );
		AddLAAPacketTime( DropResult.GetLAAPacketTime() );
		AddLTEWiFiPacketTime( DropResult.GetLTEWiFiPacketTime( SYSTEM_LTE ), SYSTEM_LTE );
		AddLTEWiFiPacketTime( DropResult.GetLTEWiFiPacketTime( SYSTEM_WIFI ), SYSTEM_WIFI );

		AddLTEPacketLoss( DropResult.GetLTEPacketLoss() );
		AddWiFiPacketLoss( DropResult.GetWiFiPacketLoss() );
		AddLAAPacketLoss( DropResult.GetLAAPacketLoss() );
		AddLTEWiFiPacketLoss( DropResult.GetLTEWiFiPacketLoss( SYSTEM_LTE ), SYSTEM_LTE );
		AddLTEWiFiPacketLoss( DropResult.GetLTEWiFiPacketLoss( SYSTEM_WIFI ), SYSTEM_WIFI );

		AddChannelUtilization(DropResult.GetChannelUtilization());
	}

	void RunResult::AddUEThr( const vector<double> &uethr )
	{
		m_uethr.insert(m_uethr.end(), uethr.begin(), uethr.end());
	}

	void RunResult::AddNBThr( const vector<double> &nbthr )
	{
		m_nbthr.insert(m_nbthr.end(), nbthr.begin(), nbthr.end());
	}

	void RunResult::AddHAPAUEThr( const vector<double>& rHSPAUEThrVec )
	{
		m_HSPAUEThroughputVec.insert( m_HSPAUEThroughputVec.end(), rHSPAUEThrVec.begin(), rHSPAUEThrVec.end() );
	}

	void RunResult::AddHSPANBThr( const vector<double>& rHSPANBThrVec )
	{
		m_HSPANBThroughputVec.insert( m_HSPANBThroughputVec.end(), rHSPANBThrVec.begin(), rHSPANBThrVec.end() );
	}

	void RunResult::AddWiFiUEThr( const vector<double>& rWiFiUEThrVec )
	{
		m_WiFiUEThroughputVec.insert(m_WiFiUEThroughputVec.end(), rWiFiUEThrVec.begin(), rWiFiUEThrVec.end());
	}

	void RunResult::AddLAAUEThr( const vector<double>& rLAAUEThrVec )
	{
		m_LAAUEThroughputVec.insert(m_LAAUEThroughputVec.end(), rLAAUEThrVec.begin(), rLAAUEThrVec.end());
	}

	void RunResult::AddDualModeUEThr( const vector<double>& rDualModeUEThrVec, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			m_DualModeUELTEPartThroughputVec.insert( m_DualModeUELTEPartThroughputVec.end(), rDualModeUEThrVec.begin(), rDualModeUEThrVec.end() );
			break;
		case SYSTEM_HSPA:
			m_DualModeUEHSPAPartThroughputVec.insert( m_DualModeUEHSPAPartThroughputVec.end(), rDualModeUEThrVec.begin(), rDualModeUEThrVec.end() );
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	void RunResult::AddLTEWiFiUEThr( const vector<double>& rLTEWiFiUEThrVec, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			m_LTEWiFiUELTEPartThroughputVec.insert( m_LTEWiFiUELTEPartThroughputVec.end(), rLTEWiFiUEThrVec.begin(), rLTEWiFiUEThrVec.end() );
			break;
		case SYSTEM_WIFI:
			m_LTEWiFiUEWiFiPartThroughputVec.insert( m_LTEWiFiUEWiFiPartThroughputVec.end(), rLTEWiFiUEThrVec.begin(), rLTEWiFiUEThrVec.end() );
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	void RunResult::AddWiFiCollisionNum( const vector<int>& collisionnum )
	{
		m_WiFiCollisionNum.insert(m_WiFiCollisionNum.end(),collisionnum.begin(),collisionnum.end());
	}

	void RunResult::AddLAACollisionNum( const vector<int>& collisionnum )
	{
		m_LAACollisionNum.insert(m_LAACollisionNum.end(),collisionnum.begin(),collisionnum.end());
	}

	void RunResult::AddLTEPacketTime( const vector<double>& time )
	{
		m_LTEPacketTime.insert(m_LTEPacketTime.end(), time.begin(), time.end());
	}

	void RunResult::AddWiFiPacketTime( const vector<double>& time )
	{
		m_WiFiPacketTime.insert(m_WiFiPacketTime.end(), time.begin(), time.end());
	}

	void RunResult::AddLAAPacketTime( const vector<double>& time )
	{
		m_LAAPacketTime.insert(m_LAAPacketTime.end(), time.begin(), time.end());
	}

	void RunResult::AddLTEWiFiPacketTime( const vector<double>& time, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			m_LTEWiFiPacketTimeLTEPart.insert(m_LTEWiFiPacketTimeLTEPart.end(), time.begin(), time.end());
			break;
		case SYSTEM_WIFI:
			m_LTEWiFiPacketTimeWiFiPart.insert(m_LTEWiFiPacketTimeWiFiPart.end(), time.begin(), time.end());
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	void RunResult::AddLTEPacketLoss(const std::vector<double> &loss)
	{
		if ( m_LTEPacketLoss.empty() )
		{		
			m_LTEPacketLoss = loss;
		}
		else
		{
			m_LTEPacketLoss[0] += loss[0];
			m_LTEPacketLoss[1] += loss[1];
		}
	}

	void RunResult::AddWiFiPacketLoss(const std::vector<double> &loss)
	{
		if ( m_WiFiPacketLoss.empty() )
		{		
			m_WiFiPacketLoss = loss;
		}
		else
		{
			m_WiFiPacketLoss[0] += loss[0];
			m_WiFiPacketLoss[1] += loss[1];
		}
	}

	void RunResult::AddLAAPacketLoss(const std::vector<double> &loss)
	{
		if ( m_LAAPacketLoss.empty() )
		{		
			m_LAAPacketLoss = loss;
		}
		else
		{
			m_LAAPacketLoss[0] += loss[0];
			m_LAAPacketLoss[1] += loss[1];
		}
	}

	void RunResult::AddLTEWiFiPacketLoss( const vector<double>& loss, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			{
				if ( m_LTEWiFiPacketLossLTEPart.empty() )
				{		
					m_LTEWiFiPacketLossLTEPart = loss;
				}
				else
				{
					m_LTEWiFiPacketLossLTEPart[0] += loss[0];
					m_LTEWiFiPacketLossLTEPart[1] += loss[1];
				}
			}
			break;
		case SYSTEM_WIFI:
			{
				if ( m_LTEWiFiPacketLossWiFiPart.empty() )
				{		
					m_LTEWiFiPacketLossWiFiPart = loss;
				}
				else
				{
					m_LTEWiFiPacketLossWiFiPart[0] += loss[0];
					m_LTEWiFiPacketLossWiFiPart[1] += loss[1];
				}
			}
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	void RunResult::AddLTEChannelUtilization(const std::vector<double> &cu)
	{
		if ( m_LTEChannelUtilization.empty() )
		{		
			m_LTEChannelUtilization = cu;
		}
		else
		{
			m_LTEChannelUtilization[0] += cu[0];
			m_LTEChannelUtilization[1] += cu[1];
		}
	}

	void RunResult::AddWiFiChannelUtilization(const std::vector<double> &cu)
	{
		if ( m_WiFiChannelUtilization.empty() )
		{		
			m_WiFiChannelUtilization = cu;
		}
		else
		{
			m_WiFiChannelUtilization[0] += cu[0];
			m_WiFiChannelUtilization[1] += cu[1];
		}
	}

	void RunResult::AddLTEWiFiChannelUtilization( const vector<double>& cu, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			{
				if ( m_LTEWiFiChannelUtilizationLTEPart.empty() )
				{		
					m_LTEWiFiChannelUtilizationLTEPart = cu;
				}
				else
				{
					m_LTEWiFiChannelUtilizationLTEPart[0] += cu[0];
					m_LTEWiFiChannelUtilizationLTEPart[1] += cu[1];
				}
			}
			break;
		case SYSTEM_WIFI:
			{
				if ( m_LTEWiFiChannelUtilizationWiFiPart.empty() )
				{		
					m_LTEWiFiChannelUtilizationWiFiPart = cu;
				}
				else
				{
					m_LTEWiFiChannelUtilizationWiFiPart[0] += cu[0];
					m_LTEWiFiChannelUtilizationWiFiPart[1] += cu[1];
				}
			}
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	void RunResult::AddChannelUtilization( const vector<double>& cu )
	{
			m_LTECU.push_back(cu[0]);
			m_WiFiCU.push_back(cu[1]);
	}

	void LTESim::RunResult::WriteTo( std::ostream& os ) const
	{
		os << m_title << endl << endl;

#pragma region LTE Result

		double dLTEUESumThr = std::accumulate(m_uethr.begin(), m_uethr.end(), 0.0);
		os << "Total LTE UE Throughput:\t" << dLTEUESumThr << " Kbps"  << endl;
		os << "Average LTE UE Cell Throughput:\t" << dLTEUESumThr / m_nbthr.size() << " Kbps"  << endl;
		os << "Average LTE UE Throughput:\t" << dLTEUESumThr / m_uethr.size() << " Kbps"  << endl;
		double dLTENBSumThr = std::accumulate( m_nbthr.begin(), m_nbthr.end(), 0.0 );
		os << "Average LTE Cell Throughput:\t" << dLTENBSumThr / m_nbthr.size() << " Kbps"  << endl;
	
		os << "Average LTE UE Packet Time:\t" << std::accumulate(m_LTEPacketTime.begin(),m_LTEPacketTime.end(),0.0)/m_LTEPacketTime.size() << endl;
		double loss;
		if( !m_LTEPacketLoss.empty() && m_LTEPacketLoss[1]>0 )	loss=m_LTEPacketLoss[0]/m_LTEPacketLoss[1];
		else	loss=0.0;
		os << "Average LTE UE Packet Loss:\t" << loss << endl;
		//double cu;
		//if( !m_LTEChannelUtilization.empty() && m_LTEChannelUtilization[1]>0 )	cu=m_LTEChannelUtilization[0]/m_LTEChannelUtilization[1];
		//else	cu=0.0;
		//os << "Average LTE UE Channel Utilization:\t" << cu << endl;
		os << endl;

#pragma endregion

#pragma region HSPA Result

		double dHSPAUESumThr = std::accumulate( m_HSPAUEThroughputVec.begin(), m_HSPAUEThroughputVec.end(), 0.0 );
		os << "Total HSPA UE Throughput:\t" << dHSPAUESumThr << " Kbps"  << endl;
		os << "Average HSPA UE Throughput:\t" << dHSPAUESumThr / m_HSPAUEThroughputVec.size() << " Kbps"  << endl;
		double dHSPANBSumThr = std::accumulate( m_HSPANBThroughputVec.begin(), m_HSPANBThroughputVec.end(), 0.0 );
		os << "Average HSPA Cell Throughput:\t" << dHSPANBSumThr / m_HSPANBThroughputVec.size() << " Kbps"  << endl;
		os << endl;

#pragma endregion

#pragma region WiFi Result

		double dWiFiUESumThr = std::accumulate(m_WiFiUEThroughputVec.begin(), m_WiFiUEThroughputVec.end(), 0.0);
		os << "Total WiFi UE Throughput:\t" << dWiFiUESumThr << " Kbps"  << endl;
		os << "Average WiFi UE Cell Throughput:\t" << dWiFiUESumThr / 4<< " Kbps"  << endl;
		os << "Average WiFi UE Throughput:\t" << dWiFiUESumThr / m_WiFiUEThroughputVec.size() << " Kbps"  << endl;
	
		os << "Average WiFi UE Packet Time:\t" << std::accumulate(m_WiFiPacketTime.begin(),m_WiFiPacketTime.end(),0.0)/m_WiFiPacketTime.size() << endl;
		if( !m_WiFiPacketLoss.empty() && m_WiFiPacketLoss[1]>0 )	loss=m_WiFiPacketLoss[0]/m_WiFiPacketLoss[1];
		else	loss=0.0;
		os << "Average WiFi UE Packet Loss:\t" << loss << endl;
		//if( !m_WiFiChannelUtilization.empty() && m_WiFiChannelUtilization[1]>0 )	cu=m_WiFiChannelUtilization[0]/m_WiFiChannelUtilization[1];
		//else	cu=0.0;
		//os << "Average WiFi UE Channel Utilization:\t" << cu << endl;
		os << "Average WiFi Cell Collision Num:\t" << std::accumulate(m_WiFiCollisionNum.begin(),m_WiFiCollisionNum.end(),0.0)/m_WiFiCollisionNum.size() << endl;
		os << endl;

#pragma endregion


#pragma region LAA Result

		double dLAAUESumThr = std::accumulate(m_LAAUEThroughputVec.begin(), m_LAAUEThroughputVec.end(), 0.0);
		os << "Total LAA UE Throughput:\t" << dLAAUESumThr << " Kbps"  << endl;
		os << "Average LAA UE Cell Throughput:\t" << dLAAUESumThr / 4 << " Kbps"  << endl;
		os << "Average LAA UE Throughput:\t" << dLAAUESumThr / m_LAAUEThroughputVec.size() << " Kbps"  << endl;
	
		os << "Average LAA UE Packet Time:\t" << std::accumulate(m_LAAPacketTime.begin(),m_LAAPacketTime.end(),0.0)/m_LAAPacketTime.size() << endl;
		if( !m_LAAPacketLoss.empty() && m_LAAPacketLoss[1]>0 )	loss=m_LAAPacketLoss[0]/m_LAAPacketLoss[1];
		else	loss=0.0;
		os << "Average LAA UE Packet Loss:\t" << loss << endl;
		os << "Average LAA Cell Collision Num:\t" << std::accumulate(m_LAACollisionNum.begin(),m_LAACollisionNum.end(),0.0)/m_LAACollisionNum.size() << endl;
		os << endl;

#pragma endregion

#pragma region DualMode Result

		double dDualModeUELTEPartSumThr = std::accumulate(m_DualModeUELTEPartThroughputVec.begin(), m_DualModeUELTEPartThroughputVec.end(), 0.0);
		double dDualModeUEHSPAPartSumThr = std::accumulate(m_DualModeUEHSPAPartThroughputVec.begin(), m_DualModeUEHSPAPartThroughputVec.end(), 0.0);
		os << "DualModeUESumThroughput" << "\t" << "LTE_Thr(Kbps)" << "\t" << "HSPA_Thr(Kbps)" << endl;
		os << "DualModeUESumThroughput"
			<< "\t" << dDualModeUELTEPartSumThr
			<< "\t" << dDualModeUEHSPAPartSumThr
			<< endl;
		os << "DualModeUEAverage" << "\t" << "LTE_Thr(Kbps)" << "\t" << "HSPA_Thr(Kbps)" << endl;
		os << "DualModeUEAverage"
			<< "\t" << dDualModeUELTEPartSumThr / m_DualModeUELTEPartThroughputVec.size()
			<< "\t" << dDualModeUEHSPAPartSumThr / m_DualModeUEHSPAPartThroughputVec.size()
			<< endl;
		os << endl;

#pragma endregion

#pragma region LTEWiFi Result

		double dLTEWiFiUELTEPartSumThr = std::accumulate(m_LTEWiFiUELTEPartThroughputVec.begin(), m_LTEWiFiUELTEPartThroughputVec.end(), 0.0);
		double dLTEWiFiUEWiFiPartSumThr = std::accumulate(m_LTEWiFiUEWiFiPartThroughputVec.begin(), m_LTEWiFiUEWiFiPartThroughputVec.end(), 0.0);
		os << "\t\t\t" << "LTE_Thr(Kbps)" << "\t" << "WiFi_Thr(Kbps)" << endl;
		os << "LTEWiFiUESumThroughput" << "\t" << dLTEWiFiUELTEPartSumThr << "\t" << dLTEWiFiUEWiFiPartSumThr << endl;
		os << "LTEWiFiUECellAverage"
			<< "\t" << dLTEWiFiUELTEPartSumThr / m_nbthr.size()
			<< "\t" << dLTEWiFiUEWiFiPartSumThr / m_nbthr.size()
			<< endl;
		os << "LTEWiFiUEAverage"
			<< "\t" << dLTEWiFiUELTEPartSumThr / m_LTEWiFiUELTEPartThroughputVec.size()
			<< "\t" << dLTEWiFiUEWiFiPartSumThr / m_LTEWiFiUEWiFiPartThroughputVec.size()
			<< endl;

		os << "Average LTEWiFi UE Packet Time:\t" << std::accumulate(m_LTEWiFiPacketTimeLTEPart.begin(),m_LTEWiFiPacketTimeLTEPart.end(),0.0)/m_LTEWiFiPacketTimeLTEPart.size() 
			<< "\t" << std::accumulate(m_LTEWiFiPacketTimeWiFiPart.begin(),m_LTEWiFiPacketTimeWiFiPart.end(),0.0)/m_LTEWiFiPacketTimeWiFiPart.size() << endl;
		if( !m_LTEWiFiPacketLossLTEPart.empty() && m_LTEWiFiPacketLossLTEPart[1]>0 )	loss=m_LTEWiFiPacketLossLTEPart[0]/m_LTEWiFiPacketLossLTEPart[1];
		else	loss=0.0;
		double loss2;
		if( !m_LTEWiFiPacketLossWiFiPart.empty() && m_LTEWiFiPacketLossWiFiPart[1]>0 )	loss2=m_LTEWiFiPacketLossWiFiPart[0]/m_LTEWiFiPacketLossWiFiPart[1];
		else	loss2=0.0;
		os << "Average LTEWiFi UE Packet Loss:\t" << loss << "\t" << loss2 << endl;
		//if( !m_LTEWiFiChannelUtilizationLTEPart.empty() && m_LTEWiFiChannelUtilizationLTEPart[1]>0 )	cu=m_LTEWiFiChannelUtilizationLTEPart[0]/m_LTEWiFiChannelUtilizationLTEPart[1];
		//else	cu=0.0;
		//double cu2;
		//if( !m_LTEWiFiChannelUtilizationWiFiPart.empty() && m_LTEWiFiChannelUtilizationWiFiPart[1]>0 )	cu2=m_LTEWiFiChannelUtilizationWiFiPart[0]/m_LTEWiFiChannelUtilizationWiFiPart[1];
		//else	cu2=0.0;
		//os << "Average LTEWiFi UE Channel Utilization:\t" << cu << "\t" << cu2 << endl;
		os << endl;

#pragma endregion

		os << "LTEChannelUtilization: " <<  std::accumulate(m_LTECU.begin(),m_LTECU.end(),0.0) / m_LTECU.size() << endl;
		os << "WiFiChannelUtilization: " <<  std::accumulate(m_WiFiCU.begin(),m_WiFiCU.end(),0.0) / m_WiFiCU.size() << endl << endl;

		os << "End of " << m_title << endl << endl << endl << endl;
	}

	//void RunResult::WriteWiFiTo( std::ostream& os ) const
	//{
	//	os << m_title << endl << endl;
	//	double dWiFiUESumThr = std::accumulate(m_WiFiUEThroughputVec.begin(), m_WiFiUEThroughputVec.end(), 0.0);
	//	os << "Total Throughput:\t" << dWiFiUESumThr << " Kbps"  << endl;
	//	os << "Average UE Throughput:\t" << dWiFiUESumThr / m_WiFiUEThroughputVec.size() << " Kbps"  << endl;
	//	os << "Average WiFi Cell Throught:\t" << dWiFiUESumThr / 2 << " Kbps"  << endl; //2¸öDrop
	//	os << "End of " << m_title << endl << endl << endl << endl;
	//}
}
