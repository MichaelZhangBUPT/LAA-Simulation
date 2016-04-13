#include "commondef.h"
#include <limits>
#include <stdexcept>
#include <boost/bind.hpp>
#include "CellSearchCommand.h"
#include "AccessPriorityCalculator.h"
#include "System.h"
#include "NodeB.h"
#include "UE.h"
#include "HSPANodeB.h"
#include "HSPAUE.h"
#include "Fading.h"
#include "Pathloss.h"
#include "SimLog.h"
#include "WiFiAP.h"
#include "WiFiUE.h"
#include "LAANodeB.h"
#include "LAAUE.h"
#include "RunParameters.h"

namespace LTESim
{

	class AccessByDist : public AccessPriorityCalculator
	{
	public:
		AccessByDist(const UEPTR& pUE) : AccessPriorityCalculator(pUE)
		{

		}

		AccessByDist(const HSPAUEPTR& pUE) : AccessPriorityCalculator(pUE)
		{

		}

		AccessByDist(const WiFiUEPTR& pUE) : AccessPriorityCalculator(pUE)
		{

		}

		AccessByDist(const LAAUEPTR& pUE) : AccessPriorityCalculator(pUE)
		{

		}

		bool operator() (const NBPTR& pNB1, const NBPTR& pNB2)
		{
			if (shared_ptr<ILTEUE> pUE = m_pLTEUE.lock())	//判断是否有UE需要接入
			{
				//将UE与基站的距离作为接入权值
				double dist1 = Distance(pUE->GetPos(), pNB1->GetPos());
				double dist2 = Distance(pUE->GetPos(), pNB2->GetPos());
				return dist1 < dist2;
			}
			throw std::logic_error("UE is not exist any more!");
		}

		bool operator()(const HSPANBPTR& pNB1, const HSPANBPTR& pNB2)
		{
			if (shared_ptr<IHSPAUE> pUE = m_pHSPAUE.lock())	//判断是否有UE需要接入
			{
				//将UE与基站的距离作为接入权值
				double dist1 = Distance(pUE->GetPos(), pNB1->GetPos());
				double dist2 = Distance(pUE->GetPos(), pNB2->GetPos());
				return dist1 < dist2;
			}
			throw std::logic_error("UE is not exist any more!");
		}

		bool operator()(const WiFiAPPTR& pAP1, const WiFiAPPTR& pAP2)
		{
			if (shared_ptr<IWiFiUE> pUE = m_pWiFiUE.lock())	//判断是否有UE需要接入
			{
				//将UE与基站的距离作为接入权值
				double dist1 = Distance(pUE->GetPos(), pAP1->GetPos());
				double dist2 = Distance(pUE->GetPos(), pAP2->GetPos());
				return dist1 < dist2;
			}
			throw std::logic_error("UE is not exist any more!");
		}

		bool operator()(const LAANBPTR& pNB1, const LAANBPTR& pNB2)
		{
			if (shared_ptr<ILAAUE> pUE = m_pLAAUE.lock())	//判断是否有UE需要接入
			{
				//将UE与基站的距离作为接入权值
				double dist1 = Distance(pUE->GetPos(), pNB1->GetPos());
				double dist2 = Distance(pUE->GetPos(), pNB2->GetPos());
				return dist1 < dist2;
			}
			throw std::logic_error("UE is not exist any more!");
		}
	};

	class AccessByReferencePower : public AccessPriorityCalculator
	{
	public:
		AccessByReferencePower(const UEPTR& pUE):AccessPriorityCalculator(pUE)
		{

		}

		AccessByReferencePower(const HSPAUEPTR& pUE) : AccessPriorityCalculator(pUE)
		{

		}

		bool operator() (const NBPTR& pNB1, const NBPTR& pNB2)
		{
			if (shared_ptr<ILTEUE> pUE = m_pLTEUE.lock())	
			{
				//将UE与基站的路损和慢衰作为接入权值
				if (shared_ptr<System> pSystem = System::GetCurrentSystem())
				{
					const Pathloss& pathloss = pSystem->GetPathLoss(SYSTEM_LTE);
					double pathloss1 = pathloss(Distance(pNB1->GetPos(), pUE->GetPos()));
					double pathloss2 = pathloss(Distance(pNB2->GetPos(), pUE->GetPos()));
					const Fading& fading = pSystem->GetSlowFading();
					double fading1 = fading.GetFading(pNB1->GetID(), pUE->GetID(), 0, 0, 0);
					double fading2 = fading.GetFading(pNB2->GetID(), pUE->GetID(), 0, 0, 0);
					return pathloss1 * fading1 < pathloss2 * fading2;
				}
				throw std::logic_error("System is not valid!");
			}
			throw std::logic_error("UE is not exist any more!");
		}

		bool operator() (const HSPANBPTR& pNB1, const HSPANBPTR& pNB2)
		{
			if (shared_ptr<IHSPAUE> pUE = m_pHSPAUE.lock())	
			{
				//将UE与基站的路损和慢衰作为接入权值
				if (shared_ptr<System> pSystem = System::GetCurrentSystem())
				{
					const Pathloss& pathloss = pSystem->GetPathLoss(SYSTEM_HSPA);
					double pathloss1 = pathloss(Distance(pNB1->GetPos(), pUE->GetPos()));
					double pathloss2 = pathloss(Distance(pNB2->GetPos(), pUE->GetPos()));
					const Fading& fading = pSystem->GetSlowFading();
					double fading1 = fading.GetFading(pNB1->GetID(), pUE->GetID(), 0, 0, 0);
					double fading2 = fading.GetFading(pNB2->GetID(), pUE->GetID(), 0, 0, 0);
					return pathloss1 * fading1 < pathloss2 * fading2;
				}
				else
				{
					throw std::runtime_error( "Get System Error." );
				}
			}
			else
			{
				throw std::runtime_error( "Get HSPA UE Error" );
			}
		}
	};


	CellSearchCommand::CellSearchCommand(UEPTR pUE)
			: m_pUE(pUE)
	{

	}

	CellSearchCommand::~CellSearchCommand(void)
	{
	}

	void CellSearchCommand::Do(const Clock& clk)
	{
		std::vector<NodeB::PTR> nblisttoaccess;
		PrepareNBList(nblisttoaccess);
		//按照接入权值，即距离排序
		sort(nblisttoaccess.begin(), nblisttoaccess.end(), AccessByDist(m_pUE.lock()));
		std::vector<NodeB::PTR>::iterator iter = nblisttoaccess.begin();
		while (!AttemptToAccess(*iter))
		{
			++iter;
		}
		if (iter == nblisttoaccess.end())
		{
			FailedToAccess();
		}
		else
		{
			SuccessToAccess(*iter);
			//接入成功之后，计算din
			shared_ptr<System> pSystem = System::GetCurrentSystem();
			double CircleRadius = pSystem->GetConfig().GetInt("Circle_Cell_Radius");
			int id = m_pUE.lock()->GetServingNodeB()->GetID();
			shared_ptr<WiFiAP> pAP = pSystem->GetWiFiAPFromOffsetID(id);
			double din = std::max(CircleRadius-::Distance(pAP->GetPos(),m_pUE.lock()->GetPos()), 0.0);
			m_pUE.lock()->SetDin(din);
		}
	}

	void CellSearchCommand::PrepareNBList( std::vector<NodeB::PTR>& nblisttoaccess )
	{
		 if (ILTEUE::PTR pUE = m_pUE.lock())
		{
			//取得UE的候选集NB列表准备接纳
			const ILTEUE::NBLIST& nbList = pUE->GetCandidateNodeBSet();
			ILTEUE::NBLIST::const_iterator iter = nbList.begin();
			ILTEUE::NBLIST::const_iterator end = nbList.end();
			std::transform(nbList.begin(), nbList.end(), std::inserter(nblisttoaccess, nblisttoaccess.end()), 
				boost::bind(&ILTEUE::NBLIST::value_type::second, _1));
		}
	}

	bool CellSearchCommand::AttemptToAccess( const NodeB::PTR& pNodeB )
	{
		if (ILTEUE::PTR pUE = m_pUE.lock())	//判断UE是否存在
		{
			if (pNodeB->RequireAccess(pUE))
			{
				return true;
			}
		}
		return false;
	}

	void CellSearchCommand::FailedToAccess()
	{
		throw UnFinishedFunc();
		//处理所有NodeB都不能接纳的情况，如往ue中加入新的一个接入command
	}

	void CellSearchCommand::SuccessToAccess( const NodeB::PTR& pNodeB )
	{
		//接入成功，将NodeB加入激活集
		if (ILTEUE::PTR pUE = m_pUE.lock())
		{
			pUE->AddToActiveSet(pNodeB);
			SimLog::Instance()->LogAccessInformation( pUE->GetID(), pUE->GetPos(), pNodeB->GetID(), pNodeB->GetPos(), Distance(pUE->GetPos(), pNodeB->GetPos()) );
		}
		else
		{
			throw std::runtime_error("UE pointer is invalid.");
		}
	}

	HSPACellSearchCommand::HSPACellSearchCommand( shared_ptr<IHSPAUE> pUE ) : m_pMasterUE( pUE )
	{
		
	}

	void HSPACellSearchCommand::Do( const Clock& clk )
	{
		std::vector<shared_ptr<HSPANodeB>> candNBVec;
		PrepareNBList( candNBVec );
		sort( candNBVec.begin(), candNBVec.end(), AccessByDist( m_pMasterUE.lock() ) );
		//for ( std::vector<shared_ptr<HSPANodeB>>::iterator vecIter = candNBVec.begin(); vecIter != candNBVec.end(); ++vecIter )
		//{
		//	SimLog::Instance()->LogAccessInformation( m_pMasterUE.lock()->GetID(), m_pMasterUE.lock()->GetPos(), (*vecIter)->GetID(), (*vecIter)->GetPos(), Distance(m_pMasterUE.lock()->GetPos(), (*vecIter)->GetPos()) );
		//}
		//SimLog::Instance()->LogAccessInformation( 0, POS(), 0, POS(), 0 );
		for ( std::vector<shared_ptr<HSPANodeB>>::iterator vecIter = candNBVec.begin(); vecIter != candNBVec.end(); ++vecIter )
		{
			if ( AttemptToAccess( *vecIter ) )
			{
				SuccessToAccess( *vecIter );
				return;
			}
		}
		FailedToAccess();
	}

	void HSPACellSearchCommand::PrepareNBList( std::vector<shared_ptr<HSPANodeB>>& refNBList )
	{
		refNBList.clear();
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const System::HSPANBLIST& refHSPANBList = pSystem->GetHSPANBList();
		for ( System::HSPANBLIST::const_iterator nbListIter = refHSPANBList.begin(); nbListIter != refHSPANBList.end(); ++nbListIter )
		{
			refNBList.push_back( nbListIter->second );
		}
	}

	bool HSPACellSearchCommand::AttemptToAccess( shared_ptr<HSPANodeB> pNodeB )
	{
		return true;
	}

	void HSPACellSearchCommand::SuccessToAccess( shared_ptr<HSPANodeB> pNodeB )
	{
		if ( shared_ptr<IHSPAUE> pUE = m_pMasterUE.lock() )
		{
			pNodeB->AddUE( pUE );
			pUE->AddToActiveSet( pNodeB );

			SimLog::Instance()->LogAccessInformation( pUE->GetID(), pUE->GetPos(), pNodeB->GetID(), pNodeB->GetPos(), Distance(pUE->GetPos(), pNodeB->GetPos()) );
			//string lineone( "UEID\tNodeBID\tUE.X\tUE.Y\tNB.X\tNB.Y\tDistance" );
			//vector<int> iVec;
			//vector<double> dVec;
			//iVec.push_back( pUE->GetID() );	
			//iVec.push_back( pNodeB->GetID() );
			//dVec.push_back( pUE->GetPos().GetX() );
			//dVec.push_back( pUE->GetPos().GetY() );
			//dVec.push_back( pNodeB->GetPos().GetX() );
			//dVec.push_back( pNodeB->GetPos().GetY() );
			//dVec.push_back( Distance(pUE->GetPos(), pNodeB->GetPos()) );
			//SimLog::Instance()->Log( "AccessInformationNew", lineone, iVec, dVec );
		}
		else
		{
			throw std::runtime_error("HSPA UE pointer is invalid.");
		}
	}

	void HSPACellSearchCommand::FailedToAccess()
	{

	}


	WiFiCellSearchCommand::WiFiCellSearchCommand(shared_ptr<IWiFiUE> pUE):m_pMasterUE(pUE)//?
	{

	}

	void WiFiCellSearchCommand::Do( const Clock& clk )
	{
		std::vector<shared_ptr<WiFiAP>> candAPVec;
		PrepareAPList( candAPVec );
		sort( candAPVec.begin(), candAPVec.end(), AccessByDist( m_pMasterUE.lock() ) );
		for ( std::vector<shared_ptr<WiFiAP>>::iterator vecIter = candAPVec.begin(); vecIter != candAPVec.end(); ++vecIter )
		{
			if ( AttemptToAccess( *vecIter ) )
			{
				SuccessToAccess( *vecIter );
				return;
			}
		}
		FailedToAccess();
	}

	void WiFiCellSearchCommand::PrepareAPList( std::vector<shared_ptr<WiFiAP>>& refAPList )
	{
		refAPList.clear();
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const System::WiFiAPLIST& refWiFiAPList = pSystem->GetWiFiAPList();
		for ( System::WiFiAPLIST::const_iterator apListIter = refWiFiAPList.begin(); apListIter != refWiFiAPList.end(); ++apListIter )
		{
			refAPList.push_back( apListIter->second );
		}
	}

	bool WiFiCellSearchCommand::AttemptToAccess( shared_ptr<WiFiAP> pAP )
	{
		return true;
	}

	void WiFiCellSearchCommand::SuccessToAccess( shared_ptr<WiFiAP> pAP )
	{
		if ( shared_ptr<IWiFiUE> pUE = m_pMasterUE.lock() )
		{
			pAP->AddUE( pUE );
			pUE->AddToActiveAP( pAP );
			SimLog::Instance()->LogAccessInformation( pUE->GetID(), pUE->GetPos(), pAP->GetID(), pAP->GetPos(), Distance(pUE->GetPos(), pAP->GetPos()) );
		}
		else
		{
			throw std::runtime_error("WiFi UE pointer is invalid.");
		}
	}

	void WiFiCellSearchCommand::FailedToAccess()
	{

	}

	LAACellSearchCommand::LAACellSearchCommand(shared_ptr<ILAAUE> pUE):m_pMasterUE(pUE)
	{

	}

	void LAACellSearchCommand::Do( const Clock& clk )
	{
		std::vector<shared_ptr<LAANodeB>> candNBVec;
		PrepareNBList( candNBVec );
		sort( candNBVec.begin(), candNBVec.end(), AccessByDist( m_pMasterUE.lock() ) );
		for ( std::vector<shared_ptr<LAANodeB>>::iterator vecIter = candNBVec.begin(); vecIter != candNBVec.end(); ++vecIter )
		{
			if ( AttemptToAccess( *vecIter ) )
			{
				SuccessToAccess( *vecIter );
				return;
			}
		}
		FailedToAccess();
	}

	void LAACellSearchCommand::PrepareNBList( std::vector<shared_ptr<LAANodeB>>& refNBList )
	{
		refNBList.clear();
		shared_ptr<System> pSystem = System::GetCurrentSystem();
		const System::LAANBLIST& refLAANBList = pSystem->GetLAANBList();
		for ( System::LAANBLIST::const_iterator nbListIter = refLAANBList.begin(); nbListIter != refLAANBList.end(); ++nbListIter )
		{
			refNBList.push_back( nbListIter->second );
		}
	}

	bool LAACellSearchCommand::AttemptToAccess( shared_ptr<LAANodeB> pNB )
	{
		return true;
	}

	void LAACellSearchCommand::SuccessToAccess( shared_ptr<LAANodeB> pNB )
	{
		if ( shared_ptr<ILAAUE> pUE = m_pMasterUE.lock() )
		{
			pNB->AddUE( pUE );
			pUE->AddToActiveNB( pNB );
			SimLog::Instance()->LogAccessInformation( pUE->GetID(), pUE->GetPos(), pNB->GetID(), pNB->GetPos(), Distance(pUE->GetPos(), pNB->GetPos()) );
		}
		else
		{
			throw std::runtime_error("LAA UE pointer is invalid.");
		}
	}

	void LAACellSearchCommand::FailedToAccess()
	{

	}
}
