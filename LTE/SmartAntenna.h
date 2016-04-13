#pragma once
#include "Antenna.h"
namespace LTESim
{
class NodeB;
class ILTEUE;
class SmartAntenna :
    public Antenna
{
public:
    ///���캯��
    /*
    * @param antennaGain, ������������
    */
    SmartAntenna(double antennaGain);
public:
    ///��������
    ~SmartAntenna(void);
public:
    ///��ʼ���������߷���ͼ
    /*
    * Ϊ���������Ҫ֪��NodeB��UE����Ŀ
    * @param SAGainArray���������߷���ͼ��ʵֵ
    * @param NBNum, NodeB��Ŀ
    * @param UENum��UE��Ŀ
    */
    static void InitSAGainArray( const std::vector<double>& SAGainArray, int NBNum, int UENum );

    ///����ӿ�
    /*
    * �����������߶˿ں�Ϊ1���˺��������û�assert(false)
    */
    void SetAntennaPortNum(int antennaPortNum);

    ///�õ����߶˿�
    /*
    * @return ʼ�շ���1
    */
    int GetAntennaPortNum() const;

    ///�õ���������
    /*
    * @return ������������+ZERO_ANGLE_OFFSET���������������棨���Ե�����ʵ�ַ����������
    */
    double GetAntennaGain() const;

    ///���߷ּ��ϲ�
    /*
    * �������߲��������߷ּ�����K=1, SINRPerRB = SINRPerRBPerTxAntenna[0]
    */
    void CombineSINR(const std::vector< std::vector<double> >& SINRPerRBPerTxAntenna, 
                     std::vector<double>& SINRPerRB) const;

    ///��������������
    /*
    * �õ��������߶���ָ���ο�NodeB��Ŀ��UE��������UE��������������
    * @param CenterNodeB, �ο�NodeB���������źŵ�NodeB
    * @param Target, Ŀ��UE�����������߶�׼��UE
    * @param Neighbor, ����UE����Ҫ����������淽��
    * @return ������������
    */
    double GetSAGain(const NodeB& CenterNodeB, const ILTEUE& Target, const ILTEUE& Neighbor) const;

protected:
    ///��������������
    /*
     * ��ĳһ���Ƕ��ϵ�������������
     * @param angle, -180~180, Ҫ��ĽǶȷ������Ľ���0
     * @return �Ƕȶ�Ӧ��������������
     */
    double GetSAGain(int angle ) const;

    ///���±�����������������
    /*
    * Ϊ�����㣬����������ʱҪ��������ΪʧЧ
    */
    static void OnPosUpdated();

protected:
    double m_antennaGain;/*������������*/
    static std::vector<double> ms_SAGainArray;/*�������ߴ�������*/
    enum {	ZERO_ANGLE_OFFSET = 180/*0�Ƚ���ms_SAGainArray�е�ƫ��*/,
				MAXSAGAIN = 100000 /*��Ч��SA���棬���������ڴ�ֵʱ��ʾ��Ҫ���㣬�������ֱ��ʹ��*/};
    
    /*UE��Ŀ,��ʼ����*/
    static size_t ms_UENum;

    /*NodeB��Ŀ,��ʼ����*/
    static size_t ms_NobeBNum;

    /**��ĳһ��NodeBΪ��׼�������ĳһ��UE������UE������*/
    typedef std::vector<double> GainsForOneUE;

    /**��ά����һάΪ�����źŵ�NodeB���ڶ�άΪĿ��UE������άΪҪ�������UE*/
    typedef std::vector<GainsForOneUE> GainsForOneNodeB;

    /*�Ѽ��������SAGAIN�����������Ժ����ʱֱ�ӷ��ز����ظ����㣬
    *ע�⵱UEλ�øı�ʱҪOnPosUpdated����*/
    static std::vector< GainsForOneNodeB > ms_CalculatedGains;    
    
    
};

};
