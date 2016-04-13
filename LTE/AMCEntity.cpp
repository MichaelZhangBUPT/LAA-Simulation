#include "commondef.h"
#include "AMCEntity.h"
namespace LTESim
{

	AMCEntity::AMCEntity( const AMCTABLE& amctable, const DB& stepup_db, const DB& stepdown_db )
	:m_AMCTable(amctable),  m_ThresholdOffset_db(0.0), m_StepUp_db(stepup_db), m_StepDown_db(stepdown_db)
	{

	}

	AMCEntity::~AMCEntity(void)
	{
	}

	MCS AMCEntity::SelectMCS( const DB& ChannelQuality ) const
	{
		AMCTABLE::const_iterator beg = m_AMCTable.begin();
		//经门限调整查表(调整门限相当于反向调整查表值)
		//越大的SINR对应的MCS等级越低，找到第一个大于目标SINR值，这里的准则是查找一个小的MCS，保证传输的正确性
		AMCTABLE::const_iterator iter = m_AMCTable.lower_bound(ChannelQuality - m_ThresholdOffset_db);
		if (iter != m_AMCTable.begin())
		{
			--iter;
		}
		return iter->second;
	}

	void AMCEntity::UpdateThreshold( bool crc_result )
	{
		//判决出错时上调，倾向于选择一个小的MCS
		//判决正确时下调门限，倾向于选择一个大的MCS
		m_ThresholdOffset_db += ( crc_result ? -m_StepDown_db : m_StepUp_db );
	}

}
