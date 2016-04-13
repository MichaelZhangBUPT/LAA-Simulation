#pragma once
#include "POS.h"
#include "System.h"

namespace LTESim
{
	class System;

	//��Ԫ�࣬����ע�ᵽSystem�����潫�̳г�UE��NodeB
	class SystemElement
	{
	public:
		SystemElement(int id, const POS& pos);
	public:
		virtual ~SystemElement(void);
		/*����UEID��0��ʼ*/
		int GetID() const;

		/*��������*/
		const POS& GetPos() const;

		//Attach��һ��system
		shared_ptr<System> Attach( shared_ptr<System> pNewSys );

		//ȡ��Attach��system
		shared_ptr<System> GetSystem() const
		{
			return m_pSystem.lock();
		};
	protected:
		weak_ptr<System> m_pSystem;
		int m_ID;//��Ԫ��ʶ
		POS m_pos;//��Ԫλ��
	};
	//�����Ԫ��ʶ
	inline int SystemElement::GetID() const
	{
		return m_ID;
	}
	//�����Ԫλ��
	inline const POS& SystemElement::GetPos() const
	{
		return m_pos;
	}

}
