#pragma once

namespace LTESim
{
	//���ɿ���˥����
	class WiFiFading
	{
	public:
		WiFiFading(void){};
	public:
		virtual ~WiFiFading(void){};
		///����Ϊʵֵ����ʾ˥��ķ��ȣ�Խ���ʾ�ŵ�Խ�
		virtual double GetFading( int nbid, int ueid, int TxAntenna, int RxAntenna, double distance) const = 0;
		virtual void Update() = 0; 
	};
}

