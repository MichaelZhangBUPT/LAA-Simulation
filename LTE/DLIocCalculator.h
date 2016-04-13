#pragma once
#include "IocCalculator.h"
#include "UE.h"
#include "NodeB.h"

namespace LTESim
{
	class DLIocCalculator :
		public IocCalculator
	{
	public:
		DLIocCalculator(void);
	public:
		~DLIocCalculator(void);
        
        //计算其他小区的干扰
        virtual double CalcOtherNodeBsInterf( const NodeB &nodeb, 
                                              const ILTEUE &ue, 
                                              const PhyResource &RB,
                                              int RevAntennaIndex);
		
        static shared_ptr<IocCalculator> GetIocCalculator(void);
	};

    class DLSAIocCalculator :
        public IocCalculator
    {
    public:
        DLSAIocCalculator();
        ~DLSAIocCalculator();
        virtual double CalcOtherNodeBsInterf( const NodeB &nodeb, const ILTEUE &ue, const PhyResource &RB,int RevAntennaIndex);//计算其他小区的干扰
        static shared_ptr<IocCalculator> GetIocCalculator();
    };
}
