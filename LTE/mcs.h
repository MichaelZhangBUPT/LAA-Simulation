#pragma once
#include <stdexcept>

#include "commondef.h"

namespace LTESim
{
	///MCS��
	class MCS
	{
	public:
		///�����ض���MCS�ȼ�
		/*
		* @param MCSLevel, MCS�ȼ�
		*/
		MCS( int MCSLevel = 0, SystemType systemtype = SYSTEM_LTE  );
		//������MCS���
		/*
		* MCS�ȼ���Ŵ�0~�������������Խ���MCS�ȼ�TBԽ��
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

		///����ܵ�MCS�ȼ���
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
		///��ʼ��Ϊָ���ȼ���MCS
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

