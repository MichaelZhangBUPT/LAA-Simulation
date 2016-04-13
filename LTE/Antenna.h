#pragma once
#include <vector>
namespace LTESim
{
class Antenna
{
public:
    ///���캯��
    Antenna(void);
public:
    ///��������
    virtual ~Antenna(void);

    ///������߶˿���
    /*
    * ���߶˿ڲ������������������ǿ�����RB����Դ������
    * @return ���߶˿���
    */
    virtual int GetAntennaPortNum() const;	//������߸���

    ///�������߶˿���
    /*
    * @param antennaPortNum, �����ߵ����߶˿���
    */
    virtual void SetAntennaPortNum(int antennaPortNum);	//�������߸���
    
    ///����SINR�ּ��ϲ�
    /*
    * �����߶˿ڲο����ϵ�SINRת��ΪRB�ϵ�SINR
    * @param SINRPerRBPerTxAntenna, ����İ����ּ���SINR��ά���飬
    * �������߷ּ�����K����ô������ڶ�άΪK, ��ÿ��K��SINR��Ϊһ���ּ��Ӽ����ᱻ�ϲ���һ��Ԫ��
    * @param SINRPerRB, �ּ��ϲ����SINR������ΪSINRPerRBPerTxAntenna�ĵ�һά
    * @comment ���ߵķּ�����K������������ϲ�����Ҳ������ʵ��
    */
    virtual void CombineSINR(	const std::vector< std::vector<double> >& SINRPerRBPerTxAntenna, 
												std::vector<double>& SINRPerRB	) const = 0;

    ///�õ���������
    virtual double GetAntennaGain() const = 0;
protected:
	int m_iAntennaPortNum;//���߶˿���
};

}
