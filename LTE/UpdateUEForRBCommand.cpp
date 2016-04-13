#include "commondef.h"
#include "UpdateUEForRBCommand.h"
#include "NodeB.h"

namespace LTESim
{

    UpdateUEForRBCommand::UpdateUEForRBCommand(shared_ptr<NodeB> pnodeb, const vector< shared_ptr<ILTEUE> >& UEToRBList)
		:m_nodeb(pnodeb), m_UEToRBlist(UEToRBList)
    {
    }

    UpdateUEForRBCommand::~UpdateUEForRBCommand(void)
    {
    }

    void LTESim::UpdateUEForRBCommand::Do( const Clock& clk )
    {
        //UE���ڣ��������
        if (shared_ptr<NodeB> pNodeb = m_nodeb.lock())
        {
            pNodeb->UpdateUEForRB(m_UEToRBlist);
            return;
        }
    }
}