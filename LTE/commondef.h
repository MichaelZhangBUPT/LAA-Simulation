#pragma once
#pragma region ��׼��
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

#pragma region boost��
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
using boost::shared_ptr;
using boost::weak_ptr;
using boost::dynamic_pointer_cast;
#pragma endregion boost��

#pragma region �û��Զ���ռ估����
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


//�������ͣ�֮���Ӵ����ݻ�������չ����
typedef double POWER;

//��Щʱ����Ҫʹ��shared_ptrָ��һ������Ҫ�ͷŵ�ָ�룬
//��ʱ��null_deleter��Ϊ�ڶ����������ݸ�shared_ptr
//��shared_ptr����ʱ����delete�ڲ�ָ��
struct null_deleter
{
    void operator()(void const *) const
    {
    }
};

#pragma endregion

#pragma region һЩ���õĸ�������
//δ��ɵĺ����׳����쳣
class UnFinishedFunc : public std::exception
{

};

//const size_t RBNUM = 50;
const double FREQUENCY = 2000.0;		//ϵͳƵ��
const double TXANTENNAHEIHGT = 20;  //����������Ч�߶�
const double REANTENNAHEIGHT = 1;	//����������Ч�߶�

const int SAMPLEDIST = 100;		//��˥��������
const double  m_Velocity = 120/3.6;	//����
const int FRAMELENGTH = 10;	//֡����
const int ShadowUpdatePeriod = static_cast<int>( SAMPLEDIST/m_Velocity )*1000 / FRAMELENGTH;	//��˥��������
//ͳһ����Ϣ����ӿ�
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

// UE״̬
enum UE_STATUS
{
	SERV,										// ��������״̬
	ACCESS_REQUEST_INI,		// ��ʼ��������
	ACCESS_REQUEST,				// ��������
	HANDOVER,							// �л���
	IDLE,										// ����״̬
	BLOCKED,								// Ӳ����
	OUTAGE,								// ������
	OUTAGEDL,							// ����
	OUTAGEUL,							// ����
	FINISH,									// �������
	WAIT
};

enum MCS_Type
{
	//AMC��ʹ�õı�����Ʒ�ʽ��2��
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

// ��Ƶ����
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

// Ŀ��C/I�ṹ��
typedef	struct stC2IThreshold
{
	pair<double, double>	fC2I_dB;		// Ŀ��C/I
	pair<double, double>	fMaxC2I_dB;		// ���Ŀ��C/I
	pair<double, double>	fMinC2I_dB;		// ��СĿ��C/I
}C2IThreshold;

inline int MapSF2RU(SF sf)
{
	return 16 / sf;
}

inline double C2IToEb2No(double c2i, int sf, double CodeGainAdjustment =0.33/*UPA�в��û�����������ΪTurbo1/3= 1.0*//*��������3/4, 1/3, 1*/,  int K = 2/*���Ƶȼ�����,QPSK��2,16QAM��4*/)
{
	// ���㴦������
	const double	BAND_WIDTH	= 5e6;							// ϵͳ����
	const double	CHIP_RATE				=	3.84e6;							// ��Ƭ����
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

