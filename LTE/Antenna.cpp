#include "Antenna.h"
namespace LTESim
{
    Antenna::Antenna(void):m_iAntennaPortNum(0)
    {

    }

    Antenna::~Antenna(void)
    {
    }
    //������߸���
    int Antenna::GetAntennaPortNum() const
    {
        return m_iAntennaPortNum;
    }
    //�������߸���
    void Antenna::SetAntennaPortNum( int antennaPortNum )
    {
        m_iAntennaPortNum = antennaPortNum;
    }
}
