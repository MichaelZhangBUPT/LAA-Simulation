#include "commondef.h"
#include <boost/foreach.hpp>
#include "SmartAntenna.h"
#include "System.h"
#include "NodeB.h"
#include "UE.h"
#include "Map.h"
namespace LTESim
{
    size_t SmartAntenna::ms_UENum = 0;
    size_t SmartAntenna::ms_NobeBNum = 0;
    std::vector<double> SmartAntenna::ms_SAGainArray;
    std::vector< SmartAntenna::GainsForOneNodeB > SmartAntenna::ms_CalculatedGains;
    SmartAntenna::SmartAntenna(double antennaGain)
    : m_antennaGain(antennaGain)
    {
    }

    SmartAntenna::~SmartAntenna(void)
    {
    }

    double SmartAntenna::GetSAGain( const NodeB& CenterNodeB, const ILTEUE& target, const ILTEUE& neighbor ) const
    {
        double& gain = ms_CalculatedGains[CenterNodeB.GetID()][target.GetID()][neighbor.GetID()];
        if (gain < MAXSAGAIN)
        {
            return gain;
        }
        if (shared_ptr<System> pSystem = System::GetCurrentSystem())
        {
            const Map& map = pSystem->GetMap();
            double ang0 = map.Angle(CenterNodeB, target);
            double ang1 = map.Angle(CenterNodeB, neighbor);
            int angleDiff = (int)fabs(ang0 - ang1);
            gain = GetSAGain(angleDiff);
        }   
        return gain;
    }

    double SmartAntenna::GetSAGain( int angle ) const
    {
        angle += ZERO_ANGLE_OFFSET;
        while (angle < 0)
        {
            angle += 360;
        }
        angle %= 360;
        return ms_SAGainArray[angle]/ ms_SAGainArray[ZERO_ANGLE_OFFSET];
    }

    int SmartAntenna::GetAntennaPortNum() const
    {
        /*智能天线天线端口只有一个，物理天线有4~8根*/
        return 1;
    }

    double SmartAntenna::GetAntennaGain() const
    {
        return m_antennaGain * ms_SAGainArray[ZERO_ANGLE_OFFSET];
    }

    void SmartAntenna::InitSAGainArray( const std::vector<double>& SAGainArray, int NBNum, int UENum )
    {
        if (SAGainArray.size() != 360)
        {
            throw std::invalid_argument("SA Gain Array must have 360 elements");
        }
        ms_NobeBNum = NBNum;
        ms_UENum = UENum;
        ms_SAGainArray = SAGainArray;
        OnPosUpdated();
        return;
    }

    void SmartAntenna::OnPosUpdated()
    {
        ms_CalculatedGains.resize(ms_NobeBNum);
        BOOST_FOREACH(GainsForOneNodeB& gainsForOneNodeB, ms_CalculatedGains)
        {
            gainsForOneNodeB.resize(ms_UENum);
            BOOST_FOREACH(GainsForOneUE& gainsForOneNodeBOneUE, gainsForOneNodeB)
            {
                gainsForOneNodeBOneUE.clear();
                gainsForOneNodeBOneUE.resize(ms_UENum, 
                    MAXSAGAIN + 100.0/*随便加一个正数确保是一个无效值*/);
            }
        }
    }

    void SmartAntenna::CombineSINR( const std::vector< std::vector<double> >& SINRPerRBPerTxAntenna, std::vector<double>& SINRPerRB ) const
    {
        /*在分集的情况下，将相同RB index的不同发射天线上的SINR合并MIMO发送分集*/
        for(unsigned i = 0; i < SINRPerRBPerTxAntenna.size(); ++i)
        {
            double sinr = SINRPerRBPerTxAntenna[i][0];
            SINRPerRB.push_back(sinr);
        }
    }

    void SmartAntenna::SetAntennaPortNum( int antennaPortNum )
    {
        assert(false);
        return;
    }
}
