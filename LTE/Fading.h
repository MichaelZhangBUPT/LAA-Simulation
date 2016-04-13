#pragma once

namespace LTESim
{
	//生成快慢衰的类
	class Fading
	{
	public:
		Fading(void);
	public:
		virtual ~Fading(void);
        ///返回为实值，表示衰落的幅度（越大表示信道越差）
		virtual double GetFading( int nbid, int ueid, int RBIndex, int TxAntenna, int RxAntenna) const = 0;
		virtual void Update() = 0; 
	};

}

