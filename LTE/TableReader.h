#pragma once
#include <vector>
namespace LTESim
{
///从文件中读入一个二维表
class TableReader
{
protected:
    ///构造函数
    /*
     * 要读取的二维表的文件名
     */
	TableReader(const std::string& filename);

    ///析构函数
    virtual ~TableReader(void);
public:
    //二维表结构
	typedef std::vector< std::vector<double> > TABLE;	
    
    ///得到读取出来的表
    /*
     * @return 读出的表，第一维为文件的行，第二维为文件的列
     */
	virtual const TABLE& GetTable() const = 0;

    ///重载的[]运算符
	virtual const std::vector<double>& operator[] (size_t offset) const = 0;
protected:
    /**要读取的文件名*/
	const std::string m_filename;
};

///二维表读取器的文本文件实现类
/*
 *	负责从一个文本格式的二维表文件中读取数据
 */
class TextTableReader : public TableReader
{
public:
    ///构造函数
    /*
     * @param filename, 要读取的文本文件名
     * @param row, 文本文件的行数
     * @param col, 文本文件的列数
     */
	TextTableReader(const std::string& filename, std::size_t row, std::size_t col);
    ~TextTableReader();
    
    ///以下为TableReader的实现
public:
	virtual const TABLE& GetTable() const	
	{
		return m_table;
	};
	virtual const std::vector<double>& operator[] (size_t offset) const	//重载运算符“[]”
	{
		return m_table[offset];
	}
protected:
	virtual void ReadTextFile();	//读取函数

protected:
    TABLE m_table;	//储存读取得数据
};
}
