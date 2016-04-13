#pragma once

#include "commondef.h"
#include "Matrix.h"
#include "mcs.h"

namespace LTESim
{
	class IHSPAUE;

	class IHSPAReporter
	{
	public:
		IHSPAReporter(){};
		virtual ~IHSPAReporter(){};
	public:
		virtual void SetRecommendDLTransWeightMatrix( const Matrix& refMatrix ) = 0;
		virtual const Matrix& GetRecommendDLTransWeightMatrix() const = 0;

		virtual void SetC2I( double dC2I ) = 0;
		virtual const double GetC2I() const = 0;

		virtual void SetRecommendMCS( const MCS& refMCS ) = 0;
		virtual const MCS& GetRecommendMCS() const = 0;
	};

	class HSPAReporter : public IHSPAReporter
	{
	public:
		HSPAReporter( shared_ptr<IHSPAUE> pUE );
		virtual ~HSPAReporter();

		virtual void SetRecommendDLTransWeightMatrix( const Matrix& refMatrix );
		virtual const Matrix& GetRecommendDLTransWeightMatrix() const;

		virtual void SetC2I( double dC2I );
		virtual const double GetC2I() const;

		virtual void SetRecommendMCS( const MCS& refMCS );
		virtual const MCS& GetRecommendMCS() const;

	protected:
		double			m_dC2I;
		Matrix			m_RecommendDLTransWeightMatrix;
		MCS				m_RecommendMCS;
		weak_ptr<IHSPAUE> m_pMasterUE;
	};

}