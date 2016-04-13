#include "commondef.h"
#include <utility>
#include "MCSTable.h"
#include "TableReader.h"

namespace LTESim
{


	MCSTable::MCSTable( const std::vector<  shared_ptr<const TableReader> >& pTableReaders )
	{
		m_MCSTables.resize(pTableReaders.size());	//定义m_MCSTables的大小
		for (size_t mcsindex = 0; mcsindex < pTableReaders.size(); ++mcsindex)
		{
			const TableReader::TABLE& table = pTableReaders[mcsindex]->GetTable();	//获得TableReaders中的数据，二维
			for (size_t TableRow = 0; TableRow < table.size(); ++TableRow)
			{
				//认为TABLE第一列是以DB为单位的SINR，第二列是BLER
				m_MCSTables[mcsindex].insert(std::make_pair(table[TableRow][0], table[TableRow][1]));
			}
		}
		//std::map<DB, double>::iterator itr = m_MCSTables[4].begin();
		//for(; itr != m_MCSTables[4].end(); ++itr)
		//{
		//	cout << "MCS test " << itr->first << "  " << itr->second << endl;
		//}

	}

	MCSTable::~MCSTable(void)
	{
	}

	const MCSTable::MCSTABLE& MCSTable::GetMCSTable(size_t MCSLevel) const
	{
		//得到的是特定MCSIndex这个下标对应的MCS对应的曲线，该曲线表示的是SINR的DB值与BLER之间的对应的曲线
		return m_MCSTables[MCSLevel];
	}

};
