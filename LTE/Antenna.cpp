#include "Antenna.h"
namespace LTESim
{
    Antenna::Antenna(void):m_iAntennaPortNum(0)
    {

    }

    Antenna::~Antenna(void)
    {
    }
    //获得天线个数
    int Antenna::GetAntennaPortNum() const
    {
        return m_iAntennaPortNum;
    }
    //设置天线个数
    void Antenna::SetAntennaPortNum( int antennaPortNum )
    {
        m_iAntennaPortNum = antennaPortNum;
    }
}
