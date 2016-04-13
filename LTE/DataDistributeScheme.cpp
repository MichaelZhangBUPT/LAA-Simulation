#include "DataDistributeScheme.h"

namespace LTESim
{
	DataDistributeSchemeNode::DataDistributeSchemeNode( SystemType systemType, double dBufferDataVol )
		: m_SystemType( systemType ), m_dBufferDataVol( dBufferDataVol )
	{

	}

	DataDistributeSchemeNode::~DataDistributeSchemeNode()
	{

	}

	SystemType DataDistributeSchemeNode::GetSystemType() const
	{
		return m_SystemType;
	}

	double DataDistributeSchemeNode::GetBufferDataVolume() const
	{
		return m_dBufferDataVol;
	}

	DataDistributeScheme::DataDistributeScheme()
	{

	}

	DataDistributeScheme::DataDistributeScheme( std::vector<DataDistributeSchemeNode>& rSchemeNodeVec )
		: m_SchemeNodeVec( rSchemeNodeVec )
	{

	}

	DataDistributeScheme::~DataDistributeScheme()
	{

	}

	double DataDistributeScheme::GetBufferDataVolume( SystemType systemType ) const
	{
		for ( std::vector<DataDistributeSchemeNode>::const_iterator vecIter = m_SchemeNodeVec.begin(); vecIter != m_SchemeNodeVec.end(); ++vecIter )
		{
			if ( vecIter->GetSystemType() == systemType )
				return vecIter->GetBufferDataVolume();
		}
		throw std::invalid_argument( "InValid System Type for Data Distribution" );
	}

	void DataDistributeScheme::AddSchemeNode( const DataDistributeSchemeNode& rSchemeNode )
	{
		m_SchemeNodeVec.push_back( rSchemeNode );
	}

}