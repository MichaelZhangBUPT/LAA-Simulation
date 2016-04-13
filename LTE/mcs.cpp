#include "mcs.h"
#include "commondef.h"

namespace LTESim
{
	MCS::MCS( int MCSLevel, SystemType systemtype )
	{
		Init( MCSLevel, systemtype );
	}

	void MCS::Init(int MCSLevel, SystemType systemtype )
	{
		//调制方式
		const static MODULATION LTEModsVec[] =					{QPSK, QPSK, QPSK, QPSK,QPSK, QAM16,QAM16,QAM16,QAM16, QAM64,QAM64,QAM64};
		//编码速率
		const static CHANNELCODE_RATE LTERatesVec[]	=	{RATE_1_5, RATE_1_3, RATE_1_2, RATE_2_3, RATE_3_4, RATE_1_2, RATE_2_3, RATE_3_4, RATE_4_5, RATE_3_5, RATE_2_3, RATE_4_5};
		const static unsigned LTETbPerRBVec[]						=	{30,50,76,101,114,152,202,228,243,273,304,364};

		const static MODULATION HSPAModRank[]				=	{QPSK,QPSK,QPSK,QPSK,QAM16,QAM16,QAM16,QAM64,QAM64,QAM64,QAM64};
		const static double HSPACodeGain[]								=	{0.333, 0.5, 0.667, 0.8, 0.5, 0.667, 0.8, 0.333, 0.5, 0.667, 0.8};
		const static size_t HSPATableBlock[]							=	{320, 480, 640, 768, 960, 1280, 1536, 1604, 1767, 1940, 2328};

		const static MODULATION WiFiModVec[] ={BPSK, QPSK, QPSK, QAM16, QAM16, QAM64, QAM64, QAM64};
		const static CHANNELCODE_RATE WiFiCodeRateVec[] = {RATE_1_2, RATE_1_2, RATE_3_4, RATE_1_2, RATE_3_4, RATE_2_3, RATE_3_4, RATE_5_6};
		const static double WiFiRateVec[] = {6.5, 13.0, 19.5, 26.0, 39.0, 52.0, 58.5, 65.0};//单位 Mbps

		const static MODULATION LAAModVec[] =		{QPSK, QPSK, QPSK, QPSK,QPSK, QAM16,QAM16,QAM16,QAM16, QAM64,QAM64,QAM64};
		const static CHANNELCODE_RATE LAACodeRateVec[] = {RATE_1_5, RATE_1_3, RATE_1_2, RATE_2_3, RATE_3_4, RATE_1_2, RATE_2_3, RATE_3_4, RATE_4_5, RATE_3_5, RATE_2_3, RATE_4_5};
		const static double LAARateVec[] ={30,50,76,101,114,152,202,228,243,273,304,364};//单位 Mbps
		
		m_iMCSLevel = MCSLevel;
		m_SystemType = systemtype;
		if ( systemtype == SYSTEM_LTE )
		{
			if (MCSLevel >= sizeof(LTEModsVec)/sizeof(LTEModsVec[0]))
				throw std::out_of_range("LTE MCS Index Out of Range!");

			m_Modulation = LTEModsVec[MCSLevel];
			m_CodeRate = LTERatesVec[MCSLevel];
			m_dTableBlock = LTETbPerRBVec[MCSLevel];
		}
		else if ( systemtype == SYSTEM_HSPA )
		{
			if ( MCSLevel >= sizeof( HSPAModRank ) / sizeof( HSPAModRank[0] ))
				throw std::out_of_range("HSPA MCS Index Out of Range");

			m_Modulation = HSPAModRank[MCSLevel];
			m_dCodeGain = HSPACodeGain[MCSLevel];
			m_dTableBlock = HSPATableBlock[MCSLevel];
		}
		else if ( systemtype == SYSTEM_WIFI )
		{
			if ( MCSLevel >= sizeof( WiFiModVec ) / sizeof( WiFiModVec[0] ))
				throw std::out_of_range("WiFi MCS Index Out of Range");

			m_Modulation = WiFiModVec[MCSLevel];
			m_CodeRate = WiFiCodeRateVec[MCSLevel];
			m_dWiFiRate = WiFiRateVec[MCSLevel];
		}
		else if ( systemtype == SYSTEM_LAA )
		{
			if ( MCSLevel >= sizeof( LAAModVec ) / sizeof( LAAModVec[0] ))
				throw std::out_of_range("LAA MCS Index Out of Range");

			m_Modulation = LAAModVec[MCSLevel];
			m_CodeRate = LAACodeRateVec[MCSLevel];
			m_dLAARate = LAARateVec[MCSLevel];
		}
		else
			throw std::invalid_argument( "Unknown System Type" );
	}

	//初始化MCS编号
	int MCS::ToIndex() const
	{
		return m_iMCSLevel;
	}

	MODULATION MCS::GetModulation() const
	{
		return m_Modulation;
	}

	CHANNELCODE_RATE MCS::GetCodeRate() const
	{
		return m_CodeRate;
	}

	double MCS::GetCodeGain() const
	{
		return m_dCodeGain;
	}

	double MCS::GetTableBlock() const
	{
		return m_dTableBlock;
	}

	double MCS::GetWiFiRate() const
	{
		return m_dWiFiRate;
	}

	double MCS::GetLAARate() const
	{
		return m_dLAARate;
	}
}