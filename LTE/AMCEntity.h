#pragma once
#include <map>
#include "mcs.h"
#include "DB.h"
namespace LTESim
{

class AMCEntity
{
public:
    //�ŵ�����(SINR)�½缰���Ӧ��MCS����֤MCS�����������ڵ�CQI�м�
	typedef std::map<DB/*ChannelQuality*/, MCS> AMCTABLE;
    ///���캯��
    /*
    * @param AMCTable, MCS�Ƽ������е�ÿһ��ΪCQI�½缰���Ӧ��MCS
    * @param stepup_db, �����ϵ��Ĳ���
    * @param stepdown_db, �����µ��Ĳ���
    */
	AMCEntity( const AMCTABLE& AMCTable, const DB& stepup_db, const DB& stepdown_db );
public:
    ///��������
	virtual ~AMCEntity(void);

	//AMC���޸���
    /*
    * ����������������һ��AMC���޵���ֵ����ȷ�Ļ��µ����ޣ������ϵ�����
    * @param crc_resutl, CRCУ��Ľ����trueΪУ����ȷ,falseΪУ�����
    */
	void UpdateThreshold( bool crc_result );
	
	//���ص�ǰ�������ۻ�����ֵ
    /*
    * @return ��ǰ�����ۻ��ĵ���ֵ����ʼΪ0db
    */
	const DB& GetThresholdOffset_db() const;
	
	//���������ۻ�����ֵ
	void ResetThresholdOffset();

	//�����ŵ�����ѡ��MCS
    /*
    * @param ChannelQuality��CQI��ֵ���Դ�ѡȡMCS�ȼ�
    * @return ��Ӧ��MCS�ȼ�
    */
	MCS SelectMCS( const DB& ChannelQuality ) const;

protected:
	//Ϊ���������㷨��ʵ���ڸ�map��ͷβ������һ�������
	//ͷ���ֵ���κδ����SINR��С������value������С��MCS
	//β���ֵ���κδ����SINR���󣬲���value��������MCS
	//���������㷨�����ж��Ƿ�����ʧ�ܷ�����end
	const AMCTABLE& m_AMCTable;

	//���޵���ֵ
	DB m_ThresholdOffset_db;
	
	//�����ϵ����������о�����ʱ�ϵ�
	DB m_StepUp_db;

	//�����µ����������о���ȷʱ�µ�
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

