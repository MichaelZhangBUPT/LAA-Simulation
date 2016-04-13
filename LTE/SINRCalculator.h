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
		
		//�����Channel����ÿ��RB��SINR��������Channel
		void   CalculateSINRofChannel(const shared_ptr<Channel> pRxchannel, int iframe, int islot) const;
		
		//����ÿ��RB�ϵ�SINR,��ͬ���������ϵ�RB������Ϊ�����ڲ�ͬ��RB
		double CalcSINRofOneRB( const NodeB & nodeb, const ILTEUE & ue, const PhyResource &RB) const;

		double EESMCalculator(vector<double>& vSINR, const Channel &channel) const; 
#pragma region ����SINR���Ӻ���
	protected:	

	
		//����ÿ������������ÿ��RB��SINR
        double CalcSINRofOneRBofOneRecvAntenna(const NodeB & nodeb, const ILTEUE & ue, const PhyResource &RB, int RevAntennaIndex) const;

		//����RB��ĳ�����������ϵĽ��ܹ���
		double CalcSelfRBPower(const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB, int RevAntennaIndex) const;    

		//����ĳ������������ĳ��RB���ܵ�С������Ź���
		double CalcIoc(const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB, int RevAntennaIndex) const;

		//����ĳ������������ĳ��RB���ܵ�С���ڸ��Ź��ʣ�����û���õ���ֱ�ӷ��ص�0
		double CalcIor() const;

		
		//����UE������
		double CalcUENoisePower(const NodeB& nodeb, const ILTEUE &ue, const PhyResource &RB) const;

		////����EESM�ϲ�֮�������SINR���ڵ��ô˺���֮ǰ���뱣֤channel�ж�Ӧʱ϶��ÿ����������ÿ��RB�ϵ�SINR�������������
		//double CalcSINRAfterEESM(const Channel & channel, int frame, int slot) const;

		double GetBeta(const MCS &mcs) const;
#pragma endregion ����SINR���Ӻ���

	protected:
		shared_ptr <IocCalculator> m_pIocCalc; //С����
		shared_ptr <IorCalculator> m_pIorCalc; //С����
        static SINRCalculator ms_dlCalculator;
	};
}

