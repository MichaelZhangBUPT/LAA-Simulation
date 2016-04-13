#pragma once
#include <complex>
#include <math.h>
namespace LTESim
{
    //�������꣬������WrapAround
    class POS : public std::complex<double>
    {
    public:
        ///���캯��
        /*
        * ��ʼ��Ϊ(0,0)
        */
        POS(void);

        ///���캯��
        /*
        * ��ʼ��Ϊ(x,y)
        */
        POS(double x, double y);
    public:
        ///��������
        ~POS(void);

        ///���x����
        double GetX() const
        {
            return real();	
        }

        ///���y����
        double GetY() const	
        {
            return imag();
        }
    };

    //�õ�����֮��ľ��룬û�п���WrapAround��������Ҫ��һ���޸ģ�����Function Object��
    inline double Distance(const POS& p1, const POS& p2)
    {
        return abs(p1 - p2);	
    }


    //����pDest�����pOrig�ķ���
    /*
    * @return �Ƕ�
    */
    inline double Angle(const POS& pOrig, const POS& pDest)
    {
        return arg(pDest - pOrig) * 180 / M_PI;
    }

}
