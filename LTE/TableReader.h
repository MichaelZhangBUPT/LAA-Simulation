#pragma once
#include <vector>
namespace LTESim
{
///���ļ��ж���һ����ά��
class TableReader
{
protected:
    ///���캯��
    /*
     * Ҫ��ȡ�Ķ�ά����ļ���
     */
	TableReader(const std::string& filename);

    ///��������
    virtual ~TableReader(void);
public:
    //��ά��ṹ
	typedef std::vector< std::vector<double> > TABLE;	
    
    ///�õ���ȡ�����ı�
    /*
     * @return �����ı���һάΪ�ļ����У��ڶ�άΪ�ļ�����
     */
	virtual const TABLE& GetTable() const = 0;

    ///���ص�[]�����
	virtual const std::vector<double>& operator[] (size_t offset) const = 0;
protected:
    /**Ҫ��ȡ���ļ���*/
	const std::string m_filename;
};

///��ά���ȡ�����ı��ļ�ʵ����
/*
 *	�����һ���ı���ʽ�Ķ�ά���ļ��ж�ȡ����
 */
class TextTableReader : public TableReader
{
public:
    ///���캯��
    /*
     * @param filename, Ҫ��ȡ���ı��ļ���
     * @param row, �ı��ļ�������
     * @param col, �ı��ļ�������
     */
	TextTableReader(const std::string& filename, std::size_t row, std::size_t col);
    ~TextTableReader();
    
    ///����ΪTableReader��ʵ��
public:
	virtual const TABLE& GetTable() const	
	{
		return m_table;
	};
	virtual const std::vector<double>& operator[] (size_t offset) const	//�����������[]��
	{
		return m_table[offset];
	}
protected:
	virtual void ReadTextFile();	//��ȡ����

protected:
    TABLE m_table;	//�����ȡ������
};
}
