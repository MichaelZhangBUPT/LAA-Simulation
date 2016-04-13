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
		//������0~1���ȷֲ�x��x<bler�ĸ�����bler����CRCУ�鵱x>blerʱ����true���أ�x<blerʱ����false
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
		//�ҵ����ڵ�������SINR����СSINR��
		MCSTable::MCSTABLE::const_iterator iter = BLERTable.lower_bound(sinr);
		
		//cout << "I'm here " << BLERTable.size() << "  " <<  iter->first << "  " << iter->second << " sinr " << sinr << " MCS  " << mcs.ToIndex() <<  endl;
		//��������һ���SINR�������Ϊȫ��
		if (iter == BLERTable.end())
		{
			return 0;
		}
		return iter->second;
	}
}

