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
    ///构造函数
    /*
    * @param antennaGain, 物理天线增益
    */
    SmartAntenna(double antennaGain);
public:
    ///析构函数
    ~SmartAntenna(void);
public:
    ///初始化智能天线方向图
    /*
    * 为查表运算需要知道NodeB和UE的数目
    * @param SAGainArray，智能天线方向图，实值
    * @param NBNum, NodeB数目
    * @param UENum，UE数目
    */
    static void InitSAGainArray( const std::vector<double>& SAGainArray, int NBNum, int UENum );

    ///基类接口
    /*
    * 智能天线天线端口恒为1，此函数被调用会assert(false)
    */
    void SetAntennaPortNum(int antennaPortNum);

    ///得到天线端口
    /*
    * @return 始终返回1
    */
    int GetAntennaPortNum() const;

    ///得到天线增益
    /*
    * @return 物理天线增益+ZERO_ANGLE_OFFSET方向智能天线增益（可以调整来实现非理想情况）
    */
    double GetAntennaGain() const;

    ///天线分集合并
    /*
    * 智能天线不考虑天线分集，即K=1, SINRPerRB = SINRPerRBPerTxAntenna[0]
    */
    void CombineSINR(const std::vector< std::vector<double> >& SINRPerRBPerTxAntenna, 
                     std::vector<double>& SINRPerRB) const;

    ///求智能天线增益
    /*
    * 得到智能天线对于指定参考NodeB，目的UE，求相邻UE的智能天线增益
    * @param CenterNodeB, 参考NodeB，即发送信号的NodeB
    * @param Target, 目的UE，即智能天线对准的UE
    * @param Neighbor, 相邻UE，即要求的天线增益方向
    * @return 智能天线增益
    */
    double GetSAGain(const NodeB& CenterNodeB, const ILTEUE& Target, const ILTEUE& Neighbor) const;

protected:
    ///求智能天线增益
    /*
     * 求某一个角度上的智能天线增益
     * @param angle, -180~180, 要求的角度方向，中心角是0
     * @return 角度对应的智能天线增益
     */
    double GetSAGain(int angle ) const;

    ///更新保存的天线增益计算结果
    /*
    * 为查表计算，当环境更新时要将表项标记为失效
    */
    static void OnPosUpdated();

protected:
    double m_antennaGain;/*天线物理增益*/
    static std::vector<double> ms_SAGainArray;/*智能天线处理增益*/
    enum {	ZERO_ANGLE_OFFSET = 180/*0度角在ms_SAGainArray中的偏移*/,
				MAXSAGAIN = 100000 /*无效的SA增益，增益表项被大于此值时表示需要计算，否则可以直接使用*/};
    
    /*UE数目,初始表用*/
    static size_t ms_UENum;

    /*NodeB数目,初始表用*/
    static size_t ms_NobeBNum;

    /**以某一个NodeB为基准，相对于某一个UE，其他UE的增益*/
    typedef std::vector<double> GainsForOneUE;

    /**三维表，第一维为发射信号的NodeB，第二维为目标UE，第三维为要求增益的UE*/
    typedef std::vector<GainsForOneUE> GainsForOneNodeB;

    /*已计算出来的SAGAIN保存下来，以后调用时直接返回不再重复计算，
    *注意当UE位置改变时要OnPosUpdated重置*/
    static std::vector< GainsForOneNodeB > ms_CalculatedGains;    
    
    
};

};
