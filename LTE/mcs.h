#pragma once
#include <stdexcept>

#include "commondef.h"

namespace LTESim
{
	///MCS类
	class MCS
	{
	public:
		///构造特定的MCS等级
		/*
		* @param MCSLevel, MCS等级
		*/
		MCS( int MCSLevel = 0, SystemType systemtype = SYSTEM_LTE  );
		//获得最大MCS编号
		/*
		* MCS等级编号从0~此最大编号连续，越大的MCS等级TB越大
		*/
		static size_t GetMaxMCSIndex( SystemType systemtype )	
		{
			if ( systemtype == SYSTEM_LTE )
				return 11;
			else if ( systemtype == SYSTEM_HSPA )
				return 10;
			else if ( systemtype == SYSTEM_WIFI )
				return 7;
			else if ( systemtype == SYSTEM_LAA )
				return 11;
			else
				throw std::invalid_argument( "Unknown System Type" );
		}

		///获得总的MCS等级数
		static size_t GetMCSLevels( SystemType systemtype )
		{
			if ( systemtype == SYSTEM_LTE )
				return 12;
			else if ( systemtype == SYSTEM_HSPA )
				return 11;
			else if ( systemtype == SYSTEM_WIFI )
				return 8;
			else if ( systemtype == SYSTEM_LAA )
				return 12;
			else
				throw std::invalid_argument( "Unknown System Type" );
		}

	public:
		int ToIndex() const;
		double GetTableBlock() const;
		MODULATION GetModulation() const;
		CHANNELCODE_RATE GetCodeRate() const;
		double GetCodeGain() const;
		double GetWiFiRate() const;
		double GetLAARate() const;
	protected:
		///初始化为指定等级的MCS
		void Init( int MCSLevel, SystemType systemtype );
	protected:
		MODULATION					m_Modulation;
		CHANNELCODE_RATE	m_CodeRate;
		size_t									m_dTableBlock;
		double									m_dCodeGain;
		int											m_iMCSLevel;
		SystemType							m_SystemType;
		double								m_dWiFiRate;
		double								m_dLAARate;
	};
}

