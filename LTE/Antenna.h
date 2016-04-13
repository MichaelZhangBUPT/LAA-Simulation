#pragma once
#include <vector>
namespace LTESim
{
class Antenna
{
public:
    ///构造函数
    Antenna(void);
public:
    ///析构函数
    virtual ~Antenna(void);

    ///获得天线端口数
    /*
    * 天线端口并非物理天线数，而是可配置RB的资源网格数
    * @return 天线端口数
    */
    virtual int GetAntennaPortNum() const;	//获得天线个数

    ///设置天线端口数
    /*
    * @param antennaPortNum, 该天线的天线端口数
    */
    virtual void SetAntennaPortNum(int antennaPortNum);	//设置天线个数
    
    ///天线SINR分集合并
    /*
    * 将天线端口参考点上的SINR转换为RB上的SINR
    * @param SINRPerRBPerTxAntenna, 输入的包含分集的SINR二维数组，
    * 假设天线分集阶数K，那么该数组第二维为K, 即每行K个SINR视为一个分集子集，会被合并成一个元素
    * @param SINRPerRB, 分集合并后的SINR，长度为SINRPerRBPerTxAntenna的第一维
    * @comment 天线的分集阶数K由子类决定，合并方法也由子类实现
    */
    virtual void CombineSINR(	const std::vector< std::vector<double> >& SINRPerRBPerTxAntenna, 
												std::vector<double>& SINRPerRB	) const = 0;

    ///得到天线增益
    virtual double GetAntennaGain() const = 0;
protected:
	int m_iAntennaPortNum;//天线端口数
};

}
