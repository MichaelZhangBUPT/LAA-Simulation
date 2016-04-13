#include "commondef.h"
#include <boost/lambda/lambda.hpp>
#include "Decisioner.h"
#include "DB.h"
#include "MCSTable.h"
namespace LTESim
{

	Decisioner::Decisioner()
		:m_decisioner(RandomManager::GetRandomManager().GetUniformRealGen(0,1))
	{

	}
	Decisioner::~Decisioner(void)
	{
	}

	bool Decisioner::CheckCRC(const MCS& mcs,  const DB& sinr ) const
	{
		//生成以0~1均匀分布x，x<bler的概率是bler，则CRC校验当x>bler时返回true返回，x<bler时返回false
		//cout << "Decision threshhold " << GetBLER(mcs, sinr) << " SINR " << sinr << " MCS " << mcs.ToIndex() << endl;
		return m_decisioner() > GetBLER(mcs, sinr);
	}

	double TestDecisioner::GetBLER( const MCS& mcs, const DB& sinr ) const
	{
		if(sinr > 15.0)
			return 0;
		if (sinr < -3.0)
		{
			return 1;
		}
		return 1.0 - (sinr + 3) / 18;
	}


	
	DecisionerImp::DecisionerImp( const MCSTable& MCSTables ):m_BLERTables(MCSTables)
	{

	}

	double DecisionerImp::GetBLER( const MCS& mcs, const DB& sinr ) const
	{

		const MCSTable::MCSTABLE& BLERTable = m_BLERTables.GetMCSTable(mcs.ToIndex());
		//找到大于等于输入SINR的最小SINR项
		MCSTable::MCSTABLE::const_iterator iter = BLERTable.lower_bound(sinr);
		
		//cout << "I'm here " << BLERTable.size() << "  " <<  iter->first << "  " << iter->second << " sinr " << sinr << " MCS  " << mcs.ToIndex() <<  endl;
		//如果比最后一项的SINR还大就判为全对
		if (iter == BLERTable.end())
		{
			return 0;
		}
		return iter->second;
	}
}

