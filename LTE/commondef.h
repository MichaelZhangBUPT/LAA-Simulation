#pragma once
#pragma region 标准库
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <complex>
//#include <algorithm>
//#include <functional>
//#include <numeric>
using std::vector;
using std::map;
using std::set;
using std::pair;
using std::cout;
using std::endl;
using std::string;
using std::complex;
#pragma endregion

#pragma region boost库
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
using boost::shared_ptr;
using boost::weak_ptr;
using boost::dynamic_pointer_cast;
#pragma endregion boost库

#pragma region 用户自定义空间及常量
namespace LTESim {};
using namespace LTESim;
enum CLOCKID {
	TESTCLOCK, 
	SYSCLOCK, 
	FRAMECLOCK, 
	LTESUBFRAMECLOCK, 
	HSPASUBFRAMECLOCK,
	LTESLOTCLOCK, 
	HSPASLOTCLOCK, 
	TXCLOCK, 
	RXCLOCK, 
	HIGHLAYERCLOCK,
	STANDARDCLOCK,
	WIFICLOCK,
	LAACLOCK
};

enum CHANNELCODE_RATE{
	RATE_1_5, 
	RATE_1_3,
	RATE_2_5, 
	RATE_1_2,  
	RATE_3_5, 
	RATE_2_3, 
	RATE_3_4, 
	RATE_4_5,
	RATE_5_6
};

enum MODULATION{
	BPSK = 0,
	QPSK = 2, 
	QAM16 = 4, 
	QAM64= 6
};

enum LinkDirection
{
	NULLLINK = 0,
	UPLINK = 1,
	DOWNLINK = 2
};

enum SystemType
{
	SYSTEM_NULL = 0,
	SYSTEM_LTE = 1,
	SYSTEM_HSPA = 2,
	SYSTEM_DUAL = 3,
	SYSTEM_WIFI = 4,
	SYSTEM_LAA =5
};

enum ChannelState
{
	Idle = 0,
	Backoff = 1,//HFR
	Busy = 2,
	Collision = 3
};


enum ServiceType
{
	FTP = 0,
	VoIP = 1,
	FullBuffer = 2,
	Auxiliary = 3,
	DualModeFTP = 4,
	Stream = 5
};

//enum WiFiTransMode
//{
//	A, B, C, D, E, F
//};

const int STANDARDTIMESLICEPERFRAME = 60;
const int STANDARDTIMESLICEPERHSPASUBFRAME = 12;
const int LTESUBFRAMEPERFRAME = 10;
const int HSPASUBFRAMEPERFRAME = 5;

const int LTESLOTPERFRAME = 20;
const int HSPASLOTPERFRAME = 15;

const int LTESLOTPERSUBFRAME = 2;
const int HSPASLOTPERSUBFRAME = 3;

const int FRAME_LENGTH_MS = 10;
const int LTE_SUBFRAME_LENGTH_MS = 1;
const int HSPA_SUBFRAME_LENGTH_MS = 2;

const int WIFI_STANDARD_US = 8;
const int WIFI_DIFS_US = 34;
const int WIFI_DEFER_US = 34;//HFR
const int WIFI_SIFS_US = 16;
const int WIFI_SLOT_US = 8;
const int WIFI_RTS_US = 0;
const int WIFI_CTS_US = 0;
const double WIFI_ACK_US = 14.0*8.0/65.0 + 20.0;
const int WIFI_ACK_TIMEOUT_US = 50;

const int WIFISTANDARDPERLTESLOT = 125;

const int LAA_ICCA_US = 34;
const int LAA_ECCA_US = 34;
const int LAA_ECCA_SLOT_US = 8;
const int LAA_TX_ConfigTime = 71;


//功率类型，之后视代码演化可能扩展成类
typedef double POWER;

//有些时候需要使用shared_ptr指向一个不需要释放的指针，
//这时候将null_deleter作为第二个参数传递给shared_ptr
//在shared_ptr结束时将不delete内部指针
struct null_deleter
{
    void operator()(void const *) const
    {
    }
};

#pragma endregion

#pragma region 一些常用的辅助函数
//未完成的函数抛出此异常
class UnFinishedFunc : public std::exception
{

};

//const size_t RBNUM = 50;
const double FREQUENCY = 2000.0;		//系统频率
const double TXANTENNAHEIHGT = 20;  //发送天线有效高度
const double REANTENNAHEIGHT = 1;	//接受天线有效高度

const int SAMPLEDIST = 100;		//慢衰采样距离
const double  m_Velocity = 120/3.6;	//速率
const int FRAMELENGTH = 10;	//帧长度
const int ShadowUpdatePeriod = static_cast<int>( SAMPLEDIST/m_Velocity )*1000 / FRAMELENGTH;	//慢衰更新周期
//统一的信息输出接口
//#define USER_MESSAGE(S) std::cerr << endl << S << endl;
#define USER_MESSAGE(S)
#define USER_LOG(S, FILE) FILE << S << endl;
#define MAKE_SURE( VERDICT, CALL_FUNC ) if( !(VERDICT) ) {USER_MESSAGE( CALL_FUNC ); throw std::logic_error( CALL_FUNC );}


#pragma endregion

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#pragma region HSPA Utility

typedef double Real;

// UE状态
enum UE_STATUS
{
	SERV,										// 正常服务状态
	ACCESS_REQUEST_INI,		// 初始接入请求
	ACCESS_REQUEST,				// 接入请求
	HANDOVER,							// 切换中
	IDLE,										// 空闲状态
	BLOCKED,								// 硬阻塞
	OUTAGE,								// 软阻塞
	OUTAGEDL,							// 下行
	OUTAGEUL,							// 上行
	FINISH,									// 服务完成
	WAIT
};

enum MCS_Type
{
	//AMC所使用的编码调制方式有2种
	//    MCS1:   16QAM + 1/3 Turbo           MCS2: QPSK + 1/3 Turbo
	MCS1 = 0,
	MCS2,
	MCS3,
	MCS4,
	MCS5,
	MCS6,
	MCS7,
	MCS8,
	MCS9,
	MCS10,
	MCS11,
	MCSUNDEFINED,
};

typedef struct tagCQI
{
	MCS_Type mcs;
	bool IsValid() const
	{
		return mcs != MCSUNDEFINED; 
	}
	bool operator == (const tagCQI& cqi)
	{
		return mcs == cqi.mcs;
	}
}CQI;

// 扩频因子
enum SF
{
	SF_NONE =   -1,
	SF_1	=	1,
	SF_2	=	2,
	SF_4	=	4,
	SF_8	=	8,
	SF_16	=	16
};

enum TPC
{
	UP,
	DOWN,
	INVALID
};

// 目标C/I结构体
typedef	struct stC2IThreshold
{
	pair<double, double>	fC2I_dB;		// 目标C/I
	pair<double, double>	fMaxC2I_dB;		// 最大目标C/I
	pair<double, double>	fMinC2I_dB;		// 最小目标C/I
}C2IThreshold;

inline int MapSF2RU(SF sf)
{
	return 16 / sf;
}

inline double C2IToEb2No(double c2i, int sf, double CodeGainAdjustment =0.33/*UPA中采用基本编码速率为Turbo1/3= 1.0*//*编码增益3/4, 1/3, 1*/,  int K = 2/*调制等级因子,QPSK是2,16QAM是4*/)
{
	// 计算处理增益
	const double	BAND_WIDTH	= 5e6;							// 系统带宽
	const double	CHIP_RATE				=	3.84e6;							// 码片速率
	//double	fGp		=	BAND_WIDTH * sf / (CodeGainAdjustment * K * CHIP_RATE);
	double	fGp		=	BAND_WIDTH * sf / CHIP_RATE;
	return	c2i * fGp; 
}

enum {MCS_SIZE = 14,HSUPAMCS_SIZE=12,HSDPAMCS_SIZE = 11};


const complex<double> HSPAMIMOWeight[4][2]= {
	{0.707,complex<double>(0.5,0.5)},
	{0.707,complex<double>(0.5,-0.5)},
	{0.707,complex<double>(-0.5,0.5)},
	{0.707,complex<double>(-0.5,-0.5)}
};

#pragma endregion

