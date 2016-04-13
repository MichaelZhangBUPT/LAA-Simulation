#pragma once

#include "commondef.h"
#include "HSPAChannel.h"
#include <vector>
#include <map>

namespace LTESim
{
	class HSPANodeB;
	class Service;

	class IFDScheduler
	{
	public:
		typedef std::vector<shared_ptr<Service>> SERVICELIST;
		typedef std::map<shared_ptr<Service>, std::vector<int>> SERVICECODELISTMAP;
	public:
		IFDScheduler(){};
		virtual ~IFDScheduler(){};
	public:
		virtual void Process( SERVICELIST& refServiceList ) = 0;
	};

	class HSPAFDDownlinkScheduler
		: public IFDScheduler
	{
	public:
		typedef std::vector<shared_ptr<Service>> SERVICELIST;
	public:
		HSPAFDDownlinkScheduler( shared_ptr<HSPANodeB> pNodeB );
		virtual ~HSPAFDDownlinkScheduler();
	public:
		virtual void Process( SERVICELIST& refServiceList );
	protected:
		void Clear();
		void GetCandidateServiceList( const SERVICELIST& refServiceList );

		void AllocateCodeChannelToService();

		void	InitChannels();
		void	AddScheduledChannelsToTransmitter();

		//和时域调度的时候优先级计算相关的参数 To Be Added，除了考虑码道，还需要考虑什么？
		virtual double CalculatePriority( shared_ptr<Service> pService, int iCodeIndex ) = 0;
	protected:
		struct ServicePriority
		{
			shared_ptr<Service>		m_pService;
			int									m_iCode;
			double								m_dPriority;
			bool operator > ( const ServicePriority& refServicePriority )const { return m_dPriority > refServicePriority.m_dPriority; }
		};
	protected:
		SERVICELIST														m_CandServiceList;
		SERVICECODELISTMAP									m_ServiceCodeMap;
		std::map<shared_ptr<HSPAChannel>, int>		m_ScheduledChannelMap;

		int																	m_iTotalCodeNum;
		int																	m_iCodeNumPerUE;

		weak_ptr<HSPANodeB>									m_pMasterNodeB;
	};
}