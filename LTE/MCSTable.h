#pragma once
#include <map>
#include "DB.h"
namespace LTESim
{
class TableReader;
class MCSTable
{
public:
    //��λ�洢���ͣ�ÿһ��Ϊÿ��SNR��Ӧ��BLER
	typedef std::map<DB, double> MCSTABLE;	
public:
    ///���캯��
    /*
     * @param pTableReaders, ��������������飬ÿһ����Ӧһ��MCS
     */
	MCSTable(const std::vector< shared_ptr<const TableReader> >& pTableReaders);
public:
    ///��������
	virtual ~MCSTable(void);

    ///��ô洢���ݱ�Ĵ�С,����ͬMCS�ļ���
	size_t GetMCSTableSize() const	
	{
		return m_MCSTables.size();
	};
    
    //��ô洢�ĵ�λ���ݣ���ĳһ��MCS�ı�MCSTABLE
	virtual const MCSTable::MCSTABLE& GetMCSTable(size_t MCSLevel) const;	

protected:
	std::vector< MCSTABLE > m_MCSTables;	//�洢��TableReaders�еõ�������
};

};
