#pragma once 
#include "HSPANodeB.h"
#include "HSPAUE.h"
#include "Map.h"
#include "Fading.h"
#include "RunParameters.h"
#include "Pathloss.h"

namespace LTESim
{
	class HSPAChannel;

	class C2ICalculator
	{
	public:
		static C2ICalculator * Instance();
	public:
		~C2ICalculator(){};
		//在这里会将该计算的该Channel的C2I信息存到该Channel中
		void CalculateC2IOfChannel( shared_ptr<HSPAChannel> pChannel );
		//从该Channel存储的每个Slot的C2I信息得到等效的C2I
		double CalculateEffectiveC2I( shared_ptr<HSPAChannel> pChannel );
		 Matrix MatrixGenerator(shared_ptr<HSPAChannel> pChannel);
		//SCM下的大尺度和中尺度
		double SCMPropLossDBDL(shared_ptr<HSPANodeB> nodeb, shared_ptr<IHSPAUE> ue);
	protected:
		C2ICalculator();
	protected:
		static C2ICalculator * m_pInstance;
		//C2I计算相关
		//计算有用信号功率
	     double GetSelfPower(shared_ptr<HSPAChannel> pChannel, int stream);
		//计算小区内干扰
		 double CalcIor(shared_ptr<HSPAChannel> pChannel, int stream);
		//计算小区间干扰
		 double CalcIoc(shared_ptr<HSPAChannel> pChannel, int stream);
		//计算C2I
		 double CalcProlossFastFading(shared_ptr<HSPAChannel> pChannel);


		//用于初始化ue的权值
		
		 Matrix HMatrixGen(shared_ptr<HSPAChannel> pChannel);
		 void MatrixCalc(shared_ptr<HSPAChannel> pChannel);
	private:
		//用于计算的中间变量
		Matrix m_GMatrix;
		Matrix m_KMatrix;
		Matrix m_KMatrixother;
		Matrix m_NoiseMatrix;
		double NodeBAntennaGain_dB;
		double UEAntennaGain_dB;
		double CableLoss_dB;
		double UEOtherLoss_dB;
	};
}