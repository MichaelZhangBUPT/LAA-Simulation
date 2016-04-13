#pragma once
#include <map>
namespace LTESim
{
    class MCSTable;
    class MCS;
    class DB;
    ///AMC表类
    class AMCTable
    {
    public:
        typedef std::map<DB, MCS> AMCTABLE;	//AMC存储类型
        ///构造函数
        /*
         * @param MCSTables, 判决用的MCS表对象
         * @param TargetBLER, 目标BLER值
         */
        AMCTable(const MCSTable& MCSTables, double TargetBLER, SystemType systemType);

        ///获得存储的AMC表数据
        /*
         *	@return 存储的AMC表，每一项为一个MCS和它对应的CQI值下限，
         *  也就是高于此CQI到下一项的CQI之间就应当选择此MCS
         */
        shared_ptr<const AMCTABLE> GetTable() const;	
    public:
        ///析构函数
        virtual ~AMCTable(void);
    
    protected:
        /**AMC表*/
        shared_ptr<AMCTABLE> m_AMCTable;	
    };

}
