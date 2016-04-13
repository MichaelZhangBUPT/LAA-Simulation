#include "SimLog.h"
#include "commondef.h"
#include "Configuration.h"
#include "System.h"
#include "Run.h"
#include "POS.h"
#include "Pathloss.h"
#include "Fading.h"
#include "ClockManager.h"
#include "Clock.h"
#include "RunParameters.h"

#include <iomanip>
#include <fstream>
#include <iostream>


namespace LTESim
{
	using std::setw;
	using std::endl;

	SimLog * SimLog::m_Instance = NULL;

	SimLog * SimLog::Instance()
	{
		if( m_Instance == NULL )
			m_Instance = new SimLog();
		return m_Instance;
	}

	SimLog::SimLog()
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		m_bLogServiceBufferUpdate = pSystem->GetConfig().GetBool("Log_Service_Buffer_Update");
		m_bLogAccessInformation = pSystem->GetConfig().GetBool( "Log_Access_Information" );
		m_bLogCompositeServiceDistributeBuffer = pSystem->GetConfig().GetBool("Log_Composite_Service_Distribute_Buffer");
		m_bLogBufferEmpty = 0;
		m_bMakeDecisionCount = 0;
		m_bLogLTERatio = 1;
		m_bLogC2I = 0;
		m_bLogMaxTransTimeNum = 0;
		m_bLog = 0;
		m_bLogSINR = 0;
		m_FileIndex.insert( std::make_pair( -1,-1 ) );
		m_bLogDualTimeDifference = 1;
		m_bLogWiFi = 0;
		m_bLTECandidateServices = 0;
		m_bWiFiCandidateServices = 0;
		m_bThrAndCU = 0;
		m_bUESINR = 0;
	}

	SimLog::~SimLog()
	{
		if ( m_LogServiceBufferUpdate.is_open() )
		{
			m_LogServiceBufferUpdate.flush();
			m_LogServiceBufferUpdate.close();
		}
		if ( m_LogAccessInformation.is_open() )
		{
			m_LogAccessInformation.flush();
			m_LogAccessInformation.close();
		}
		if ( m_LogCompositeServiceDistributeBuffer.is_open() )
		{
			m_LogCompositeServiceDistributeBuffer.flush();
			m_LogCompositeServiceDistributeBuffer.close();
		}
		if ( m_LogBufferEmpty.is_open() )
		{
			m_LogBufferEmpty.flush();
			m_LogBufferEmpty.close();
		}
		if ( m_MakeDecisionCount.is_open() )
		{
			m_MakeDecisionCount.flush();
			m_MakeDecisionCount.close();
		}
		if ( m_LogLTERatio.is_open() )
		{
			m_LogLTERatio.flush();
			m_LogLTERatio.close();
		}
		if ( m_LogDualTimeDifference.is_open() )
		{
			m_LogDualTimeDifference.flush();
			m_LogDualTimeDifference.close();
		}

		if ( m_LogMaxTransTimeNum.is_open() )
		{
			m_LogMaxTransTimeNum.flush();
			m_LogMaxTransTimeNum.close();
		}
		if ( m_Log.is_open() )
		{
			m_Log.flush();
			m_Log.close();
		}
		for ( int i=0; i<50; i++ )
		{
			if ( m_LogC2IArray[i].is_open() )
			{
				m_LogC2IArray[i].flush();
				m_LogC2IArray[i].close();
			}
			if ( m_LogSINRArray[i].is_open() )
			{
				m_LogSINRArray[i].flush();
				m_LogSINRArray[i].close();
			}
		}
		if ( m_LogWiFi.is_open() )
		{
			m_LogWiFi.flush();
			m_LogWiFi.close();
		}
		if ( m_LogLTECandidateServices.is_open() )
		{
			m_LogLTECandidateServices.flush();
			m_LogLTECandidateServices.close();
		}
		if ( m_LogWiFiCandidateServices.is_open() )
		{
			m_LogWiFiCandidateServices.flush();
			m_LogWiFiCandidateServices.close();
		}
		if ( m_LogThrAndCU.is_open() )
		{
			m_LogThrAndCU.flush();
			m_LogThrAndCU.close();
		}
		if ( m_LogUESINR.is_open() )
		{
			m_LogUESINR.flush();
			m_LogUESINR.close();
		}

	}

	bool SimLog::LogServiceBufferUpdate(int iID, int iFrame, int iInterval, SystemType ueType )
	{
		if ( m_bLogServiceBufferUpdate == false )
			return false;

		if ( !m_LogServiceBufferUpdate.is_open() )
		{
			m_LogServiceBufferUpdate.open( "../Output/LogServiceBufferUpdate.txt", std::ios::trunc );
			m_LogServiceBufferUpdate << "UEID\tFrame\tInterval\tUEType" << std::endl;
		}
		m_LogServiceBufferUpdate	<< iID << "\t"
													<< iFrame << "\t"
													<< iInterval << "\t"
													<< ueType << "\t"
													<< std::endl;

		return true;
	}

	bool SimLog::LogAccessInformation( int iUEID, const POS& rUEPos, int iNBID, const POS& rNBPos, double dDistance )
	{
		if ( m_bLogAccessInformation == false )
			return false;

		if ( !m_LogAccessInformation.is_open() )
		{
			m_LogAccessInformation.open( "../Output/LogAccessInformation.txt", std::ios::trunc );
			m_LogAccessInformation << "UEID\tUEPos.X\tUEPos.Y\tNBID\tNBPos.X\tNBPos.Y\tDistance" << std::endl;
		}
		m_LogAccessInformation	<< iUEID << "\t"
												<< rUEPos.GetX() << "\t"
												<< rUEPos.GetY() << "\t"
												<< iNBID << "\t"
												<< rNBPos.GetX() << "\t"
												<< rNBPos.GetY() << "\t"
												<< dDistance << "\t"
												<< std::endl;

		return true;
	}

	bool SimLog::LogCompositeServiceDistributeBuffer(int iID, int iFrame, double dLTEBufferVol, double dHSPABufferVol)
	{
		if (!m_bLogCompositeServiceDistributeBuffer)
			return false;

		if (!m_LogCompositeServiceDistributeBuffer.is_open())
		{
			m_LogCompositeServiceDistributeBuffer.open( "../Output/LogCompositeServiceDistributeBuffer.txt", std::ios::trunc );
			m_LogCompositeServiceDistributeBuffer << "UEID\tFrame\tLTEData\tHSPAData" << std::endl;
		}
		m_LogCompositeServiceDistributeBuffer << iID << "\t"
																		<< iFrame << "\t"
																		<< dLTEBufferVol << "\t"
																		<< dHSPABufferVol << "\t"
																		<< std::endl;
		return true;
	}

	bool SimLog::LogBufferEmpty( int iID, int iFrame, SystemType ueType, SystemType serviceType )
	{
		if (!m_bLogBufferEmpty)
			return false;

		if (!m_LogBufferEmpty.is_open())
		{
			m_LogBufferEmpty.open( "../Output/LogBufferEmpty.txt", std::ios::trunc );
			m_LogBufferEmpty << "UEID\tFrame\tUEType\tServiceType" << std::endl;
		}
		m_LogBufferEmpty	<< iID << "\t"
										<< iFrame << "\t"
										<< ueType << "\t"
										<< serviceType << "\t"
										<< std::endl;

		return true;
	}

	bool SimLog::MakeDecisionCount( int iID, int iFrame, SystemType ueType, SystemType serviceType, bool bIsSuccess )
	{
		if (!m_bMakeDecisionCount)
			return false;

		if (!m_MakeDecisionCount.is_open())
		{
			m_MakeDecisionCount.open( "../Output/MakeDecisionCount.txt", std::ios::trunc );
			m_MakeDecisionCount << "UEID\tFrame\tUEType\tServiceType\tIsSuccess" << std::endl;
		}
		m_MakeDecisionCount	<< iID << "\t"
			<< iFrame << "\t"
			<< ueType << "\t"
			<< serviceType << "\t"
			<< bIsSuccess << "\t"
			<< std::endl;

		return true;
	}

	bool SimLog::LogLTERatio( int iID, int iFrame, double dLTERatio )
	{
		if (!m_bLogLTERatio)
			return false;

		if (!m_LogLTERatio.is_open())
		{
			m_LogLTERatio.open( "../Output/LogLTERatio.txt", std::ios::trunc );
			m_LogLTERatio << "UEID\tFrame\tdLTERatio" << std::endl;
		}
		m_LogLTERatio	<< iID << "\t"
			<< iFrame << "\t"
			<< dLTERatio << "\t"
			<< std::endl;

		return true;
	}

	bool SimLog::LogC2I( int iID, int iFrame, double dTime, double dC2I )
	{
		if (!m_bLogC2I)
			return false;

		int iArrayNo = 0;
		if ( m_FileIndex.find(iID) == m_FileIndex.end() )
		{
			iArrayNo = (--m_FileIndex.end())->second + 1;
			m_FileIndex.insert( std::make_pair( iID,iArrayNo ) );
		}
		else
			iArrayNo = m_FileIndex.find(iID) -> second;

		if ( !m_LogC2IArray[iArrayNo].is_open() )
		{
			std::stringstream ss;
			string s;
			ss << iID;
			ss >> s;
			m_LogC2IArray[iArrayNo].open( ("../Output/LogC2I_UEID_" + s + ".txt").c_str(), std::ios::trunc );
			m_LogC2IArray[iArrayNo] << "UEID\tFrame\tTime\tC2I" << std::endl;
		}
		m_LogC2IArray[iArrayNo] << iID << "\t" << iFrame << "\t" << dTime << "\t" << dC2I << std::endl;

		return true;
	}

	bool SimLog::LogMaxTransTimeNum( int iID, int iTime )
	{
		if (!m_bLogMaxTransTimeNum)
			return false;

		if (!m_LogMaxTransTimeNum.is_open())
		{
			m_LogMaxTransTimeNum.open( "../Output/LogMaxTransTimeNum.txt", std::ios::trunc );
			m_LogMaxTransTimeNum << "UEID\tMaxTransTimeNum" << std::endl;
		}
		m_LogMaxTransTimeNum << iID << "\t" << iTime << std::endl;

		return true;

	}

	bool SimLog::LogDualTimeDifference( int iID, double starttime, double lteendtime, double hspaendtime, double timedifference )
	{
		if ( !m_bLogDualTimeDifference )
			return false;

		if( !m_LogDualTimeDifference.is_open() )
		{
			m_LogDualTimeDifference.open( "../Output/LogDualTimeDifference.txt", std::ios::trunc );
			m_LogDualTimeDifference << "UEID\tStart\tLTEEnd\tWiFiEnd\tTimeDifference" << std::endl;
		}
		m_LogDualTimeDifference << iID << "\t" << starttime << "\t" << lteendtime << "\t" 
			<< hspaendtime << "\t" << timedifference << std::endl;

		return true;
	}


	bool SimLog::Log( string& filename, string& lineone, vector<int>& iVec, vector<double>& dVec )
	{
		if(!m_bLog)
			return false;
		if(!m_Log.is_open())
		{
			m_Log.open( ("../Output/Log" + filename + ".txt").c_str(), std::ios::trunc );
			//for( vector<string>::const_iterator sIter = lineone.begin(); sIter != lineone.end(); sIter++ )
			//{
			//	m_Log << *sIter;
			//}
			m_Log << lineone << endl;
		}

		for( vector<int>::const_iterator iIter = iVec.begin(); iIter != iVec.end(); iIter++ )
		{
			m_Log << *iIter << "\t";
		}
		for( vector<double>::const_iterator dIter = dVec.begin(); dIter != dVec.end(); dIter++ )
		{
			m_Log << *dIter << "\t\t";
		}
		m_Log << endl;

		return true;
	}


	bool SimLog::LogSINR( int iID, int iFrame, double dTime, vector<double> vecSINR )
	{
		if (!m_bLogSINR)
			return false;

		int ArrayNo = iID / 7;
		if ( !m_LogSINRArray[ArrayNo].is_open() )
		{
			std::stringstream ss;
			string s;
			ss << iID;
			ss >> s;
			m_LogSINRArray[ArrayNo].open( ("../Output/LogSINR_UEID_" + s + ".txt").c_str(), std::ios::trunc );
			m_LogSINRArray[ArrayNo] << "UEID\tFrame\tTime\tSINR" << std::endl;
		}
		m_LogSINRArray[ArrayNo] << iID << "\t" << iFrame << "\t" << dTime << "\t";
		for ( vector<double>::const_iterator dIter = vecSINR.begin(); dIter != vecSINR.end(); dIter++ )
		{
			m_LogSINRArray[ArrayNo] << std::left <<std::setw(10) << *dIter;
		}
		m_LogSINRArray[ArrayNo] << std::endl;

		return true;
	}

	bool SimLog::LogWiFi( int wifitime, ChannelState channelstate )
	{
		if (!m_bLogWiFi)
			return false;

		if (!m_LogWiFi.is_open())
		{
			m_LogWiFi.open( "../Output/LogWiFi.txt", std::ios::trunc );
			m_LogWiFi << "WiFiTime\tChannelState\t" << std::endl;
		}
		m_LogWiFi	<< wifitime << "\t\t"
			<< channelstate << "\t"
			<< std::endl;

		return true;
	}

	bool SimLog::LogLTECandidateServices( int subframetime, int nodebid, int servicenum )
	{
		if (!m_bLTECandidateServices)
			return false;

		if (!m_LogLTECandidateServices.is_open())
		{
			m_LogLTECandidateServices.open( "../Output/LogLTECandidateServices.txt", std::ios::trunc );
			m_LogLTECandidateServices << "SubFrameTime\tNodeBID\tServiceNum" << std::endl;
		}
		m_LogLTECandidateServices	<< subframetime << "\t\t"
			<< nodebid << "\t\t" <<servicenum
			<< std::endl;

		return true;
	}

	bool SimLog::LogWiFiCandidateServices( int wifitime, int apid, int servicenum )
	{
		if (!m_bWiFiCandidateServices)
			return false;

		if (!m_LogWiFiCandidateServices.is_open())
		{
			m_LogWiFiCandidateServices.open( "../Output/LogWiFiCandidateServices.txt", std::ios::trunc );
			m_LogWiFiCandidateServices << "WiFiTime\tAPID\tServiceNum" << std::endl;
		}
		m_LogWiFiCandidateServices	<< wifitime << "\t\t"
			<< apid << "\t\t" <<servicenum
			<< std::endl;

		return true;
	}

	bool SimLog::LogThrAndCU( int time, int nodebid, double ltethr, double wifithr, double ltecu, double wificu )
	{
		if (!m_bThrAndCU)
			return false;

		if (!m_LogThrAndCU.is_open())
		{
			m_LogThrAndCU.open( "../Output/LogThrAndCU.txt", std::ios::trunc );
			m_LogThrAndCU << "Time(ms)\tNodeBID\tLTEThr\tWiFiThr\tLTECU\tWiFiCU" << std::endl;
		}
		m_LogThrAndCU	<< time << "\t" << nodebid << "\t" << ltethr << "\t" << wifithr
			<< "\t" << ltecu << "\t" <<wificu << std::endl;

		return true;
	}

	bool SimLog::LogUESINR( int time, int nodebid, int ueid, double sinr)
	{
		if (!m_bUESINR)
			return false;

		if (!m_LogUESINR.is_open())
		{
			m_LogUESINR.open( "../Output/LogUESINR.txt", std::ios::trunc );
			m_LogUESINR << "Time(ms) NodeBID UEID SINR(dB)" << std::endl;
		}
		m_LogUESINR	<< time << "\t" << nodebid << "\t" << ueid << "\t" << sinr << std::endl;

		return true;
	}

}