#include "commondef.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <boost/bind.hpp>
#include "TableReader.h"
namespace LTESim
{
	TableReader::TableReader( const std::string& filename):
	m_filename(filename)
{
}
TableReader::~TableReader(void)
{
}


TextTableReader::TextTableReader( const std::string& filename, std::size_t row, std::size_t col )
:TableReader(filename)
{
	//先分配m_table空间，然后再读取
	m_table.resize(row);
	std::for_each(m_table.begin(), m_table.end(), boost::bind(&std::vector<double>::resize, _1, col, 0.0));
	ReadTextFile();	//读取函数
}

TextTableReader::~TextTableReader()
{

}
void TextTableReader::ReadTextFile()
{
	std::ifstream file(m_filename.c_str());
	if (file)
	{
		for ( size_t row = 0; row < m_table.size(); ++row )
		{
			for( size_t col = 0; col < m_table[row].size(); ++col )
			{
				file >> m_table[row][col];	//应用对“[]”的重载							
			}
		}
		file.close();
	}
	else
	{
		throw std::logic_error("Can't find the table file.\n");
	}
}

}
