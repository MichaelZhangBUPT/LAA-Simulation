#pragma once

namespace LTESim
{
	//���ɿ���˥����
	class LAAFading
	{
	public:
		LAAFading(void){};
	public:
		virtual ~LAAFading(void){};
		///����Ϊʵֵ����ʾ˥��ķ��ȣ�Խ���ʾ�ŵ�Խ�
		virtual double GetFading( int nbid, int ueid, int TxAntenna, int RxAntenna, double distance) const = 0;
		virtual void Update() = 0; 
	};
}

