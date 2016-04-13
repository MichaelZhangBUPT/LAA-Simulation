#pragma once
#include "UE.h"
#include "NodeB.h"
#include "IorCalculator.h"
#include "Channel.h"
namespace LTESim
{
	class IocCalculator;
	class Channel;
	class SINRCalculator
	{
	public:
		SINRCalculator(void);
		static SINRCalculator & GetDLCalculator();
        static void InitDLCalculator(shared_ptr<IorCalculator> pIorCalc, shared_ptr<IocCalculator> pIocCalc);        
	public:
		~SINRCalculator(void);
		
		//计算该Channel里面每个RB的SINR，并存入Channel
		void   CalculateSINRofChannel(const shared_ptr<Channel> pRxchannel, int iframe, int islot) const;
		
		//计算每个RB上的SINR,不同发射天线上的RB我们认为是属于不同的RB
		double CalcSINRofOneRB( const NodeB & nodeb, const ILTEUE & ue, const PhyResource &RB) const;

		double EESMCalculator(vector<double>& vSINR, const Channel &channel) const; 
#pragma region 计算SINR的子函数
	protected:	

	
		//计算每根接受天线上每个RB的SINR
        double CalcSINRofOneRBofOneRecvAntenna(const NodeB & nodeb, const ILTEUE & ue, const PhyResource &RB, int RevAntennaIndex) const;

		//计算RB在某个接受天线上的接受功率
		double CalcSelfRBPower(const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB, int RevAntennaIndex) const;    

		//计算某根接收天线上某个RB承受的小区间干扰功率
		double CalcIoc(const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB, int RevAntennaIndex) const;

		//计算某根接收天线上某个RB承受的小区内干扰功率，现在没有用到，直接返回的0
		double CalcIor() const;

		
		//计算UE白噪声
		double CalcUENoisePower(const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB) const;

		////计算EESM合并之后的有限SINR，在调用此函数之前必须保证channel中对应时隙的每个接收天线每个RB上的SINR都被计算出来了
		//double CalcSINRAfterEESM(const Channel & channel, int frame, int slot) const;

		double GetBeta(const MCS &mcs) const;
#pragma endregion 计算SINR的子函数

	protected:
		shared_ptr <IocCalculator> m_pIocCalc; //小区间
		shared_ptr <IorCalculator> m_pIorCalc; //小区内
        static SINRCalculator ms_dlCalculator;
	};
}

