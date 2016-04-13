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
		//给定判决参数进行一次CRC校验，判决正确返回true，错误返回false
		virtual bool CheckCRC(const MCS& mcs, const DB& sinr) const;

		//给定判决参数返回对应的BLER
		virtual double GetBLER(const MCS& mcs, const DB& sinr ) const = 0;
	protected:
		mutable UNIFORM_REAL_GEN m_decisioner;
	};

	/*全判对，用于极限速率测试*/
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

	/*全判错*/
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
