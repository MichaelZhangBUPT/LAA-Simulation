#include "commondef.h"
#include <boost/lambda/lambda.hpp>
#include <iomanip>
#include <string>
#include <numeric>
#include <boost/foreach.hpp>
#include "SystemResult.h"
#include "mcs.h"

namespace LTESim
{
	SystemResult::SystemResult(void)
	{
	}

	SystemResult::~SystemResult(void)
	{
	}

	//数值传递
	void SystemResult::SetTitle(const std::string& title)
	{
		m_title  = title;
	};

	const std::string& SystemResult::GetTitle() const
	{
		return m_title;
	}

	//以ms为单位的统计时长
	void SystemResult::SetSimDuration(double duration)
	{
		m_simduration = duration;
	}

	//void SystemResult::SetWiFiSimDuration( double duration )
	//{
	//	m_dWiFiSimDuration = duration;
	//}

	void SystemResult::AddUEThr( const vector<double>& uethr )
	{
		using namespace boost::lambda;
		assert(m_uethr.empty());
		m_uethr.resize(uethr.size());	//若应用m_uethr,则先为其开辟空间
		transform(uethr.begin(), uethr.end(), m_uethr.begin(), _1 / m_simduration);	//存储单位时间的吞吐量
	}

	void SystemResult::AddNBThr( const vector<double>& nbthr )
	{
		using namespace boost::lambda;
		assert(m_nbthr.empty());
		m_nbthr.resize(nbthr.size());	//若应用m_nbthr,则先为其开辟空间
		transform(nbthr.begin(), nbthr.end(), m_nbthr.begin(), _1 / m_simduration);	//存储单位时间的吞吐量
	}

    void SystemResult::AddNBMCS( const vector<vector<double> >& nbmcs )
    {
        m_nbmcs = nbmcs;
	}

    void SystemResult::AddHSPANBMCS( const vector<vector<double> >& HSPAnbmcs )
    {
        m_HSPAnbmcs = HSPAnbmcs;
	}

	void SystemResult::AddHSPAUEThr(const vector<double>& rHSPAUEThrVec )
	{
		m_HSPAUEThrVec.clear();
		for ( std::vector<double>::const_iterator vecIter = rHSPAUEThrVec.begin(); vecIter != rHSPAUEThrVec.end(); ++vecIter )
		{
			m_HSPAUEThrVec.push_back( *vecIter / m_simduration );
		}
	}	
	void SystemResult::AddHSPANBThr( const vector<double>& rHSPANBThrVec )
	{
		m_HSPANBThrVec.clear();
		for ( std::vector<double>::const_iterator vecIter = rHSPANBThrVec.begin(); vecIter != rHSPANBThrVec.end(); ++vecIter )
		{
			m_HSPANBThrVec.push_back( *vecIter / m_simduration );
		}
	}


	void SystemResult::AddWiFiTransInterval( const vector<double>& time )
	{
		m_WiFiTransInterval = time;
	}

	void SystemResult::AddLAATransInterval( const vector<double>& time )
	{
		m_LAATransInterval = time;
	}

	void SystemResult::AddLTETransInterval( const vector<double>& time )
	{
		m_LTETransInterval = time;
	}

	void SystemResult::AddLTEPacketTime( const vector<double>& uetime, const vector<double>& alltime )
	{
		m_LTEUEPacketTime = uetime;
		m_LTEPacketTime = alltime;
	}

	void SystemResult::AddWiFiPacketTime( const vector<double>& uetime, const vector<double>& alltime )
	{
		m_WiFiUEPacketTime = uetime;		
		m_WiFiPacketTime = alltime;
	}

	void SystemResult::AddLAAPacketTime( const vector<double>& uetime, const vector<double>& alltime )
	{
		m_LAAUEPacketTime = uetime;		
		m_LAAPacketTime = alltime;
	}

	void SystemResult::AddLTEWiFiPacketTime( const vector<double>& uetime, const vector<double>& alltime, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			{
				m_LTEWiFiUEPacketTimeLTEPart = uetime;
				m_LTEWiFiPacketTimeLTEPart = alltime;
			}
			break;
		case SYSTEM_WIFI:
			{
				m_LTEWiFiUEPacketTimeWiFiPart = uetime;
				m_LTEWiFiPacketTimeWiFiPart = alltime;
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

	void SystemResult::AddLTEPacketLoss(const std::vector<double> &ueloss, const vector<double>& loss)
	{
		m_LTEUEPacketLoss = ueloss;
		m_LTEPacketLoss = loss;
	}

	void SystemResult::AddWiFiPacketLoss(const std::vector<double> &ueloss, const vector<double>& loss)
	{
		m_WiFiUEPacketLoss = ueloss;
		m_WiFiPacketLoss = loss;
	}

	void SystemResult::AddLAAPacketLoss(const std::vector<double> &ueloss, const vector<double>& loss)
	{
		m_LAAUEPacketLoss = ueloss;
		m_LAAPacketLoss = loss;
	}

	void SystemResult::AddLTEWiFiPacketLoss( const vector<double>& ueloss, const vector<double>& loss, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			{
				m_LTEWiFiUEPacketLossLTEPart = ueloss;
				m_LTEWiFiPacketLossLTEPart = loss;
			}
			break;
		case SYSTEM_WIFI:
			{
				m_LTEWiFiUEPacketLossWiFiPart = ueloss;
				m_LTEWiFiPacketLossWiFiPart = loss;
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

	void SystemResult::AddLTEChannelUtilization(const std::vector<double> &uecu, const vector<double>& cu)
	{
		m_LTEUEChannelUtilization = uecu;
		m_LTEChannelUtilization = cu;
	}

	void SystemResult::AddWiFiChannelUtilization( const std::vector<double> &uecu, const vector<double>& cu )
	{
		m_WiFiUEChannelUtilization = uecu;
		m_WiFiChannelUtilization = cu;
	}

	void SystemResult::AddLTEWiFiChannelUtilization( const vector<double>& uecu, const vector<double>& cu, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			{
				m_LTEWiFiUEChannelUtilizationLTEPart = uecu;
				m_LTEWiFiChannelUtilizationLTEPart = cu;
			}
			break;
		case SYSTEM_WIFI:
			{
				m_LTEWiFiUEChannelUtilizationWiFiPart = uecu;
				m_LTEWiFiChannelUtilizationWiFiPart = cu;
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

	void SystemResult::AddChannelUtilization(double lte, double wifi)
	{
		m_ChannelUtilization.push_back(lte);
		m_ChannelUtilization.push_back(wifi);
	}

	void SystemResult::AddDualModeUEThr( const vector<double>& rDualModeUEThrVec, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			for ( std::vector<double>::const_iterator vecIter = rDualModeUEThrVec.begin(); vecIter != rDualModeUEThrVec.end(); ++vecIter )
				m_DualModeUELTEPartThrVec.push_back( *vecIter / m_simduration );
			break;
		case SYSTEM_HSPA:
			for ( std::vector<double>::const_iterator vecIter = rDualModeUEThrVec.begin(); vecIter != rDualModeUEThrVec.end(); ++vecIter )
				m_DualModeUEHSPAPartThrVec.push_back( *vecIter / m_simduration );
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	void SystemResult::AddWiFiUEThr( const vector<double>& rWiFiUEThrVec )
	{
		m_WiFiUEThrVec.clear();
		for ( std::vector<double>::const_iterator vecIter = rWiFiUEThrVec.begin(); vecIter != rWiFiUEThrVec.end(); ++vecIter )
		{
			m_WiFiUEThrVec.push_back( *vecIter / m_simduration );
		}
	}

	void SystemResult::AddLAAUEThr( const vector<double>& rLAAUEThrVec )
	{
		m_LAAUEThrVec.clear();
		for ( std::vector<double>::const_iterator vecIter = rLAAUEThrVec.begin(); vecIter != rLAAUEThrVec.end(); ++vecIter )
		{
			m_LAAUEThrVec.push_back( *vecIter / m_simduration );
		}
	}

	void SystemResult::AddWiFiCollisionNum( const vector<int>& collisionnum )
	{
		m_WiFiCollisionNum.insert(m_WiFiCollisionNum.end(), collisionnum.begin(), collisionnum.end());
	}

	void SystemResult::AddLAACollisionNum( const vector<int>& collisionnum )
	{
		m_LAACollisionNum.insert(m_LAACollisionNum.end(), collisionnum.begin(), collisionnum.end());
	}

	void SystemResult::AddLTEWiFiUEThr( const vector<double>& rLTEWiFiUEThrVec, SystemType systemType )
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			for ( std::vector<double>::const_iterator vecIter = rLTEWiFiUEThrVec.begin(); vecIter != rLTEWiFiUEThrVec.end(); ++vecIter )
				m_LTEWiFiUELTEPartThrVec.push_back( *vecIter / m_simduration );
			break;
		case SYSTEM_WIFI:
			for ( std::vector<double>::const_iterator vecIter = rLTEWiFiUEThrVec.begin(); vecIter != rLTEWiFiUEThrVec.end(); ++vecIter )
				m_LTEWiFiUEWiFiPartThrVec.push_back( *vecIter / m_simduration );
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	//获得UE的吞吐量
	const std::vector<double>& SystemResult::GetUEThroughput() const
	{
		return m_uethr;
	};

	//获得NodeB的吞吐量
	const std::vector<double>& SystemResult::GetNBThroughput() const
	{
		return m_nbthr;
	};

	const std::vector<double>& SystemResult::GetHSPAUEThroughput() const
	{
		return m_HSPAUEThrVec;
	}

	const std::vector<double>& SystemResult::GetHSPANBThrougput() const
	{
		return m_HSPANBThrVec;
	}

	const std::vector<double>& SystemResult::GetDualModeUEThroughput( SystemType systemType ) const
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			return m_DualModeUELTEPartThrVec;
		case SYSTEM_HSPA:
			return m_DualModeUEHSPAPartThrVec;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	const std::vector<double>& SystemResult::GetWiFiUEThroughput() const
	{
		return m_WiFiUEThrVec;
	}

	const std::vector<double>& SystemResult::GetLAAUEThroughput() const
	{
		return m_LAAUEThrVec;
	}

	const std::vector<double>& SystemResult::GetLTEWiFiUEThroughput( SystemType systemType ) const
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			return m_LTEWiFiUELTEPartThrVec;
		case SYSTEM_WIFI:
			return m_LTEWiFiUEWiFiPartThrVec;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	const std::vector<double>& SystemResult::GetLTEPacketTime() const
	{
		return m_LTEPacketTime;
	}

	const std::vector<double>& SystemResult::GetWiFiPacketTime() const
	{
		return m_WiFiPacketTime;
	}

	const std::vector<double>& SystemResult::GetLAAPacketTime() const
	{
		return m_LAAPacketTime;
	}

	const std::vector<double>& SystemResult::GetLTEWiFiPacketTime( SystemType systemType ) const
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			return m_LTEWiFiPacketTimeLTEPart;
			break;
		case SYSTEM_WIFI:
			return m_LTEWiFiPacketTimeWiFiPart;
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	const std::vector<double>& SystemResult::GetLTEPacketLoss() const
	{
		return m_LTEPacketLoss;
	}

	const std::vector<double>& SystemResult::GetWiFiPacketLoss() const
	{
		return m_WiFiPacketLoss;
	}

	const std::vector<double>& SystemResult::GetLAAPacketLoss() const
	{
		return m_LAAPacketLoss;
	}

	const std::vector<double>& SystemResult::GetLTEWiFiPacketLoss( SystemType systemType ) const
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			return m_LTEWiFiPacketLossLTEPart;
			break;
		case SYSTEM_WIFI:
			return m_LTEWiFiPacketLossWiFiPart;
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	const std::vector<double>& SystemResult::GetLTEChannelUtilization() const
	{
		return m_LTEChannelUtilization;
	}

	const std::vector<double>& SystemResult::GetWiFiChannelUtilization() const
	{
		return m_WiFiChannelUtilization;
	}

	const std::vector<double>& SystemResult::GetLTEWiFiChannelUtilization( SystemType systemType ) const
	{
		switch( systemType )
		{
		case SYSTEM_LTE:
			return m_LTEWiFiChannelUtilizationLTEPart;
			break;
		case SYSTEM_WIFI:
			return m_LTEWiFiChannelUtilizationWiFiPart;
			break;
		case SYSTEM_DUAL:
			throw UnFinishedFunc();
			break;
		default:
			throw std::invalid_argument( "Invalid System Type" );
			break;
		}
	}

	const std::vector<double>& SystemResult::GetChannelUtilization()const
	{
		return m_ChannelUtilization;
	}

	const std::vector<int>& SystemResult::GetWiFiCollisionNum() const
	{
		return m_WiFiCollisionNum;
	}

	const std::vector<int>& SystemResult::GetLAACollisionNum() const
	{
		return m_LAACollisionNum;
	}


	void SystemResult::WriteTo( std::ostream& os ) const
	{
		os << m_title << endl << endl;

#pragma region LTE Result

		int iLTEUEID = 0;
		os << "UEID:" << std::setw(10) << "Kbps"  << std::setw(14) << "PacketTime" << std::setw(14) << "PacketLoss" /*<< std::setw(14) << "ChannelUtilization" */<< endl;
		//输出每个UE的吞吐量
		std::vector<double>::const_iterator i = m_uethr.begin();
		std::vector<double>::const_iterator iPT = m_LTEUEPacketTime.begin();
		std::vector<double>::const_iterator iPL = m_LTEUEPacketLoss.begin();
		//std::vector<double>::const_iterator iCU = m_LTEUEChannelUtilization.begin();
		for ( ; i != m_uethr.end(); ++i,++iPT,++iPL/*,++iCU*/ )
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << iLTEUEID 
				<< std::resetiosflags( std::ios::left) << std::setw(10)<< *i 
				<< std::resetiosflags( std::ios::left) << std::setw(14)<< *iPT
				<< std::resetiosflags( std::ios::left) << std::setw(14)<< *iPL
				//<< std::resetiosflags( std::ios::left) << std::setw(14)<< *iCU 
				<<endl; 
				iLTEUEID++;
		}
		os << endl;
		double loss;
		os << "LTEPacketTimeAverage: " << std::accumulate(m_LTEPacketTime.begin(),m_LTEPacketTime.end(),0.0)/m_LTEPacketTime.size() << endl;
		if(!m_LTEPacketLoss.empty() && m_LTEPacketLoss[1]>0)	loss=m_LTEPacketLoss[0]/m_LTEPacketLoss[1];
		else	loss=0.0;
		os << "LTEPacketLossAverage: " << loss << endl;
		//double cu;
		//if( !m_LTEChannelUtilization.empty() && m_LTEChannelUtilization[1]>0 )	cu=m_LTEChannelUtilization[0]/m_LTEChannelUtilization[1];
		//else	cu=0.0;
		//os << "LTEChannelUtilizationAverage: " << cu << endl;
		os << endl << endl;
		os << "NBID:" << std::setw(10) << "Kbps" << endl;
		int NBID = 0;
		//输出每个NodeB的吞吐量
		for (std::vector<double>::const_iterator i = m_nbthr.begin(); i != m_nbthr.end(); ++i, ++NBID)
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << "NodeB " << NBID << ':' 
				<<  std::resetiosflags( std::ios::left) << std::setw(10)<< *i << endl;
		}
		os << endl;

		double SumUEThr = std::accumulate(m_uethr.begin(), m_uethr.end(), 0.0);
		os << "Total LTE Throughput:\t" << SumUEThr << " Kbps" << endl;										//UE的总吞吐量
		os << "Average LTE UE Throughput:\t" << SumUEThr / m_uethr.size() << " Kbps" << endl;	//UE的平均吞吐量

		double SumCellThr = std::accumulate(m_nbthr.begin(), m_nbthr.end(), 0.0);
		os << "Average LTE Cell Throughput:\t" << SumCellThr / m_nbthr.size() << " Kbps" << endl;	//NodeB的平均吞吐量
		os << endl;
		//assert(SumCellThr - SumUEThr < 0.01);																		//NodeB的总吞吐量应该等于UE的总吞吐量

		//输出每个NodeB的MCS等级个数 YRM
		os << endl;
		os << "Statistic of MCS" << endl;
		NBID = 0;
		os << std::setiosflags( std::ios::left ) << std::setw(5) << "NBID:"
			<< std::resetiosflags( std::ios::left) << std::setw(14) << "  QPSk/RATE_1_5"
			<< std::resetiosflags( std::ios::left) << std::setw(14) << "  QPSk/RATE_1_3"
			<< std::resetiosflags( std::ios::left) << std::setw(14) << "  QPSk/RATE_1_2" 
			<< std::resetiosflags( std::ios::left) << std::setw(14) << "  QPSk/RATE_2_3" 
			<< std::resetiosflags( std::ios::left) << std::setw(14) << "  QPSk/RATE_3_4"
			<< std::resetiosflags( std::ios::left) << std::setw(14) << "  QAM16/RATE_1_2" 
			<< std::resetiosflags( std::ios::left) << std::setw(14) << " QAM16/RATE_2_3" 
			<< std::resetiosflags( std::ios::left) << std::setw(14) << " QAM16/RATE_3_4" 
			<< std::resetiosflags( std::ios::left) << std::setw(14) << " QAM16/RATE_4_5" 
			<< std::resetiosflags( std::ios::left) << std::setw(14) << " QAM64/RATE_3_5"
			<< std::resetiosflags( std::ios::left) << std::setw(14) << " QAM64/RATE_2_3" 
			<< std::resetiosflags( std::ios::left) << std::setw(14) << " QAM64/RATE_4_5" << endl;

		vector<size_t> SysMCS(MCS::GetMCSLevels( SYSTEM_LTE ), 0);
		//计算总的MCS数目
		for (std::vector<vector<double> >::const_iterator i = m_nbmcs.begin(); i != m_nbmcs.end(); ++i, ++NBID)
		{
			int colindex = 0;
			for (vector<size_t>::iterator itrSysMCS = SysMCS.begin() ; itrSysMCS != SysMCS.end() ; ++itrSysMCS)
			{
				*itrSysMCS += (size_t)  (*i)[colindex];
				++colindex;
			}
		}
		NBID = 0;
		for (std::vector<vector<double> >::const_iterator m = m_nbmcs.begin(); m != m_nbmcs.end(); ++m, ++NBID)
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << NBID; 
			size_t TotalSelectedMCS = std::accumulate(m->begin(), m->end(), 0.0);
			for (size_t n = 0 ; n < m->size(); ++n)
			{
				os << std::resetiosflags( std::ios::left) << std::setw(8) << (*m)[n] << "(" 
					<<  (TotalSelectedMCS == 0 ? 0 : (100.0 * (*m)[n] / TotalSelectedMCS ) )
					<< "%)" ;
			}
			os << endl;
		}
		os << std::setiosflags( std::ios::left ) << std::setw(5) << "Sum";
		double TotalSelectedMCS = std::accumulate(SysMCS.begin(), SysMCS.end(), 0u);
		BOOST_FOREACH(size_t eachMCS, SysMCS)
		{
			os << std::resetiosflags( std::ios::left) << std::setw(15) << eachMCS
				<< "(" << ( eachMCS == 0 ? 0 :  eachMCS / TotalSelectedMCS * 100) << "%)"
				<< std::resetiosflags( std::ios::left) << std::setw(15);
		}
		os << endl;
		os << endl << endl;

#pragma endregion

#pragma region HSPA Result

		os << std::setiosflags( std::ios::left ) << "HSPAUEID:" << std::setw(10) << "Kbps" <<endl;
		int iHSPAUEID = 0;
		//std::vector<double>::const_iterator TimevecIter = m_HSPAPacketTime.begin();
		for ( std::vector<double>::const_iterator vecIter = m_HSPAUEThrVec.begin(); vecIter != m_HSPAUEThrVec.end(); ++vecIter )
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << iHSPAUEID 
				<< std::resetiosflags( std::ios::left) << std::setw(10) << *vecIter << endl;
			iHSPAUEID++;
			//TimevecIter++;
		}
		os << endl << endl;

		os << std::setiosflags( std::ios::left ) << "HSPANBID:" << std::setw(10) << "Kbps" << endl;
		int iHSPANBID = 0;
		for ( std::vector<double>::const_iterator vecIter = m_HSPANBThrVec.begin(); vecIter != m_HSPANBThrVec.end(); ++vecIter )
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << "NodeB " << iHSPANBID << ':' 
				<<  std::resetiosflags( std::ios::left) << std::setw(10)<< *vecIter << endl;
			iHSPANBID++;
		}
		os << endl << endl;

		double dHSPAUESumThr = std::accumulate( m_HSPAUEThrVec.begin(), m_HSPAUEThrVec.end(), 0.0 );
		os << "HSPA UE Total Throughput:\t" << dHSPAUESumThr << " Kbps" << endl;															//HSPA UE的总吞吐量
		os << "Average HSPA UE Throughput:\t" << dHSPAUESumThr / m_HSPAUEThrVec.size() << " Kbps" << endl;			//HSPA UE的平均吞吐量

		double dHSPANodeBSumThr = std::accumulate( m_HSPANBThrVec.begin(), m_HSPANBThrVec.end(), 0.0 );				//HSPA NodeB的总吞吐量
		os << "Average HSPA Cell Throughput:\t" << dHSPANodeBSumThr / m_HSPANBThrVec.size() << " Kbps" << endl;										//HSPA NodeB的平均吞吐量
		os << endl << endl << endl;

		//输出每个HSPA NodeB的MCS等级个数
		os << endl;
		os << "Statistic of MCS" << endl;
		NBID = 0;
		os << std::setiosflags( std::ios::left ) << std::setw(5) << "NBID:"
			<< "  QPSk/RATE_1_3" << "  QPSk/RATE_1_2" << "  QPSk/RATE_2_3" 
			<< "  QPSk/RATE_4_5" << "  QAM16/RATE_1_2" << " QAM16/RATE_2_3" 
			<< " QAM16/RATE_4_5" << " QAM64/RATE_1_3" << " QAM64/RATE_1_2"
			<< " QAM64/RATE_2_3" << " QAM64/RATE_4_5" << endl;

		vector<size_t> HSPASysMCS(MCS::GetMCSLevels( SYSTEM_HSPA ), 0);
		//计算总的MCS数目
		for (std::vector<vector<double> >::const_iterator i = m_HSPAnbmcs.begin(); i != m_HSPAnbmcs.end(); ++i, ++NBID)
		{
			int colindex = 0;
			for (vector<size_t>::iterator itrSysMCS = HSPASysMCS.begin() ; itrSysMCS != HSPASysMCS.end() ; ++itrSysMCS)
			{
				*itrSysMCS += (size_t)  (*i)[colindex];
				++colindex;
			}
		}
		NBID = 0;
		for (std::vector<vector<double> >::const_iterator m = m_HSPAnbmcs.begin(); m != m_HSPAnbmcs.end(); ++m, ++NBID)
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << NBID; 
			size_t TotalSelectedMCS = std::accumulate(m->begin(), m->end(), 0.0);
			for (size_t n = 0 ; n < m->size(); ++n)
			{
				os << std::resetiosflags( std::ios::left) << std::setw(8) << (*m)[n] << "(" 
					<< std::setw(7) << (TotalSelectedMCS == 0 ? 0 : (100.0 * (*m)[n] / TotalSelectedMCS ) )
					<< "%)" ;
			}
			os << endl;
		}
		os << std::setiosflags( std::ios::left ) << std::setw(5) << "Sum";
		double HSPATotalSelectedMCS = std::accumulate(HSPASysMCS.begin(), HSPASysMCS.end(), 0u);
		BOOST_FOREACH(size_t eachMCS, HSPASysMCS)
		{
			os << std::resetiosflags( std::ios::left) << std::setw(9) << eachMCS
				<< "(" << ( eachMCS == 0 ? 0 :  eachMCS / HSPATotalSelectedMCS * 100) << "%)"
				<< std::resetiosflags( std::ios::left);
		}
		os << endl;
		os << endl << endl;


#pragma endregion

#pragma region WiFi Result

		os << std::setiosflags( std::ios::left ) << "WiFiUEID:" << std::setw(10) << "Kbps" << std::setw(14) << "PacketTime" << std::setw(14) << "PacketLoss" /*<< std::setw(14) << "ChannelUtilization" */<< endl;
		int iWiFiUEID = 0;
		std::vector<double>::const_iterator i1 = m_WiFiUEThrVec.begin();
		std::vector<double>::const_iterator iWPT = m_WiFiUEPacketTime.begin();
		std::vector<double>::const_iterator iWPL = m_WiFiUEPacketLoss.begin();
		//std::vector<double>::const_iterator iWCU = m_WiFiUEChannelUtilization.begin();
		for ( ; i1 != m_WiFiUEThrVec.end(); ++i1,++iWPT,++iWPL/*,++iWCU*/ )
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << iWiFiUEID 
				<< std::resetiosflags( std::ios::left) << std::setw(10) << *i1 
				<< std::resetiosflags( std::ios::left) << std::setw(14) << *iWPT 
				<< std::resetiosflags( std::ios::left) << std::setw(14) << *iWPL
				//<< std::resetiosflags( std::ios::left) << std::setw(14) << *iWCU
				<< endl;
			iWiFiUEID++;
		}
		os << endl ;
		os << "WiFiPacketTimeAverage: " << std::accumulate(m_WiFiPacketTime.begin(),m_WiFiPacketTime.end(),0.0)/m_WiFiPacketTime.size() << endl;
		if(!m_WiFiPacketLoss.empty() && m_WiFiPacketLoss[1]>0)	loss=m_WiFiPacketLoss[0]/m_WiFiPacketLoss[1];
		else	loss=0.0;
		os << "WiFiPacketLossAverage: " << loss << endl;
		//if( !m_WiFiChannelUtilization.empty() && m_WiFiChannelUtilization[1]>0 )	cu=m_WiFiChannelUtilization[0]/m_WiFiChannelUtilization[1];
		//else	cu=0.0;
		//os << "WiFiChannelUtilizationAverage: " << cu << endl;
		os << "WiFiAPID:         0\t1\t2\t3\tAverage" << endl;
		os << "WiFiCollisionNum: ";
		std::vector<int>::const_iterator it = m_WiFiCollisionNum.begin();
		for ( ; it != m_WiFiCollisionNum.end(); ++it )
		{
			os << *it <<"\t";
		}
		os << std::accumulate(m_WiFiCollisionNum.begin(),m_WiFiCollisionNum.end(),0.0)/m_WiFiCollisionNum.size() << endl;

		double dWiFiUESumThr = std::accumulate( m_WiFiUEThrVec.begin(), m_WiFiUEThrVec.end(), 0.0 );
		os << "WiFi UE Total Throughput:\t" << dWiFiUESumThr << " Kbps" << endl;						
		os << "Average WiFi Cell Throughput:\t" << dWiFiUESumThr / 4 << " Kbps" << endl;	
		os << "Average WiFi UE Throughput:\t" << dWiFiUESumThr / m_WiFiUEThrVec.size() << " Kbps" << endl;		
		os << endl << endl;

#pragma endregion

		
#pragma region LAA Result

		os << std::setiosflags( std::ios::left ) << "LAAUEID:" << std::setw(10) << "Kbps" << std::setw(14) << "PacketTime" << std::setw(14) << "PacketLoss" /*<< std::setw(14) << "ChannelUtilization" */<< endl;
		int iLAAUEID = 0;
		std::vector<double>::const_iterator iL1 = m_LAAUEThrVec.begin();
		std::vector<double>::const_iterator iLPT = m_LAAUEPacketTime.begin();
		std::vector<double>::const_iterator iLPL = m_LAAUEPacketLoss.begin();
		//std::vector<double>::const_iterator iWCU = m_WiFiUEChannelUtilization.begin();
		for ( ; iL1 != m_LAAUEThrVec.end(); ++iL1,++iLPT,++iLPL/*,++iWCU*/ )
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << iLAAUEID 
				<< std::resetiosflags( std::ios::left) << std::setw(10) << *iL1 
				<< std::resetiosflags( std::ios::left) << std::setw(14) << *iLPT 
				<< std::resetiosflags( std::ios::left) << std::setw(14) << *iLPL
				//<< std::resetiosflags( std::ios::left) << std::setw(14) << *iWCU
				<< endl;
			iLAAUEID++;
		}
		os << endl ;
		os << "LAAPacketTimeAverage: " << std::accumulate(m_LAAPacketTime.begin(),m_LAAPacketTime.end(),0.0)/m_LAAPacketTime.size() << endl;
		if(!m_LAAPacketLoss.empty() && m_LAAPacketLoss[1]>0)	loss=m_LAAPacketLoss[0]/m_LAAPacketLoss[1];
		else	loss=0.0;
		os << "LAAPacketLossAverage: " << loss << endl;
		//if( !m_WiFiChannelUtilization.empty() && m_WiFiChannelUtilization[1]>0 )	cu=m_WiFiChannelUtilization[0]/m_WiFiChannelUtilization[1];
		//else	cu=0.0;
		//os << "WiFiChannelUtilizationAverage: " << cu << endl;
		os << "LAANBID:         0\t1\t2\t3\tAverage" << endl;
		os << "LAACollisionNum: ";
		std::vector<int>::const_iterator iLt = m_LAACollisionNum.begin();
		for ( ; iLt != m_LAACollisionNum.end(); ++iLt )
		{
			os << *iLt <<"\t";
		}
		os << std::accumulate(m_LAACollisionNum.begin(),m_LAACollisionNum.end(),0.0)/m_LAACollisionNum.size() << endl;

		double dLAAUESumThr = std::accumulate( m_LAAUEThrVec.begin(), m_LAAUEThrVec.end(), 0.0 );
		os << "LAA UE Total Throughput:\t" << dLAAUESumThr << " Kbps" << endl;						
		os << "Average LAA Cell Throughput:\t" << dLAAUESumThr /4 << " Kbps" << endl;	
		os << "Average LAA UE Throughput:\t" << dLAAUESumThr / m_LAAUEThrVec.size() << " Kbps" << endl;		
		os << endl << endl;

#pragma endregion

#pragma region DualMode Result

		os << "DualModeUEID:" << std::setw(15) << "LTE_Thr(Kbps)" << std::setw(15) << "HSPA_Thr(Kbps)" << endl;
		int iDualModeUEID = 0;
		std::vector<double>::const_iterator vecIterLTEPart = m_DualModeUELTEPartThrVec.begin();
		std::vector<double>::const_iterator vecIterHSPAPart = m_DualModeUEHSPAPartThrVec.begin();
		for (	;vecIterLTEPart != m_DualModeUELTEPartThrVec.end() && vecIterHSPAPart != m_DualModeUEHSPAPartThrVec.end(); ++vecIterLTEPart, ++vecIterHSPAPart )
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << iDualModeUEID 
				<< std::setw(10)<< *vecIterLTEPart 
				<< std::setw(10)<< *vecIterHSPAPart  << endl;
			iDualModeUEID++;
		}
		os << endl << endl;

		double dDualModeUELTEPartSumThr = std::accumulate(m_DualModeUELTEPartThrVec.begin(), m_DualModeUELTEPartThrVec.end(), 0.0);
		double dDualModeUEHSPAPartSumThr = std::accumulate(m_DualModeUEHSPAPartThrVec.begin(), m_DualModeUEHSPAPartThrVec.end(), 0.0);
		os << "DualModeUESumThroughput" << std::setw(15) << "LTE_Thr(Kbps)" << std::setw(15) << "HSPA_Thr(Kbps)" << endl;
		os << "DualModeUESumThroughput"
			<< std::setw(15) << dDualModeUELTEPartSumThr
			<< std::setw(15) << dDualModeUEHSPAPartSumThr
			<< endl;
		os << "DualModeUEAverage" << std::setw(15) << "LTE_Thr(Kbps)" << std::setw(15) << "HSPA_Thr(Kbps)" << endl;
		os << "DualModeUEAverage"
			<< std::setw(15) << dDualModeUELTEPartSumThr / m_DualModeUELTEPartThrVec.size()
			<< std::setw(15) << dDualModeUEHSPAPartSumThr / m_DualModeUEHSPAPartThrVec.size()
			<< endl;
		os << endl << endl;

#pragma endregion

#pragma region LTEWiFi Result

		os << "LTEWiFiUEID:" << std::setw(14) << "Thr(Kbps)" << std::setw(20) << "PacketTime" << std::setw(19) << "PacketLoss" /*<< std::setw(30) << "ChannelUtilization" */<< endl;
		int iLTEWiFiUEID = 0;
		std::vector<double>::const_iterator iterLTEPart = m_LTEWiFiUELTEPartThrVec.begin();
		std::vector<double>::const_iterator vecIterWiFiPart = m_LTEWiFiUEWiFiPartThrVec.begin();
		std::vector<double>::const_iterator iLWPTL = m_LTEWiFiUEPacketTimeLTEPart.begin();
		std::vector<double>::const_iterator iLWPLL = m_LTEWiFiUEPacketLossLTEPart.begin();
		//std::vector<double>::const_iterator iLWCUL = m_LTEWiFiUEChannelUtilizationLTEPart.begin();
		std::vector<double>::const_iterator iLWPTW = m_LTEWiFiUEPacketTimeWiFiPart.begin();
		std::vector<double>::const_iterator iLWPLW = m_LTEWiFiUEPacketLossWiFiPart.begin();
		//std::vector<double>::const_iterator iLWCUW = m_LTEWiFiUEChannelUtilizationWiFiPart.begin();
		for (	;iterLTEPart != m_LTEWiFiUELTEPartThrVec.end() && vecIterWiFiPart != m_LTEWiFiUEWiFiPartThrVec.end(); )
		{
			os << std::setiosflags( std::ios::left ) << std::setw(5) << iLTEWiFiUEID 
				<< std::setw(10)<< *iterLTEPart << std::setw(10)<< *vecIterWiFiPart  
				<< std::setw(10)<< *iLWPTL << std::setw(10)<< *iLWPTW  
				<< std::setw(10)<< *iLWPLL << std::setw(10)<< *iLWPLW  
				//<< std::setw(10)<< *iLWCUL << std::setw(10)<< *iLWCUW  
				<< endl;
			iLTEWiFiUEID++;
			++iterLTEPart; ++vecIterWiFiPart; ++iLWPTL; ++iLWPTW; ++iLWPLL; ++iLWPLW;
		}
		os << endl;
		os << "LTEWiFiPacketTimeAverage: " << std::accumulate(m_LTEWiFiPacketTimeLTEPart.begin(),m_LTEWiFiPacketTimeLTEPart.end(),0.0)/m_LTEWiFiPacketTimeLTEPart.size() << " " 
											<< std::accumulate(m_LTEWiFiPacketTimeWiFiPart.begin(),m_LTEWiFiPacketTimeWiFiPart.end(),0.0)/m_LTEWiFiPacketTimeWiFiPart.size() << endl;
		if(!m_LTEWiFiPacketLossLTEPart.empty() && m_LTEWiFiPacketLossLTEPart[1]>0)	loss=m_LTEWiFiPacketLossLTEPart[0]/m_LTEWiFiPacketLossLTEPart[1];
		else	loss=0.0;
		double loss2;
		if(!m_LTEWiFiPacketLossWiFiPart.empty() && m_LTEWiFiPacketLossWiFiPart[1]>0)	loss2=m_LTEWiFiPacketLossWiFiPart[0]/m_LTEWiFiPacketLossWiFiPart[1];
		else	loss2=0.0;
		os << "LTEWiFiPacketLossAverage: " << loss << " " << loss2 << endl;
		//if( !m_LTEWiFiChannelUtilizationLTEPart.empty() && m_LTEWiFiChannelUtilizationLTEPart[1]>0 )	cu=m_LTEWiFiChannelUtilizationLTEPart[0]/m_LTEWiFiChannelUtilizationLTEPart[1];
		//else	cu=0.0;
		//double cu2;
		//if( !m_LTEWiFiChannelUtilizationWiFiPart.empty() && m_LTEWiFiChannelUtilizationWiFiPart[1]>0 )	cu2=m_LTEWiFiChannelUtilizationWiFiPart[0]/m_LTEWiFiChannelUtilizationWiFiPart[1];
		//else	cu2=0.0;
		//os << "LTEWiFiChannelUtilizationAverage: " << cu << " " << cu2 << endl<<endl;

		double dLTEWiFiUELTEPartSumThr = std::accumulate(m_LTEWiFiUELTEPartThrVec.begin(), m_LTEWiFiUELTEPartThrVec.end(), 0.0);
		double dLTEWiFiUEWiFiPartSumThr = std::accumulate(m_LTEWiFiUEWiFiPartThrVec.begin(), m_LTEWiFiUEWiFiPartThrVec.end(), 0.0);
		os << "\t\t\t" << std::setw(15) << "LTE_Thr(Kbps)" << std::setw(15) << "WiFi_Thr(Kbps)" << endl;
		os << "LTEWiFiUESumThroughput "
			<< std::setw(20) << dLTEWiFiUELTEPartSumThr
			<< std::setw(20) << dLTEWiFiUEWiFiPartSumThr
			<< endl;
		os << "LTEWiFiAverageCellThr "
			<< std::setw(20) << dLTEWiFiUELTEPartSumThr / m_nbthr.size()
			<< std::setw(20) << dLTEWiFiUEWiFiPartSumThr / m_nbthr.size()
			<< endl;
		os << "LTEWiFiUEAverage "
			<< std::setw(20) << dLTEWiFiUELTEPartSumThr / m_LTEWiFiUELTEPartThrVec.size()
			<< std::setw(20) << dLTEWiFiUEWiFiPartSumThr / m_LTEWiFiUEWiFiPartThrVec.size()
			<< endl;
		os << endl << endl;

#pragma endregion

		os << "LTEChannelUtilization: " << m_ChannelUtilization[0] << endl;
		os << "WiFiChannelUtilization: " << m_ChannelUtilization[1] << endl << endl;

		os << "End of " << m_title << endl << endl << endl << endl;
	}

	//void SystemResult::WriteWiFiTo( std::ostream& os ) const
	//{
	//	os << m_title << endl << endl;
	//	os << std::setiosflags( std::ios::left ) << "WiFiUEID:" << std::setw(10) << "Kbps" <<endl;
	//	int iWiFiUEID = 0;
	//	for ( std::vector<double>::const_iterator vecIter = m_WiFiUEThrVec.begin(); vecIter != m_WiFiUEThrVec.end(); ++vecIter )
	//	{
	//		os << std::setiosflags( std::ios::left ) << std::setw(5) << iWiFiUEID 
	//			<< std::resetiosflags( std::ios::left) << std::setw(10) << *vecIter << endl;
	//		iWiFiUEID++;
	//	}
	//	os << endl << endl;
	//	double dWiFiUESumThr = std::accumulate( m_WiFiUEThrVec.begin(), m_WiFiUEThrVec.end(), 0.0 );
	//	os << "WiFi UE Total Throughput:\t" << dWiFiUESumThr << " Kbps" << endl;						
	//	os << "Average WiFi UE Throughput:\t" << dWiFiUESumThr / m_WiFiUEThrVec.size() << " Kbps" << endl;		
	//	os << endl << endl;
	//}

	//重载“<<”，方便输出
	std::ostream& operator << (std::ostream& os, const SystemResult& result)
	{
		result.WriteTo(os);
		return os;
	}
}
