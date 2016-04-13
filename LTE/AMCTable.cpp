#include "commondef.h"
#include <limits>
#include <boost/bind.hpp>

#include "mcs.h"
#include "AMCTable.h"
#include "MCSTable.h"
namespace LTESim
{
	AMCTable::AMCTable( const MCSTable& MCSTables, double TargetBLER, SystemType systemType )
	{
		using namespace boost;
		m_AMCTable = shared_ptr<AMCTABLE>(new AMCTABLE);
	
		//����ÿһ��MCS����
		for (size_t MCSIndex = 0; MCSIndex < MCSTables.GetMCSTableSize(); ++MCSIndex)
		{
			//MCSIndex��ӦMCS������
			const MCSTable::MCSTABLE& table = MCSTables.GetMCSTable(MCSIndex);

			//�ҵ���һ��bler��Ŀ��blerС�����൱����
			MCSTable::MCSTABLE::const_iterator iter  =
				find_if(table.begin(), table.end(), 
                bind(std::less_equal<double>(), bind(&MCSTable::MCSTABLE::value_type::second, _1), TargetBLER));	

			//����ҵ��Ļ���������뵽AMCTable��
			if( iter != table.end() )
			{
				//��AMCTable�д洢���ǣ�����ÿ��MCS������BLERС��Target BLER��SINRֵ
				//�����ڴ�����MCS��ʱ����ҪSystemType������AMCTable�Ĺ��캯����ҪSystemType
				m_AMCTable->insert(std::make_pair(iter->first, MCS(MCSIndex, systemType)));
			}
			else
				throw std::logic_error("Can't find apropriate mcs item.\n");
		}

		//��ͷβ����dummy���AMCEntity������

		//��ͷ����һ����СMCS��
		m_AMCTable->insert(
			std::make_pair(DB(-std::numeric_limits<double>::max())
			, m_AMCTable->begin()->second)
			);

		//��β����һ�����MCS��
		m_AMCTable->insert(
			std::make_pair(DB(std::numeric_limits<double>::max())
			,MCS(MCS::GetMaxMCSIndex( systemType ), systemType))
			);
			
	}
	AMCTable::~AMCTable(void)
	{
	}

	shared_ptr<const AMCTable::AMCTABLE> AMCTable::GetTable() const
	{
		return m_AMCTable;
	}
}
