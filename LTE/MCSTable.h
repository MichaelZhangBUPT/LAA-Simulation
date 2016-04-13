#pragma once
#include <map>
#include "DB.h"
namespace LTESim
{
class TableReader;
class MCSTable
{
public:
    //单位存储类型，每一项为每个SNR对应的BLER
	typedef std::map<DB, double> MCSTABLE;	
public:
    ///构造函数
    /*
     * @param pTableReaders, 表的输入对象的数组，每一个对应一个MCS
     */
	MCSTable(const std::vector< shared_ptr<const TableReader> >& pTableReaders);
public:
    ///析构函数
	virtual ~MCSTable(void);

    ///获得存储数据表的大小,即不同MCS的级数
	size_t GetMCSTableSize() const	
	{
		return m_MCSTables.size();
	};
    
    //获得存储的单位数据，即某一个MCS的表MCSTABLE
	virtual const MCSTable::MCSTABLE& GetMCSTable(size_t MCSLevel) const;	

protected:
	std::vector< MCSTABLE > m_MCSTables;	//存储从TableReaders中得到的数据
};

};
