/******************************************************************************
** �ļ�����		CLinkPrediction.h

** ��Ȩ��		Copyright (c) 2005, BUPT-FTRD Wireless Research Center.  					All Rights Reserved.

** ���ߣ�		WangZiZhou

** ���ڣ�		2005-9-3

** ������		��������ӳ��ģ��ͷ�ļ�

** �汾��		1.0

**------------------------------------------------------------------------------------

��ʷ�޸ļ�¼��
<����>		   <ʱ��>			<�汾>	   	  <����>
WangZiZhou	  2005-9-3           1.0      	�������ļ�


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
	CLinkPrediction();                //���캯��
	~CLinkPrediction();                //��������
	double getVoiceFER(char cUporDownLink,int iChannelType,double fMeanEb2N0);      //��ѯ��������ӳ���������������FER
    //add by CJ
	double getPacket144FER(char cUporDownLink,int iChannelType,double fMeanEb2N0);
    double getPacket64FER(char cUporDownLink,int iChannelType,double fMeanEb2N0);
public:
	double GetHSDPABLER(double fMeanEb2N0, int mcs);
	double GetHSUPABLER(double fMeanEb2N0, int mcs);

private:
	TABLE_TYPE VoiceTable[TABLESIZE];  //�ṹ���飬���ڴ洢��������ӳ����е�����
	//����ҵ���FER��
	//add by CJ
	TABLE_TYPE Packet144Table[130];
    TABLE_TYPE Packet64Table[138];
	//add by ZC
	typedef struct HSUPAarrayTABLE		//����HSUPA��·Ԥ���Ľṹ��
	{
		double  Eb2N0;
		double  FER;
		bool operator <(const double EBNO)
		{
			return this->Eb2N0 < EBNO;
		}
	}HSUPATABLE_TYPE;
	typedef struct HSDPAarrayTABLE		//����HSUPA��·Ԥ���Ľṹ��
	{
		double  Eb2N0;
		double  FER;
		bool operator <(const double EBNO)
		{
			return this->Eb2N0 < EBNO;
		}
	}HSDPATABLE_TYPE;
public:
	void TableInit();                  //��ʼ����������ӳ���
	void TableInitOfHSDPA();            //��ʼ��HSDPA��������ӳ���
	void TableInitOfHSUPA();			//��ʼ��HSUPA����������//add by ZC
	void TableMCStoC2IInitOfHSUPA();	//��ʼ��HSUPAÿ��MCS�ȼ��ڹ涨FER=0.1�¶�Ӧ��C/I//add by ZC
	void TableMCStoC2IInitOfHSDPA();	//��ʼ��HSDPAÿ��MCS�ȼ��ڹ涨FER=0.1�¶�Ӧ��C/I
	void LoadHSDPAFERTable(const char* name, int mcs);
	void LoadHSUPAFERTable(const char* name, int mcs);//add by ZC
	const CQI& GetRecommendTFRI( int channeltype, double C2I ) const;//ע��C2I�Ƿ�DB
	double GetTarC2ItoMCS(int mcs)const;		//����MCS�ȼ��õ���Ӧ��Ŀ��C/Iֵ//add by ZC
	double GetTarC2ItoMCSDL(int mcs)const;		//����MCS�ȼ��õ���Ӧ��Ŀ��C/Iֵ DL
	int GetMCStoC2I(double c2i);		//����C/Iֵ������֧�ֵ�����MCS��0-11��//add by ZC
	int GetMCStoC2IDL( double c2i );
	//const vector<HSDPATABLE_TYPE>&GetHSDPABLERTable( const CQI& cqi ) const;

protected:
	vector <HSDPATABLE_TYPE> HSDPAFERTABLE[HSDPAMCS_SIZE];
	vector <HSUPATABLE_TYPE> HSUPAFERTABLE[HSUPAMCS_SIZE];//ADD BY ZC
protected:
	void InitRecommendTFRI();//�����Ƽ�TFRI��
	void InitThreshHold( int channeltype, CQI cqi );
	const TABLE_TYPE* FindThreshHold( int channeltype, const vector<TABLE_TYPE>& blertable ) const;
	enum{ channeltypeStartAt = 4, channeltypeEndAt = 6};
	std::map<double, CQI> m_tfritable[channeltypeEndAt - channeltypeStartAt + 1];//��FER�µ�CQIֵ
	std::map<int, double> m_MCStoC2I;			//����MCS�ȼ���Ӧ��FER=0.1ʱ��Ŀ��C/I
												//ע�⣺MCS�ȼ��ı����0-11��
	std::map<int, double> m_MCStoC2IDL;			//����MCS�ȼ���Ӧ��FER=0.1ʱ��Ŀ��C/I
	//ע�⣺MCS�ȼ��ı����0-11��
};

#endif