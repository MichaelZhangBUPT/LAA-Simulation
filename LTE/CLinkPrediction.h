/******************************************************************************
** 文件名：		CLinkPrediction.h

** 版权：		Copyright (c) 2005, BUPT-FTRD Wireless Research Center.  					All Rights Reserved.

** 作者：		WangZiZhou

** 日期：		2005-9-3

** 描述：		服务质量映射模块头文件

** 版本：		1.0

**------------------------------------------------------------------------------------

历史修改记录：
<作者>		   <时间>			<版本>	   	  <描述>
WangZiZhou	  2005-9-3           1.0      	创建该文件


******************************************************************************/
#ifndef CLINKPREDICTION_H
#define CLINKPREDICTION_H

#include "TDSCDMA.h"
#include <iostream>
#include <fstream>
#include <map>

class CLinkPrediction
{
public:
	CLinkPrediction();                //构造函数
	~CLinkPrediction();                //析构函数
	double getVoiceFER(char cUporDownLink,int iChannelType,double fMeanEb2N0);      //查询服务质量映射表函数，返回语音FER
    //add by CJ
	double getPacket144FER(char cUporDownLink,int iChannelType,double fMeanEb2N0);
    double getPacket64FER(char cUporDownLink,int iChannelType,double fMeanEb2N0);
public:
	double GetHSDPABLER(double fMeanEb2N0, int mcs);
	double GetHSUPABLER(double fMeanEb2N0, int mcs);

private:
	TABLE_TYPE VoiceTable[TABLESIZE];  //结构数组，用于存储服务质量映射表中的数据
	//数据业务的FER表
	//add by CJ
	TABLE_TYPE Packet144Table[130];
    TABLE_TYPE Packet64Table[138];
	//add by ZC
	typedef struct HSUPAarrayTABLE		//用于HSUPA链路预算表的结构体
	{
		double  Eb2N0;
		double  FER;
		bool operator <(const double EBNO)
		{
			return this->Eb2N0 < EBNO;
		}
	}HSUPATABLE_TYPE;
	typedef struct HSDPAarrayTABLE		//用于HSUPA链路预算表的结构体
	{
		double  Eb2N0;
		double  FER;
		bool operator <(const double EBNO)
		{
			return this->Eb2N0 < EBNO;
		}
	}HSDPATABLE_TYPE;
public:
	void TableInit();                  //初始化服务质量映射表
	void TableInitOfHSDPA();            //初始化HSDPA服务质量映射表
	void TableInitOfHSUPA();			//初始化HSUPA服务质量表//add by ZC
	void TableMCStoC2IInitOfHSUPA();	//初始化HSUPA每种MCS等级在规定FER=0.1下对应的C/I//add by ZC
	void TableMCStoC2IInitOfHSDPA();	//初始化HSDPA每种MCS等级在规定FER=0.1下对应的C/I
	void LoadHSDPAFERTable(const char* name, int mcs);
	void LoadHSUPAFERTable(const char* name, int mcs);//add by ZC
	const CQI& GetRecommendTFRI( int channeltype, double C2I ) const;//注意C2I是非DB
	double GetTarC2ItoMCS(int mcs)const;		//根据MCS等级得到对应的目标C/I值//add by ZC
	double GetTarC2ItoMCSDL(int mcs)const;		//根据MCS等级得到对应的目标C/I值 DL
	int GetMCStoC2I(double c2i);		//根据C/I值返回能支持的最大的MCS（0-11）//add by ZC
	int GetMCStoC2IDL( double c2i );
	//const vector<HSDPATABLE_TYPE>&GetHSDPABLERTable( const CQI& cqi ) const;

protected:
	vector <HSDPATABLE_TYPE> HSDPAFERTABLE[HSDPAMCS_SIZE];
	vector <HSUPATABLE_TYPE> HSUPAFERTABLE[HSUPAMCS_SIZE];//ADD BY ZC
protected:
	void InitRecommendTFRI();//建立推荐TFRI表
	void InitThreshHold( int channeltype, CQI cqi );
	const TABLE_TYPE* FindThreshHold( int channeltype, const vector<TABLE_TYPE>& blertable ) const;
	enum{ channeltypeStartAt = 4, channeltypeEndAt = 6};
	std::map<double, CQI> m_tfritable[channeltypeEndAt - channeltypeStartAt + 1];//各FER下的CQI值
	std::map<int, double> m_MCStoC2I;			//各个MCS等级对应的FER=0.1时的目标C/I
												//注意：MCS等级的标号是0-11。
	std::map<int, double> m_MCStoC2IDL;			//各个MCS等级对应的FER=0.1时的目标C/I
	//注意：MCS等级的标号是0-11。
};

#endif