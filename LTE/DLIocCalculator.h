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
        
        //��������С���ĸ���
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
        virtual double CalcOtherNodeBsInterf( const NodeB &nodeb, const ILTEUE &ue, const PhyResource &RB,int RevAntennaIndex);//��������С���ĸ���
        static shared_ptr<IocCalculator> GetIocCalculator();
    };
}
