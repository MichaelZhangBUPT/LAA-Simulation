#pragma once
#include "commondef.h"
#include <iostream>
#include <fstream>

namespace LTESim
{
	//std::ofstream fout("../Output/C2I.txt");

	class Service;
	class HSPANodeB;

	class ITDScheduler
	{
	public:
		typedef std::vector<shared_ptr<Service> > SERVICELIST;
	public:
		ITDScheduler(){};
		virtual ~ITDScheduler(){};
	public:
		virtual SERVICELIST Process() = 0;
	};

	class HSPATDDownlinkScheduler
		: public ITDScheduler
	{
	public:
		typedef std::vector<shared_ptr<Service> > SERVICELIST;
	public:
		HSPATDDownlinkScheduler( shared_ptr<HSPANodeB> pNodeB );
		virtual ~HSPATDDownlinkScheduler();
	public:
		virtual SERVICELIST Process();
	protected:
		void Clear();
		void GetCandidateServiceList();
		void PrepareServicePriorityList();
		void SelectRetransUsers();
		void SelectUsersAccordingToPriorityAfterReservation();
		virtual double CalculatePriority( shared_ptr<Service> pService ) = 0;
	protected:
		struct ServicePriority
		{
			shared_ptr<Service>	m_pService;
			double							m_dPriority;
			bool operator > ( const ServicePriority& refServicePriority )const { return m_dPriority > refServicePriority.m_dPriority; }
		};
	protected:
		int											m_iMaxSelectedUENum;
		SERVICELIST								m_SelectedServiceList;
		SERVICELIST								m_CandServiceList;
		std::vector<ServicePriority>		m_ServicePriorityVec;
		//std::ofstream m_fout;
		//std::vector<double>			m_C2I;
		//std::vector<double>			m_AverThr;
		weak_ptr<HSPANodeB>			m_pMasterNodeB;

	};
}