#include "commondef.h"
#include <cmath>
#include <complex>
#include <exception>
#include "Map.h"
#include "RandomManager.h"
#include "NodeB.h"
#include "UE.h"
#include "System.h"
#include "RunParameters.h"
#include "HSPANodeB.h"
#include "HSPAUE.h"
#include "WiFiAP.h"
#include "WiFiUE.h"
#include "LAANodeB.h"
#include "LAAUE.h"

namespace LTESim
{
	Map::Map(void)
	{
	}

	Map::~Map(void)
	{
	}
   double Map::Distance( const NodeB& CenterNodeB, const ILTEUE& ue ) const
	{	//ֱ����GetPos()�������Ϳ���
		POS pos = WrapAroundMap(CenterNodeB, ue);
		return ::Distance(pos, ue.GetPos());
	}

	double Map::Distance(const HSPANodeB& CenterNodeB, const IHSPAUE& ue) const
	{
		POS pos = WrapAroundMap(CenterNodeB, ue);
		return ::Distance(pos, ue.GetPos());
	}
 
	double Map::Distance(const WiFiAP& ap, const IWiFiUE& ue) const
	{
		return ::Distance(ap.GetPos(), ue.GetPos());
	}

	double Map::Distance(const LAANodeB& nb, const ILAAUE& ue) const
	{
		return ::Distance(nb.GetPos(), ue.GetPos());
	}

//HFR
	double Map::Distance(const LAANodeB& nb1, const LAANodeB& nb2) const
	{
		return ::Distance(nb1.GetPos(), nb2.GetPos());
	}

	double Map::Distance(const LAANodeB& nb,const WiFiAP& ap) const
	{
		return ::Distance(nb.GetPos(), ap.GetPos());
	}

	double Map::Distance(const WiFiAP& ap1,const WiFiAP& ap2) const
	{
		return ::Distance(ap1.GetPos(), ap2.GetPos());
	}

	double Map::Distance(const WiFiAP& ap1,const ILAAUE& ue) const//3.22����ap��laaue����ʱ��HFR
	{
		return ::Distance(ap1.GetPos(), ue.GetPos());
	}

	double Map::Distance( const LAANodeB& nb,const IWiFiUE& ue ) const//3.22����nb��wifiue����ʱ��HFR
	{
        return ::Distance(nb.GetPos(), ue.GetPos());
	}

//yishang HFR
	double Map::Angle( const NodeB& CenterNodeB, const ILTEUE& ue ) const
	{
		POS pos = WrapAroundMap(CenterNodeB, ue);
		return ::Angle(pos, ue.GetPos());
	}

	SevenCells::SevenCells()
	{
		m_nbx.resize(7);
		m_nby.resize(7);
		//��վ�����꣬��һ��ֵ
		m_nbx[0] = 0;
		m_nbx[1] = m_nbx[6] = 1.5;
		m_nbx[2] = m_nbx[5] = 0;
		m_nbx[3] = m_nbx[4] = -1.5;
		
		//��վ�����꣬��һ��ֵ
		m_nby[0] = 0;
		m_nby[1] = m_nby[3] = std::sqrt(3.0) / 2;
		m_nby[2] = std::sqrt(3.0);
		m_nby[4] = m_nby[6] = -std::sqrt(3.0) / 2;
		m_nby[5] = -std::sqrt(3.0);
		NBRadius = System::GetCurrentSystem()->GetConfig().GetInt("Inter_Site_Distance") * std::sqrt(3.0) / 3.0;
		
	}

	const POS SevenCells::GenerateUEPOS(/*UNIFORM_REAL_GEN& UEAngGen, UNIFORM_REAL_GEN& UEAmpGen,*/double radius) const
	{   
		//���������������
		UNIFORM_REAL_GEN UEPosGen = RandomManager::GetRandomManager().GetUniformRealGen(-radius, radius);	
		double x,y;
		//sqrt(x^2+y^2)Ҫ����35,����Cost-231 HATA�ı�׼
		do 
		{
			x=UEPosGen();
			y=UEPosGen();
		} while ( (x*x + y*y) < 35*35 || (x*x + y*y) > radius*radius );
		return POS(x,y);
	}

	const POS SevenCells::WrapAroundMap( const NodeB& CenterNodeB, const ILTEUE& ue ) const
	{
		//��һ������ӳ���
		const static POS postemp[7][7] = {{POS(0, 0),POS(1.5, 1.73205/2),POS(0, 1.73205),POS(-1.5, 1.73205/2),POS(-1.5, -1.73205/2),
			                               POS(0, -1.73205), POS(1.5, -1.73205/2)},
		                                  {POS(0, 0), POS(1.5, 1.73205/2),POS(0, 1.73205),POS(3,0),POS(1.5, 3*1.73205/2),
										   POS(3, 1.73205),POS(1.5, -1.73205/2)},
		                                  {POS(0, 0),POS(1.5, 1.73205/2),POS(0, 1.73205),POS(-1.5, 1.73205/2),POS(1.5, 3*1.73205/2),
										   POS(-1.5, 3*1.73205/2),POS(0, 2*1.73205)},
		                                  {POS(0, 0),POS(-3, 1.73205),POS(0, 1.73205),POS(-1.5, 1.73205/2),POS(-1.5, -1.73205/2),
										   POS(-1.5, 3*1.73205/2),POS(-3, 0)},
		                                  {POS(0, 0),POS(-1.5, -3*1.73205/2),POS(-3, -1.73205),POS(-1.5, 1.73205/2),POS(-1.5, -1.73205/2),
										   POS(0, -1.73205),POS(-3,0)},
		                                  {POS(0, 0),POS(-1.5, -3*1.73205/2),POS(1.5, -3*1.73205/2),POS(0, -2*1.73205),POS(-1.5, -1.73205/2),
										   POS(0, -1.73205),POS(1.5, -1.73205/2)},
		                                  {POS(0, 0),POS(1.5, 1.73205/2),POS(1.5, -3*1.73205/2),POS(3, 0),POS(3, -1.73205),
										   POS(0, -1.73205),POS(1.5, -1.73205/2)}};
		//��ӳ����ʵ������
		if (shared_ptr<NodeB> NBptr = ue.GetServingNodeB())
		{
			shared_ptr<System> pSystem = System::GetCurrentSystem();
			int iNBOffsetID = pSystem->ConvertNBAbsoluteIDToOffsetID( NBptr->GetID(), SYSTEM_LTE );
			int iCenterNBOffsetID = pSystem->ConvertNBAbsoluteIDToOffsetID( CenterNodeB.GetID(), SYSTEM_LTE );

			POS MapValue(postemp[iNBOffsetID/*UE����С��*/][iCenterNBOffsetID/*��Ҫӳ���NodeB*/].GetX()*NBRadius, 
									postemp[iNBOffsetID][iCenterNBOffsetID].GetY()*NBRadius);
			return MapValue;
			
		} 
		else
		{
			throw std::logic_error("��UE��û�н���");
		}
		
	}

	const POS SevenCells::WrapAroundMap( const HSPANodeB& CenterNodeB, const IHSPAUE& ue ) const
	{
		//��һ������ӳ���
		const static POS postemp[7][7] = {{POS(0, 0),POS(1.5, 1.73205/2),POS(0, 1.73205),POS(-1.5, 1.73205/2),POS(-1.5, -1.73205/2),
			POS(0, -1.73205), POS(1.5, -1.73205/2)},
		{POS(0, 0), POS(1.5, 1.73205/2),POS(0, 1.73205),POS(3,0),POS(1.5, 3*1.73205/2),
		POS(3, 1.73205),POS(1.5, -1.73205/2)},
		{POS(0, 0),POS(1.5, 1.73205/2),POS(0, 1.73205),POS(-1.5, 1.73205/2),POS(1.5, 3*1.73205/2),
		POS(-1.5, 3*1.73205/2),POS(0, 2*1.73205)},
		{POS(0, 0),POS(-3, 1.73205),POS(0, 1.73205),POS(-1.5, 1.73205/2),POS(-1.5, -1.73205/2),
		POS(-1.5, 3*1.73205/2),POS(-3, 0)},
		{POS(0, 0),POS(-1.5, -3*1.73205/2),POS(-3, -1.73205),POS(-1.5, 1.73205/2),POS(-1.5, -1.73205/2),
		POS(0, -1.73205),POS(-3,0)},
		{POS(0, 0),POS(-1.5, -3*1.73205/2),POS(1.5, -3*1.73205/2),POS(0, -2*1.73205),POS(-1.5, -1.73205/2),
		POS(0, -1.73205),POS(1.5, -1.73205/2)},
		{POS(0, 0),POS(1.5, 1.73205/2),POS(1.5, -3*1.73205/2),POS(3, 0),POS(3, -1.73205),
		POS(0, -1.73205),POS(1.5, -1.73205/2)}};
		//��ӳ����ʵ������
		if (shared_ptr<HSPANodeB> NBptr = ue.GetServingHSPANodeB())
		{
			shared_ptr<System> pSystem = System::GetCurrentSystem();
			int iNBOffsetID = pSystem->ConvertNBAbsoluteIDToOffsetID( NBptr->GetID(), SYSTEM_HSPA );
			int iCenterNBOffsetID = pSystem->ConvertNBAbsoluteIDToOffsetID( CenterNodeB.GetID(), SYSTEM_HSPA );
			POS MapValue(postemp[iNBOffsetID/*UE����С��*/][iCenterNBOffsetID/*��Ҫӳ���NodeB*/].GetX()*NBRadius, 
									postemp[iNBOffsetID][iCenterNBOffsetID].GetY()*NBRadius);

			return MapValue;
		} 
		else
		{
			throw std::logic_error("��UE��û�н���");
		}

	}


	const double SevenCells::GetNBRadius() const
	{
		return NBRadius;
	}

	CircleCell::CircleCell()
	{
        m_nbx.resize(4);
        m_nby.resize(4);
        //��վ������
        m_nbx[0] = -43;
        m_nbx[1] = -13;
        m_nbx[2] = 17;
        m_nbx[3] = 47;

        //��վ������
        m_nby[0] = m_nby[1] = m_nby[2] = m_nby[3] = 25;
        CircleRadius = System::GetCurrentSystem()->GetConfig().GetInt("Circle_Cell_Radius") ;
	}

	CircleCell::~CircleCell()
	{

	}

	const POS CircleCell::GenerateUEPOS(double radius) const
	{
		//���������������
		UNIFORM_REAL_GEN UEPosGen = RandomManager::GetRandomManager().GetUniformRealGen(-radius, radius);	
		double x,y;
		do 
		{
			x=UEPosGen();
			y=UEPosGen();
		} while ( (x*x + y*y) >= radius * radius);
		return POS(x,y);
	}

	const double CircleCell::GetCircleRadius() const
	{
		return CircleRadius;
	}

	const POS CircleCell::WrapAroundMap(const NodeB& CenterNodeB, const ILTEUE& ue) const
	{
		return POS(0, 0);
	}

	const POS CircleCell::WrapAroundMap( const HSPANodeB& CenterNodeB, const IHSPAUE& ue ) const
	{
		return POS(0, 0);
	}

	Indoor::Indoor()
    {
        m_nbx.resize(4);
        m_nby.resize(4);
        //��վ������
        m_nbx[0] = -47;
        m_nbx[1] = -17;
        m_nbx[2] = 13;
        m_nbx[3] = 43;

        //��վ������
        m_nby[0] = m_nby[1] = m_nby[2] = m_nby[3] = 25;
        IndoorRadius = System::GetCurrentSystem()->GetConfig().GetInt("Circle_Cell_Radius") ;
    }
    const POS Indoor::GenerateUEPOS(double radius) const
    {
        //���������������
        UNIFORM_REAL_GEN UEPosGen = RandomManager::GetRandomManager().GetUniformRealGen(-radius, radius);    
        double x,y;
        do 
        {
            x=UEPosGen();
            y=UEPosGen();
        } while ( (x*x + y*y) >= radius * radius);
        return POS(x,y);
    }


    const POS Indoor::WrapAroundMap(const NodeB& CenterNodeB, const ILTEUE& ue) const
    {
        return POS(0, 0);
    }

    const POS Indoor::WrapAroundMap( const HSPANodeB& CenterNodeB, const IHSPAUE& ue ) const
    {
        return POS(0, 0);
    }
}

