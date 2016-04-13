#include "commondef.h"
#include "SystemElement.h"

namespace LTESim
{

SystemElement::SystemElement( int id, const POS& pos )
:m_ID(id), m_pos(pos)
{

}
SystemElement::~SystemElement(void)
{
}
//连接System类和UE与NodeB
shared_ptr<System> SystemElement::Attach( shared_ptr<System> pNewSys )
{
	//检测是否为空，不为孔就加到SystemElement对象中
	if (!pNewSys)
	{
		throw std::invalid_argument("Can't attach to a null system");
	}
	shared_ptr<System> temp(m_pSystem.lock());
	m_pSystem = pNewSys;
	return temp;
}
}
