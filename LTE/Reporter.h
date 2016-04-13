#pragma once
#include <list>
#include <vector>
namespace LTESim
{
	class Service;
	class MCS;

	///信道信息上报接口类
	class Reporter
	{
	public:
		typedef std::vector<double>	SINRLIST;
		typedef std::vector<MCS>		CQILIST;
	public:
		///构造函数
		Reporter(void);
	public:
		///析构函数
		virtual ~Reporter(void);
		///更新某个RB上的SINR
		/**
		* @param RBIndex, 待更新的RBIndex
		* @param SINR, 待上报的SINR
		*/
		virtual void UpdateSINR(int RBIndex, double SINR ) = 0;
		
		///更新某个RB上的CQI
		/**
		* @param RBIndex, 待更新的频点（RB)
		* @param cqi, 待上报的CQI(MCS等级)
		*/
		virtual void UpdateCQI( int RBIndex, const MCS& cqi ) = 0;
		
		//获取指定RB的SINR
		/*
		 * @param RBIndex, 要查询的RB
		 * @return 该RB上的SINR
		 */
		virtual double GetSINR( int RBIndex ) const = 0;
		
		virtual const SINRLIST& GetSINRList() const = 0;

		//获取指定RB的CQI
		/*
		* @param RBIndex, 要查询的RB
		* @return 上报的CQI
		*/
		virtual const MCS& GetCQI(int RBIndex) const  = 0;

		virtual const CQILIST& GetCQIList() const = 0;
	};

	class ReporterImp : public Reporter
	{
	public:
		typedef std::vector<double>	SINRLIST;
		typedef std::vector<MCS>		CQILIST;
	public:
		///构造函数
		ReporterImp();

		///析构函数
		ReporterImp::~ReporterImp();

		///以下为Report接口的实现
	public:
		//更新SINR
		virtual void UpdateSINR(int RBIndex, double sinr ) ;

		//更新CQI
		virtual void UpdateCQI( int RBIndex, const MCS& cqi );

		//获取指定的SINR
		virtual double GetSINR(int RBIndex ) const ;

		virtual const SINRLIST& GetSINRList() const;

		//获取指定的CQI反馈
		virtual const MCS& GetCQI(int RBIndex) const ;

		virtual const CQILIST& GetCQIList() const;
	protected:
		/**保存的SINR和CQI列表*/
		SINRLIST m_vSINRList;
		CQILIST m_vCQIList;
	};

};
