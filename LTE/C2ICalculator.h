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
		//������Ὣ�ü���ĸ�Channel��C2I��Ϣ�浽��Channel��
		void CalculateC2IOfChannel( shared_ptr<HSPAChannel> pChannel );
		//�Ӹ�Channel�洢��ÿ��Slot��C2I��Ϣ�õ���Ч��C2I
		double CalculateEffectiveC2I( shared_ptr<HSPAChannel> pChannel );
		 Matrix MatrixGenerator(shared_ptr<HSPAChannel> pChannel);
		//SCM�µĴ�߶Ⱥ��г߶�
		double SCMPropLossDBDL(shared_ptr<HSPANodeB> nodeb, shared_ptr<IHSPAUE> ue);
	protected:
		C2ICalculator();
	protected:
		static C2ICalculator * m_pInstance;
		//C2I�������
		//���������źŹ���
	     double GetSelfPower(shared_ptr<HSPAChannel> pChannel, int stream);
		//����С���ڸ���
		 double CalcIor(shared_ptr<HSPAChannel> pChannel, int stream);
		//����С�������
		 double CalcIoc(shared_ptr<HSPAChannel> pChannel, int stream);
		//����C2I
		 double CalcProlossFastFading(shared_ptr<HSPAChannel> pChannel);


		//���ڳ�ʼ��ue��Ȩֵ
		
		 Matrix HMatrixGen(shared_ptr<HSPAChannel> pChannel);
		 void MatrixCalc(shared_ptr<HSPAChannel> pChannel);
	private:
		//���ڼ�����м����
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