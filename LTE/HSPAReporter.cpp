#include "commondef.h"
#include "HSPAReporter.h"
#include "HSPAUE.h"
#include "Matrix.h"
#include "mcs.h"

namespace LTESim
{

	HSPAReporter::HSPAReporter(shared_ptr<IHSPAUE> pUE )
		:	m_dC2I(100000.0),
			m_pMasterUE( pUE ), 
			m_RecommendMCS( 0, SYSTEM_HSPA ),						//初始状态的时候，将推荐的为index为0的MCS
			m_RecommendDLTransWeightMatrix(2, std::vector<std::complex<double>>(2, std::complex<double>(0.0, 0.0) ) )
	{
		m_RecommendDLTransWeightMatrix[0][0] = m_RecommendDLTransWeightMatrix[1][0] = HSPAMIMOWeight[0][0];
		m_RecommendDLTransWeightMatrix[0][1] = m_RecommendDLTransWeightMatrix[1][1] = HSPAMIMOWeight[0][1];
	}

	HSPAReporter::~HSPAReporter()
	{

	}

	void HSPAReporter::SetC2I(double dC2I)
	{
		m_dC2I = dC2I;
	}

	const double HSPAReporter::GetC2I() const
	{
		return m_dC2I;
	}

	void HSPAReporter::SetRecommendDLTransWeightMatrix(const Matrix& refMatrix )
	{
		m_RecommendDLTransWeightMatrix = refMatrix;
	}

	const Matrix& HSPAReporter::GetRecommendDLTransWeightMatrix() const
	{
		return m_RecommendDLTransWeightMatrix;
	}

	void HSPAReporter::SetRecommendMCS(const MCS& refMCS )
	{
		m_RecommendMCS = refMCS;
	}

	const MCS& HSPAReporter::GetRecommendMCS() const
	{
		return m_RecommendMCS;
	}

}