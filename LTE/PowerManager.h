#pragma once
namespace LTESim
{
//管理功率，可以有一个总的功率管理器，包含多个小的功率管理器，
//每个小的功率管理器负责一个时间片（调度单元）内的功率分配
class PowerManager
{
public:
    PowerManager(void);
public:
    virtual ~PowerManager(void);
    //测试是否可以获得指定的功率，并不分配
    virtual bool Require( POWER RequiredPower ) const = 0;
    //分配指定的功率
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
