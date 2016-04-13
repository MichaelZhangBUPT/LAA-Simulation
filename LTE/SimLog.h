#pragma once
#include "commondef.h"
#include "UE.h"
#include "NodeB.h"
#include "POS.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace LTESim
{
	class SimLog
	{
	public:
		static SimLog * Instance();
		~SimLog();
	public:
		bool LogServiceBufferUpdate( int iID, int iFrame, int iInterval, SystemType ueType );
		bool LogBufferEmpty( int iID, int iFrame, SystemType ueType, SystemType serviceType );
		bool LogCompositeServiceDistributeBuffer( int iID, int iFrame, double dLTEBufferVol, double dHSPABufferVol );
		bool LogAccessInformation( int iUEID, const POS& rUEPos, int iNBID, const POS& rNBPos, double dDistance );
		bool MakeDecisionCount( int iID, int iFrame, SystemType ueType, SystemType serviceType, bool bIsSuccess );
		bool LogLTERatio( int iID, int iFrame, double dLTERatio );
		bool LogC2I( int iID, int iFrame, double dTime, double dC2I );
		bool LogMaxTransTimeNum( int iID, int iTime );
		bool Log( string& filename, string& lineone, vector<int>& iVec, vector<double>& dVec );
		//加个参数int LogID，用来指定使用哪个Log？
		bool LogSINR( int iID, int iFrame, double dTime, vector<double> vecSINR );//实值
		bool LogDualTimeDifference( int iID, double starttime, double lteendtime, double hspaendtime, double timedifference );
		bool LogWiFi( int wifitime, ChannelState channelstate  );
		bool LogLTECandidateServices( int subframetime, int nodebid, int servicenum );
		bool LogWiFiCandidateServices( int wifitime, int apid, int servicenum );
		bool LogThrAndCU( int time, int nodebid, double ltethr, double wifithr, double ltecu, double wificu );
		bool LogUESINR( int time, int nodebid, int ueid, double sinr);
	protected:
		SimLog();
	private:
		static SimLog * m_Instance;
	//protected:
	//	std::map<int, std::ofstream>	m_LogC2I;
	private:
		std::ofstream	m_LogServiceBufferUpdate;
		std::ofstream	m_LogAccessInformation;
		std::ofstream	m_LogCompositeServiceDistributeBuffer;
		std::ofstream	m_LogBufferEmpty;
		std::ofstream	m_MakeDecisionCount;
		std::ofstream	m_LogLTERatio;
		std::ofstream	m_LogMaxTransTimeNum;
		std::ofstream	m_Log;//需要定义许多此类变量？？一个只能用一遍吧?
		std::map<int, int>	m_FileIndex;		//用户ID 输出流数组下标
		std::ofstream	m_LogC2IArray[50];
		std::ofstream	m_LogSINRArray[140];
		std::ofstream	m_LogDualTimeDifference;
		std::ofstream	m_LogWiFi;
		std::ofstream	m_LogLTECandidateServices;
		std::ofstream	m_LogWiFiCandidateServices;
		std::ofstream	m_LogThrAndCU;
		std::ofstream	m_LogUESINR;

		bool m_bLogServiceBufferUpdate;
		bool m_bLogAccessInformation;
		bool m_bLogCompositeServiceDistributeBuffer;
		bool m_bLogBufferEmpty;
		bool m_bMakeDecisionCount;
		bool m_bLogLTERatio;
		bool m_bLogC2I;
		bool m_bLogMaxTransTimeNum;
		bool m_bLog;
		bool m_bLogSINR;
		bool m_bLogDualTimeDifference;
		bool m_bLogWiFi;
		bool m_bLTECandidateServices;
		bool m_bWiFiCandidateServices;
		bool m_bThrAndCU;
		bool m_bUESINR;
	};
}