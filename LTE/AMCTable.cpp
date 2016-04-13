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
	
		//对于每一条MCS曲线
		for (size_t MCSIndex = 0; MCSIndex < MCSTables.GetMCSTableSize(); ++MCSIndex)
		{
			//MCSIndex对应MCS的曲线
			const MCSTable::MCSTABLE& table = MCSTables.GetMCSTable(MCSIndex);

			//找到第一个bler比目标bler小或者相当的项
			MCSTable::MCSTABLE::const_iterator iter  =
				find_if(table.begin(), table.end(), 
                bind(std::less_equal<double>(), bind(&MCSTable::MCSTABLE::value_type::second, _1), TargetBLER));	

			//如果找到的话将该项加入到AMCTable中
			if( iter != table.end() )
			{
				//在AMCTable中存储的是，对于每个MCS，满足BLER小于Target BLER的SINR值
				//由于在此生成MCS的时候需要SystemType，所以AMCTable的构造函数需要SystemType
				m_AMCTable->insert(std::make_pair(iter->first, MCS(MCSIndex, systemType)));
			}
			else
				throw std::logic_error("Can't find apropriate mcs item.\n");
		}

		//在头尾加入dummy项方便AMCEntity的搜索

		//在头加入一个最小MCS项
		m_AMCTable->insert(
			std::make_pair(DB(-std::numeric_limits<double>::max())
			, m_AMCTable->begin()->second)
			);

		//在尾加入一个最大MCS项
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
