#pragma once
#include "POS.h"
#include "System.h"

namespace LTESim
{
	class System;

	//网元类，负责注册到System，下面将继承出UE和NodeB
	class SystemElement
	{
	public:
		SystemElement(int id, const POS& pos);
	public:
		virtual ~SystemElement(void);
		/*返回UEID，0开始*/
		int GetID() const;

		/*返回坐标*/
		const POS& GetPos() const;

		//Attach到一个system
		shared_ptr<System> Attach( shared_ptr<System> pNewSys );

		//取得Attach的system
		shared_ptr<System> GetSystem() const
		{
			return m_pSystem.lock();
		};
	protected:
		weak_ptr<System> m_pSystem;
		int m_ID;//网元标识
		POS m_pos;//网元位置
	};
	//获得网元标识
	inline int SystemElement::GetID() const
	{
		return m_ID;
	}
	//获得网元位置
	inline const POS& SystemElement::GetPos() const
	{
		return m_pos;
	}

}
