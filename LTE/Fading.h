#pragma once

namespace LTESim
{
	//���ɿ���˥����
	class Fading
	{
	public:
		Fading(void);
	public:
		virtual ~Fading(void);
        ///����Ϊʵֵ����ʾ˥��ķ��ȣ�Խ���ʾ�ŵ�Խ�
		virtual double GetFading( int nbid, int ueid, int RBIndex, int TxAntenna, int RxAntenna) const = 0;
		virtual void Update() = 0; 
	};

}

