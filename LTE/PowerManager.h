#pragma once
namespace LTESim
{
//�����ʣ�������һ���ܵĹ��ʹ��������������С�Ĺ��ʹ�������
//ÿ��С�Ĺ��ʹ���������һ��ʱ��Ƭ�����ȵ�Ԫ���ڵĹ��ʷ���
class PowerManager
{
public:
    PowerManager(void);
public:
    virtual ~PowerManager(void);
    //�����Ƿ���Ի��ָ���Ĺ��ʣ���������
    virtual bool Require( POWER RequiredPower ) const = 0;
    //����ָ���Ĺ���
    virtual bool Assign( POWER RequiredPower ) = 0;
};

class SimplePowerManager : public PowerManager
{
public:
	bool Require(POWER RequiredPower ) const
	{
		return true;
	}

	bool Assign(POWER RequiredPower )
	{
		return true;
	}
};
}
