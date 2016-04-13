#pragma once
#include "command.h"

namespace LTESim
{
	class ILTEUE;
	class NodeB;

    ///����ĳ��NodeB��ǰ����ʱ��ÿ��RB�϶�Ӧ��UE
	class UpdateUEForRBCommand :
		public LTESim::Command
	{
	public:
        ///���캯��
        /*
        * @param pNodeB��Ҫ���µ�NodeB
        * @param UEList��Ҫ���µ�ÿ��RB��ӦUE���б�
        */
		UpdateUEForRBCommand(shared_ptr<NodeB> pNodeB, const vector< shared_ptr<ILTEUE> >& UEList);
	public:
		~UpdateUEForRBCommand(void);

        ///Do����
        /*
        * ����NodeB�ڲ�ÿ��RB��Ӧ��UE
        */
		void Do(const Clock& clk);
	private:
        /**Ҫ���µ�NodeB*/
		weak_ptr<NodeB> m_nodeb;
        /**Ҫ���µ���RB��UE���б�*/
		vector< shared_ptr<ILTEUE> > m_UEToRBlist;
	}; 
}
