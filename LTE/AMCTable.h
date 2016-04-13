#pragma once
#include <map>
namespace LTESim
{
    class MCSTable;
    class MCS;
    class DB;
    ///AMC����
    class AMCTable
    {
    public:
        typedef std::map<DB, MCS> AMCTABLE;	//AMC�洢����
        ///���캯��
        /*
         * @param MCSTables, �о��õ�MCS�����
         * @param TargetBLER, Ŀ��BLERֵ
         */
        AMCTable(const MCSTable& MCSTables, double TargetBLER, SystemType systemType);

        ///��ô洢��AMC������
        /*
         *	@return �洢��AMC��ÿһ��Ϊһ��MCS������Ӧ��CQIֵ���ޣ�
         *  Ҳ���Ǹ��ڴ�CQI����һ���CQI֮���Ӧ��ѡ���MCS
         */
        shared_ptr<const AMCTABLE> GetTable() const;	
    public:
        ///��������
        virtual ~AMCTable(void);
    
    protected:
        /**AMC��*/
        shared_ptr<AMCTABLE> m_AMCTable;	
    };

}
