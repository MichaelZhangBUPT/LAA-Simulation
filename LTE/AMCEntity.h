#pragma once
#include <map>
#include "mcs.h"
#include "DB.h"
namespace LTESim
{

class AMCEntity
{
public:
    //信道质量(SINR)下界及其对应的MCS，保证MCS落在两个相邻的CQI中间
	typedef std::map<DB/*ChannelQuality*/, MCS> AMCTABLE;
    ///构造函数
    /*
    * @param AMCTable, MCS推荐表，表中的每一项为CQI下界及其对应的MCS
    * @param stepup_db, 门限上调的步长
    * @param stepdown_db, 门限下调的步长
    */
	AMCEntity( const AMCTABLE& AMCTable, const DB& stepup_db, const DB& stepdown_db );
public:
    ///析构函数
	virtual ~AMCEntity(void);

	//AMC门限更新
    /*
    * 根据误包结果，更新一次AMC门限调整值，正确的话下调门限，错误上调门限
    * @param crc_resutl, CRC校验的结果，true为校验正确,false为校验错误
    */
	void UpdateThreshold( bool crc_result );
	
	//返回当前的门限累积调整值
    /*
    * @return 当前门限累积的调整值，初始为0db
    */
	const DB& GetThresholdOffset_db() const;
	
	//重置门限累积调整值
	void ResetThresholdOffset();

	//根据信道质量选择MCS
    /*
    * @param ChannelQuality，CQI的值，以此选取MCS等级
    * @return 对应的MCS等级
    */
	MCS SelectMCS( const DB& ChannelQuality ) const;

protected:
	//为方便搜索算法的实现在该map的头尾各加入一个虚拟项，
	//头项键值比任何传入的SINR都小，并且value等于最小的MCS
	//尾项键值比任何传入的SINR都大，并且value等于最大的MCS
	//这样搜索算法不用判断是否搜索失败返回了end
	const AMCTABLE& m_AMCTable;

	//门限调整值
	DB m_ThresholdOffset_db;
	
	//门限上调步长，当判决错误时上调
	DB m_StepUp_db;

	//门限下调步长，当判决正确时下调
	DB m_StepDown_db;
	
};


inline const DB& AMCEntity::GetThresholdOffset_db() const
{
	return m_ThresholdOffset_db;
}

inline void AMCEntity::ResetThresholdOffset() 
{
	m_ThresholdOffset_db.m_dbval = 0;
}
}

