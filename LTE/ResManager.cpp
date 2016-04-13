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
		//以资源列表整体为单位申请资源，因为一个用户申请的资源应为一列而不只是一个
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
		//require后应当所有分配均可成功，因此不再判断
		for( PhyResourceList::const_iterator i = reslit.begin(); i != reslit.end(); ++i)
		{
			Assign(*i);
		}
		return true;
	}
}
