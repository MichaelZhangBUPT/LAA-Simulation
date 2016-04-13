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
//����System���UE��NodeB
shared_ptr<System> SystemElement::Attach( shared_ptr<System> pNewSys )
{
	//����Ƿ�Ϊ�գ���Ϊ�׾ͼӵ�SystemElement������
	if (!pNewSys)
	{
		throw std::invalid_argument("Can't attach to a null system");
	}
	shared_ptr<System> temp(m_pSystem.lock());
	m_pSystem = pNewSys;
	return temp;
}
}
