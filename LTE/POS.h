#pragma once
#include <complex>
#include <math.h>
namespace LTESim
{
    //物理坐标，不考虑WrapAround
    class POS : public std::complex<double>
    {
    public:
        ///构造函数
        /*
        * 初始化为(0,0)
        */
        POS(void);

        ///构造函数
        /*
        * 初始化为(x,y)
        */
        POS(double x, double y);
    public:
        ///析构函数
        ~POS(void);

        ///获得x坐标
        double GetX() const
        {
            return real();	
        }

        ///获得y坐标
        double GetY() const	
        {
            return imag();
        }
    };

    //得到两点之间的距离，没有考虑WrapAround，可能需要进一步修改（做成Function Object）
    inline double Distance(const POS& p1, const POS& p2)
    {
        return abs(p1 - p2);	
    }


    //返回pDest相对于pOrig的方向
    /*
    * @return 角度
    */
    inline double Angle(const POS& pOrig, const POS& pDest)
    {
        return arg(pDest - pOrig) * 180 / M_PI;
    }

}
