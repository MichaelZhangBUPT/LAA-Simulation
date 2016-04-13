#pragma once
#include "command.h"

namespace LTESim
{
	class ILTEUE;
	class NodeB;

    ///更新某个NodeB当前发射时刻每个RB上对应的UE
	class UpdateUEForRBCommand :
		public LTESim::Command
	{
	public:
        ///构造函数
        /*
        * @param pNodeB，要更新的NodeB
        * @param UEList，要更新的每个RB对应UE的列表
        */
		UpdateUEForRBCommand(shared_ptr<NodeB> pNodeB, const vector< shared_ptr<ILTEUE> >& UEList);
	public:
		~UpdateUEForRBCommand(void);

        ///Do函数
        /*
        * 更新NodeB内部每个RB对应的UE
        */
		void Do(const Clock& clk);
	private:
        /**要更新的NodeB*/
		weak_ptr<NodeB> m_nodeb;
        /**要更新到的RB对UE的列表*/
		vector< shared_ptr<ILTEUE> > m_UEToRBlist;
	}; 
}
