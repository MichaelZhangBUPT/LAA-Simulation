#pragma once

#include "commondef.h"
#include "IUE.h"

#include <vector>
#include <map>

namespace LTESim
{

	class LTENBReport
	{
	public:
		typedef std::vector<double> SINRVec;
	public:
		std::map<weak_ptr<IUE>, SINRVec>	m_UESINRVecMap;
		int m_iTotalRBNum;
		int m_iUsedRBNum;
	};

	class HSPANBReport
	{
	public:
		typedef std::vector<double> C2IVec;
	public:
		std::map<weak_ptr<IUE>, double> m_UEC2IMap;
	};

}