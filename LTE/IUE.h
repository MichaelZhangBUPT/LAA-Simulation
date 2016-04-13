#pragma once

#include "commondef.h"

namespace LTESim
{
	class POS;
	class System;
	class Antenna;
	class Service;
	class HARQ;

	class IUE
	{
	public:
		typedef shared_ptr<IUE> PTR;
	public:
		IUE(){};
		virtual ~IUE(){};
	public:
		//Interface SystemElement
		virtual const POS& GetPos() const = 0;
		virtual int GetID() const = 0;
		virtual shared_ptr<System> Attach( shared_ptr<System> pNewSys ) = 0;

		virtual void Initialization() = 0;

		//获取噪声系数
		virtual double GetNoiseFigure() const = 0;

		/*返回UE的天线*/
		virtual shared_ptr<Antenna> GetRxAntenna() const = 0;

		//业务相关
		//加入一个Service
		virtual bool AddService(shared_ptr<Service> pService) = 0;
		//返回业务
		virtual shared_ptr<Service> GetService( SystemType systemType ) = 0;

		//HARQ
		virtual shared_ptr<HARQ> GetCurrentDLHARQ( SystemType systemType ) = 0;
		virtual shared_ptr<HARQ> GetNextDLHARQ( SystemType systemType ) = 0;
		virtual void UpdateDLHARQProcess( SystemType systemType ) = 0;

		//UEType
		virtual SystemType GetType() const = 0;
	};
}
