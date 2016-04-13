#pragma once
#include <map>
#include "RandomManager.h"
#include "mcs.h"

namespace LTESim
{
	class DB;	
	class Decisioner
	{
	public:
		Decisioner();
	public:
		virtual ~Decisioner(void);
		//�����о���������һ��CRCУ�飬�о���ȷ����true�����󷵻�false
		virtual bool CheckCRC(const MCS& mcs, const DB& sinr) const;

		//�����о��������ض�Ӧ��BLER
		virtual double GetBLER(const MCS& mcs, const DB& sinr ) const = 0;
	protected:
		mutable UNIFORM_REAL_GEN m_decisioner;
	};

	/*ȫ�жԣ����ڼ������ʲ���*/
	class AllTrueDecioner : public Decisioner
	{
	public:
		bool CheckCRC(const MCS& mcs, const DB& sinr) const
		{
			return true;
		}
		double GetBLER(const MCS& mcs, const DB& sinr ) const
		{
			return 0.0;
		}
	};

	/*ȫ�д�*/
	class AllFalseDecioner : public Decisioner
	{
	public:
		bool CheckCRC(const MCS& mcs, const DB& sinr) const
		{
			return false;
		}
		double GetBLER(const MCS& mcs, const DB& sinr ) const
		{
			return 0.0;
		}
	};

	class TestDecisioner : public Decisioner
	{
	public:
		double GetBLER(const MCS& mcs, const DB& sinr ) const;
	};

	class MCSTable;
	class DecisionerImp : public Decisioner
	{
	public:
		DecisionerImp(const MCSTable& MCSTables);
	public:
		double GetBLER(const MCS& mcs, const DB& sinr ) const;
	protected:
		const MCSTable& m_BLERTables;	
	};
}
