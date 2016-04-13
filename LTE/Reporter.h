#pragma once
#include <list>
#include <vector>
namespace LTESim
{
	class Service;
	class MCS;

	///�ŵ���Ϣ�ϱ��ӿ���
	class Reporter
	{
	public:
		typedef std::vector<double>	SINRLIST;
		typedef std::vector<MCS>		CQILIST;
	public:
		///���캯��
		Reporter(void);
	public:
		///��������
		virtual ~Reporter(void);
		///����ĳ��RB�ϵ�SINR
		/**
		* @param RBIndex, �����µ�RBIndex
		* @param SINR, ���ϱ���SINR
		*/
		virtual void UpdateSINR(int RBIndex, double SINR ) = 0;
		
		///����ĳ��RB�ϵ�CQI
		/**
		* @param RBIndex, �����µ�Ƶ�㣨RB)
		* @param cqi, ���ϱ���CQI(MCS�ȼ�)
		*/
		virtual void UpdateCQI( int RBIndex, const MCS& cqi ) = 0;
		
		//��ȡָ��RB��SINR
		/*
		 * @param RBIndex, Ҫ��ѯ��RB
		 * @return ��RB�ϵ�SINR
		 */
		virtual double GetSINR( int RBIndex ) const = 0;
		
		virtual const SINRLIST& GetSINRList() const = 0;

		//��ȡָ��RB��CQI
		/*
		* @param RBIndex, Ҫ��ѯ��RB
		* @return �ϱ���CQI
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
		///���캯��
		ReporterImp();

		///��������
		ReporterImp::~ReporterImp();

		///����ΪReport�ӿڵ�ʵ��
	public:
		//����SINR
		virtual void UpdateSINR(int RBIndex, double sinr ) ;

		//����CQI
		virtual void UpdateCQI( int RBIndex, const MCS& cqi );

		//��ȡָ����SINR
		virtual double GetSINR(int RBIndex ) const ;

		virtual const SINRLIST& GetSINRList() const;

		//��ȡָ����CQI����
		virtual const MCS& GetCQI(int RBIndex) const ;

		virtual const CQILIST& GetCQIList() const;
	protected:
		/**�����SINR��CQI�б�*/
		SINRLIST m_vSINRList;
		CQILIST m_vCQIList;
	};

};
