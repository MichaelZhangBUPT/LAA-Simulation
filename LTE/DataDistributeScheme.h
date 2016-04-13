#pragma once

#include "commondef.h"
#include <vector>

namespace LTESim
{
	class DataDistributeSchemeNode
	{
	public:
		DataDistributeSchemeNode( SystemType systemType, double dBufferDataVol );
		~DataDistributeSchemeNode();
	public:
		SystemType GetSystemType() const;
		double GetBufferDataVolume() const;
	private:
		SystemType		m_SystemType;
		double				m_dBufferDataVol;
	};


	class DataDistributeScheme
	{
	public:
		DataDistributeScheme();
		DataDistributeScheme( std::vector<DataDistributeSchemeNode>& rSchemeNodeVec );
		~DataDistributeScheme();
	public:
		double	GetBufferDataVolume( SystemType systemType ) const;
		void		AddSchemeNode( const DataDistributeSchemeNode& rSchemeNode );
	private:
		std::vector<DataDistributeSchemeNode>		m_SchemeNodeVec;
	};
}