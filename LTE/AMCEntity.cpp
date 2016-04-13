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
		//�����޵������(���������൱�ڷ���������ֵ)
		//Խ���SINR��Ӧ��MCS�ȼ�Խ�ͣ��ҵ���һ������Ŀ��SINRֵ�������׼���ǲ���һ��С��MCS����֤�������ȷ��
		AMCTABLE::const_iterator iter = m_AMCTable.lower_bound(ChannelQuality - m_ThresholdOffset_db);
		if (iter != m_AMCTable.begin())
		{
			--iter;
		}
		return iter->second;
	}

	void AMCEntity::UpdateThreshold( bool crc_result )
	{
		//�о�����ʱ�ϵ���������ѡ��һ��С��MCS
		//�о���ȷʱ�µ����ޣ�������ѡ��һ�����MCS
		m_ThresholdOffset_db += ( crc_result ? -m_StepDown_db : m_StepUp_db );
	}

}
