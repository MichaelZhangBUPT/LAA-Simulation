#include "commondef.h"
#include "ResManager.h"
namespace LTESim
{

	ResManager::ResManager(void)
	{
	}

	ResManager::~ResManager(void)
	{
	}

	bool ResManager::Require( const PhyResourceList& reslit ) const
	{
		//����Դ�б�����Ϊ��λ������Դ����Ϊһ���û��������ԴӦΪһ�ж���ֻ��һ��
		for( PhyResourceList::const_iterator iter = reslit.begin(); iter != reslit.end(); ++iter)
		{
			if (!Require(*iter))
			{
				return false;
			}
		}
		return true;
	}

	bool ResManager::Assign( const PhyResourceList& reslit )
	{
		if (!Require(reslit))
		{
			return false;
		}
		//require��Ӧ�����з�����ɳɹ�����˲����ж�
		for( PhyResourceList::const_iterator i = reslit.begin(); i != reslit.end(); ++i)
		{
			Assign(*i);
		}
		return true;
	}
}
