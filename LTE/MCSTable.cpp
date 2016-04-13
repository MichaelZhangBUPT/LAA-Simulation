#include "commondef.h"
#include <utility>
#include "MCSTable.h"
#include "TableReader.h"

namespace LTESim
{


	MCSTable::MCSTable( const std::vector<  shared_ptr<const TableReader> >& pTableReaders )
	{
		m_MCSTables.resize(pTableReaders.size());	//����m_MCSTables�Ĵ�С
		for (size_t mcsindex = 0; mcsindex < pTableReaders.size(); ++mcsindex)
		{
			const TableReader::TABLE& table = pTableReaders[mcsindex]->GetTable();	//���TableReaders�е����ݣ���ά
			for (size_t TableRow = 0; TableRow < table.size(); ++TableRow)
			{
				//��ΪTABLE��һ������DBΪ��λ��SINR���ڶ�����BLER
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
		//�õ������ض�MCSIndex����±��Ӧ��MCS��Ӧ�����ߣ������߱�ʾ����SINR��DBֵ��BLER֮��Ķ�Ӧ������
		return m_MCSTables[MCSLevel];
	}

};
