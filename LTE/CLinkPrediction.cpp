/******************************************************************************
** 文件名：		CLinkPrediction.cpp

** 版权：   	Copyright (c) 2005, BUPT-FTRD Wireless Research Center. 					All Rights Reserved.

** 作者：		WangZiZhou

** 日期：		2005-9-8

** 描述：		用于详细说明此程序文件完成的主要功能，与其他模块
                
				或函数的接口，输出值、取值范围

** 版本：		1.0

**------------------------------------------------------------------------------------

历史修改记录：
<作者>		   <时间>			<版本>	   	 <描述>
WangZiZhou	  2005-9-8           1.0    	创建该文件
lizhenjie									修改包含的头文件

******************************************************************************/
#include "StdAfx.h"
#include "CLinkPrediction.h"
#include <algorithm>
#include <functional>
#include"CNetWorkDrive.h"
//////////////////////////////////////////////////////////////////////////
//		              构造函数                                          //
//////////////////////////////////////////////////////////////////////////
CLinkPrediction::CLinkPrediction()
{
}

//////////////////////////////////////////////////////////////////////////
//		              析构函数					                        //
//////////////////////////////////////////////////////////////////////////
CLinkPrediction::~CLinkPrediction()
{
}


//////////////////////////////////////////////////////////////////////////
//TITLE:     void TableInit()    将链路级仿真结果存于服务质量映射表中
//
//PARAMETERS：
//////////////////////////////////////////////////////////////////////////
void CLinkPrediction::TableInit( )
{
//	char FILENAME[] = "TDDchannelmodel-voicefer.txt";
	//add by CJ
//	char Packet144FILENAME[] = "TDDchannelmodel_144Data.txt";
//	char Packet64FILENAME[] ="TDDchannelmodel_64Data.txt";
     
	//add by liyintao
	CString voiceferFILENAME("TDDchannelmodel-voicefer.txt");
	CString Packet144FILENAME("TDDchannelmodel_144Data.txt");
    CString Packet64FILENAME("TDDchannelmodel_64Data.txt");

    for (int ii=0;ii<TABLESIZE;ii++)
	{
		VoiceTable[ii].cUPorDOwnLink='x';
		VoiceTable[ii].iCHannelType=0;
        VoiceTable[ii].fMEanEb2N0=0.0;
		VoiceTable[ii].fFER=0.0;
	}

		//add by CJ
    for (int  ii=0;ii<138;ii++)
	{
		Packet64Table[ii].cUPorDOwnLink='x';
		Packet64Table[ii].iCHannelType=0;
        Packet64Table[ii].fMEanEb2N0=0.0;
		Packet64Table[ii].fFER=0.0;
	}	
		//add by CJ
	for (int ii=0;ii<130;ii++)
	{
		Packet144Table[ii].cUPorDOwnLink='x';
		Packet144Table[ii].iCHannelType=0;
        Packet144Table[ii].fMEanEb2N0=0.0;
		Packet144Table[ii].fFER=0.0;
    }


     //modified by liyintao
	//定义一个输入文件对象
	ifstream inVoiceFER;
	if (CNetWorkDrive::ms_path == "local")//初始文件从本地文件读入
	{
		inVoiceFER.open(voiceferFILENAME , ios::in);
	}
	else//初始文件从其它文件读入
	{
		inVoiceFER.open((CNetWorkDrive::ms_path)+voiceferFILENAME , ios::in);
	}
	//ifstream inVoiceFER((CNetWorkDrive::ms_path)+voiceferFILENAME,ios::in);	
	//add by CJ
	ifstream inPacket144FER;
	if (CNetWorkDrive::ms_path == "local")//初始文件从本地文件读入
	{
		inPacket144FER.open(Packet144FILENAME , ios::in);
	}
	else//初始文件从其它文件读入
	{
		inPacket144FER.open((CNetWorkDrive::ms_path)+Packet144FILENAME , ios::in);
	}
	ifstream inPacket64FER;
	if (CNetWorkDrive::ms_path == "local")//初始文件从本地文件读入
	{
		inPacket64FER.open(Packet64FILENAME , ios::in);
	}
	else//初始文件从其它文件读入
	{
		inPacket64FER.open((CNetWorkDrive::ms_path)+Packet64FILENAME , ios::in);
	}
	//ifstream inPacket144FER((CNetWorkDrive::ms_path)+Packet144FILENAME,ios::in);
    //ifstream inPacket64FER((CNetWorkDrive::ms_path)+Packet64FILENAME,ios::in);
	// 打开数据文件准备读
	if(!inVoiceFER)
	{
		std::cerr<<"File "<<(CNetWorkDrive::ms_path)+voiceferFILENAME<<" cound not be opened\n";
		system("pause");exit(1);
	}
	//add by CJ
    if(!inPacket144FER)
	{
		std::cerr<<"File "<<(CNetWorkDrive::ms_path)+Packet144FILENAME<<" cound not be opened\n";
		system("pause");exit(1);
	}
	if(!inPacket64FER)
	{
		std::cerr<<"File "<<(CNetWorkDrive::ms_path)+Packet64FILENAME<<" cound not be opened\n";
		system("pause");exit(1);
	}

	for (int ii=0;ii<TABLESIZE;ii++)
	{
		inVoiceFER>>VoiceTable[ii].cUPorDOwnLink>>VoiceTable[ii].iCHannelType>>VoiceTable[ii].fMEanEb2N0>>VoiceTable[ii].fFER;
		if ((VoiceTable[ii].iCHannelType=='x' || VoiceTable[ii].iCHannelType==0) && VoiceTable[ii].fMEanEb2N0==0.0 && VoiceTable[ii].fFER==0.0)
		{
			break;
		}
	}
	//add by CJ
	for (int ii=0;ii<130;ii++)
	{
		inPacket144FER>>Packet144Table[ii].cUPorDOwnLink>>Packet144Table[ii].iCHannelType>>Packet144Table[ii].fMEanEb2N0>>Packet144Table[ii].fFER;
		if ((Packet144Table[ii].iCHannelType=='x' || Packet144Table[ii].iCHannelType==0) && Packet144Table[ii].fMEanEb2N0==0.0 && Packet144Table[ii].fFER==0.0)
		{
			break;
		}
	}
	for (int ii=0;ii<138;ii++)
	{
		inPacket64FER>>Packet64Table[ii].cUPorDOwnLink>>Packet64Table[ii].iCHannelType>>Packet64Table[ii].fMEanEb2N0>>Packet64Table[ii].fFER;
		if ((Packet64Table[ii].iCHannelType=='x' || Packet64Table[ii].iCHannelType==0) && Packet64Table[ii].fMEanEb2N0==0.0 && Packet64Table[ii].fFER==0.0)
		{
			break;
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//TITLE:     getVoiceFER(char ,int ,double )    取得语音FER值
//                                                             
//
//PARAMETERS：int iChannelType,double fMeanEb2N0
//////////////////////////////////////////////////////////////////////////
double CLinkPrediction::getVoiceFER(char cUporDownLink, int iChannelType,double fMeanEb2N0)
{
	double FERtemp=0.0;
	for(int ii=0 ; ii<TABLESIZE ; ii++)
	{
		if(VoiceTable[ii].cUPorDOwnLink == cUporDownLink && VoiceTable[ii].iCHannelType == iChannelType && VoiceTable[ii].fMEanEb2N0 >= fMeanEb2N0)
		{	 
			FERtemp = VoiceTable[ii].fFER;
			break;
		}
	}
	return FERtemp;
}
//add by CJ
double CLinkPrediction::getPacket144FER(char cUporDownLink,int iChannelType,double fMeanEb2N0)
{
	double FERtemp=0.0;
	for(int ii=0 ; ii<130 ; ii++)
	{
		if(Packet144Table[ii].cUPorDOwnLink == cUporDownLink && Packet144Table[ii].iCHannelType == iChannelType && Packet144Table[ii].fMEanEb2N0 >= fMeanEb2N0)
		{	 
			FERtemp = Packet144Table[ii].fFER;
			break;
		}
	}
	return FERtemp;
}

double CLinkPrediction::getPacket64FER(char cUporDownLink,int iChannelType,double fMeanEb2N0)
{
	double FERtemp=0.0;
	for(int ii=0 ; ii<138 ; ii++)
	{
		if(Packet64Table[ii].cUPorDOwnLink == cUporDownLink && Packet64Table[ii].iCHannelType == iChannelType && Packet64Table[ii].fMEanEb2N0 >= fMeanEb2N0)
		{	 
			FERtemp = Packet64Table[ii].fFER;
			break;
		}
	}
	return FERtemp;
}

//modified by liyintao
void CLinkPrediction::TableInitOfHSDPA()
{
	const char * filename[] = {"MCS1.txt","MCS2.txt","MCS3.txt","MCS4.txt",	"MCS5.txt",
	                     "MCS6.txt","MCS7.txt",	"MCS8.txt",	"MCS9.txt",  "MCS10.txt", "MCS11.txt"/*, "MCS12.txt"*/};

    for (int i = 0; i < HSDPAMCS_SIZE; i++)
	{
		LoadHSDPAFERTable(filename[i], i);
	}
	//InitRecommendTFRI();
}



double CLinkPrediction::GetHSDPABLER(/* const int& ichanneltype,*/double fMeanEb2N0, int mcs )
{
	//double FER = 0.0;
	//std::vector<TABLE_TYPE>::iterator IterBegin = find_if(HSDPAFERTABLE[cqi.mcs].begin(), 
	//	HSDPAFERTABLE[cqi.mcs].end(), 
	//	bind2nd(mem_fun_ref(&TABLE_TYPE::SameChannelType), ichanneltype)
	//	);//找对应信道类型的第一个值

	//if ( IterBegin == HSDPAFERTABLE[cqi.mcs].end() )
	//{
	//	cerr << "Invalid Channel Type " << __FILE__ << "\t" << __LINE__ << endl;
	//	assert(false);
	//	throw(-5);
	//}

	//std::vector<TABLE_TYPE>::iterator IterEnd = find_if(IterBegin,  HSDPAFERTABLE[cqi.mcs].end(), 
	//	not1(
	//	bind2nd(mem_fun_ref(&TABLE_TYPE::SameChannelType), ichanneltype)
	//		)
	//	);//确定对应信道类型的最后一个值的后一个
	//std::vector<TABLE_TYPE>::iterator IterItem = IterBegin;
	//while(IterItem != IterEnd - 1)//Debug:test 应当加上关于channeltype的判断
	//{
	//	if(IterItem->fMEanEb2N0 >= fMeanEb2N0)
	//	{
	//		break;
	//	}
	//	++IterItem;
	//}
	//return IterItem->fFER;
	std::vector<HSDPATABLE_TYPE>::iterator IterBegin = HSDPAFERTABLE[mcs].begin();
	std::vector<HSDPATABLE_TYPE>::iterator IterItem = HSDPAFERTABLE[mcs].end();
	while(IterBegin != IterItem)
	{
		if(IterBegin->Eb2N0 > fMeanEb2N0)
		{
			break;
		}
		++IterBegin;
	}
	if (IterBegin==HSDPAFERTABLE[mcs].begin())
	{
		return HSDPAFERTABLE[mcs].begin()->FER;
	} 
	else
	{
		return (--IterBegin)->FER;
	}
}

//const vector<HSDPATABLE_TYPE>& CLinkPrediction::GetHSDPABLERTable( const CQI& cqi ) const
//{
//	return HSDPAFERTABLE[cqi.mcs];
//}

void CLinkPrediction::InitRecommendTFRI()
{
//	static TB_Type tbtypes[] = {TB1,TB2,TB3,TB4};
	static MCS_Type mcstypes[] = {MCS1,MCS2, MCS3, MCS4, MCS5, MCS6, MCS7, MCS8/*, MCS9, MCS10, MCS11*/};

	CQI cqi;
	for (int channeltype = channeltypeStartAt; channeltype <= channeltypeEndAt; channeltype++ )//对每种channeltype
	{
		for (int i = 0; i < sizeof(mcstypes) / sizeof(MCS_Type); i++ )//的每种调制和编码方式
		{
					cqi.mcs = mcstypes[i];
					InitThreshHold(channeltype, cqi );//建立各TFRI下的初始目标C2I表	
		}
	}
}

void CLinkPrediction::InitThreshHold(  int channeltype, CQI cqi  )
{
	//const vector<TABLE_TYPE>& blertable = GetHSDPABLERTable(cqi);
	//const TABLE_TYPE* targetbler = FindThreshHold(channeltype, blertable);
	//double C2I = Eb2NoToC2I(dBToRealValue(targetbler->fMEanEb2N0), SF_16, GetCodingGain(cqi), GetModulatorRank(cqi) );
	//m_tfritable[channeltype-channeltypeStartAt][C2I] = cqi;
}
bool EbNoOverThresh( const TABLE_TYPE item) 
{
	static double targetbler = 0.1;
	if(item.fFER <= targetbler)
		return true;
	return false;
}

const TABLE_TYPE* CLinkPrediction::FindThreshHold( int channeltype, const vector<TABLE_TYPE>& blertable ) const
{
	//读入链路级的BLER表,已排好序
	vector<TABLE_TYPE>::const_iterator Thresh = find_if(blertable.begin(), blertable.end(), bind2nd(mem_fun_ref(&TABLE_TYPE::SameChannelType), channeltype) );
	while ( Thresh != blertable.end() && Thresh->iCHannelType == channeltype )
	{
		if(EbNoOverThresh(*Thresh))
			break;
		++Thresh;
	}
	if ( Thresh == blertable.end() || Thresh->iCHannelType != channeltype)//没找到
	{
		assert(false);
		cerr << "Error AMC CQI Selected!\n";
		system("pause");
	}
	return &*Thresh;	
}

const CQI& CLinkPrediction::GetRecommendTFRI( int channeltype, double C2I ) const
{
	std::map<double, CQI>::const_iterator iter;
	static CQI cqi;
	iter = m_tfritable[channeltype - channeltypeStartAt].lower_bound(C2I);
	if ( iter == m_tfritable[channeltype - channeltypeStartAt].end() )//C2I比何一个MCS等级的C2I目标都好
	{		
		cqi.mcs = MCS8;
		return cqi;
	}
	else if ( iter != m_tfritable[channeltype - channeltypeStartAt].begin() && iter->first > C2I )
	{
		--iter;
	}
	return iter->second;
}



//modified by liyintao
void CLinkPrediction::LoadHSDPAFERTable(const char * filename,int mcs)
{
	ifstream file;
	if (CNetWorkDrive::ms_path == "local")//初始文件从本地文件读入
	{
		file.open(filename , ios::in);
	}
	else//初始文件从其它文件读入
	{
		file.open((CNetWorkDrive::ms_path)+filename , ios::in);
	}
	//std::ifstream file((CNetWorkDrive::ms_path)+filename, ios::in);
	if (!file)
	{
		cerr<<"the file "<<(CNetWorkDrive::ms_path)+filename<<"can not be opened"<<endl;
		system("pause");
	}

	while (file)
	{
		HSDPATABLE_TYPE temp;
		file>>/*temp.iCHannelType>>*/temp.Eb2N0>>temp.FER;
		HSDPAFERTABLE[mcs].push_back(temp);
	}
}

void CLinkPrediction::TableInitOfHSUPA()
{
	const char * filename[] = {"MCS1.txt","MCS2.txt","MCS3.txt","MCS4.txt",	"MCS5.txt",
		"MCS6.txt","MCS7.txt",	"MCS8.txt",	"MCS9.txt",  "MCS10.txt", "MCS11.txt", "MCS12.txt"/*, "MCS13.txt"*/};

	for (int i = 0; i < HSUPAMCS_SIZE; i++)
	{
		LoadHSUPAFERTable(filename[i], i);
	}
}

void CLinkPrediction::LoadHSUPAFERTable( const char * filename,int mcs )
{
	ifstream file;
	if (CNetWorkDrive::ms_path == "local")//初始文件从本地文件读入
	{
		file.open(filename , ios::in);
	}
	else//初始文件从其它文件读入
	{
		file.open((CNetWorkDrive::ms_path)+filename , ios::in);
	}
	if (!file)
	{
		cerr<<"the file "<<(CNetWorkDrive::ms_path)+filename<<"can not be opened"<<endl;
		system("pause");
	}

	while (file)
	{
		HSUPATABLE_TYPE temp;
		file>>temp.Eb2N0>>temp.FER;
		HSUPAFERTABLE[mcs].push_back(temp);
	}
}

void CLinkPrediction::TableMCStoC2IInitOfHSUPA()
{
	 double tarFER = 0.1;
	 for(int inum = 0;inum < HSUPAMCS_SIZE;++inum)
	 {
		 double tarEb2N0 = 999;
		 vector<HSUPATABLE_TYPE>::iterator iter = HSUPAFERTABLE[inum].begin();
		 for(;iter != HSUPAFERTABLE[inum].end();++iter)
		 {
			 if(iter->FER < tarFER)
			 {
				 tarEb2N0 = iter->Eb2N0;
				 break;
			 }
		 }
		 if(tarEb2N0 != 999)
		 {
			 double C2I = Eb2NoToC2I(dBToRealValue(tarEb2N0), SF_4);
			 m_MCStoC2I.insert(std::make_pair(inum, C2I));
		 }
		 else
		 {
			 cerr<<"错误的目标Eb2N0值"<<endl;
			 system("pause");
		 }
	 }
}

void CLinkPrediction::TableMCStoC2IInitOfHSDPA()
{
	double tarFER = 0.1;
	for(int inum = 0;inum < HSDPAMCS_SIZE;++inum)
	{
		double tarEb2N0 = 999;
		vector<HSDPATABLE_TYPE>::iterator iter = HSDPAFERTABLE[inum].begin();
		for(;iter != HSDPAFERTABLE[inum].end();++iter)
		{
			if(iter->FER < tarFER)
			{
				tarEb2N0 = iter->Eb2N0;
				break;
			}
		}
		if(tarEb2N0 != 999)
		{
			CQI temp;
			temp.mcs = MCS_Type( inum );
			double C2I = Eb2NoToC2I(dBToRealValue(tarEb2N0), SF_16, GetCodingGain(temp), GetModulatorRank(temp));//扩频因子是？？
			m_MCStoC2IDL.insert(std::make_pair(inum, C2I));
		}
		else
		{
			cerr<<"错误的目标Eb2N0值"<<endl;
			system("pause");
		}
	}
}
double CLinkPrediction::GetTarC2ItoMCS( int mcs ) const
{
	std::map<int,double>::const_iterator iter = m_MCStoC2I.find(mcs);
	if(iter != m_MCStoC2I.end())
	{
		return iter->second;
	}
	else
	{
		cout << "it's here  " << mcs << endl;
		cerr<<"错误的输入MCS等级"<<endl;
		system("pause");
		return -1;
	}
}

int CLinkPrediction::GetMCStoC2I( double c2i )
{
	int CommMCS = 0;
	std::map<int,double>::reverse_iterator r_iter = m_MCStoC2I.rbegin();
	for(;r_iter != m_MCStoC2I.rend();++r_iter)
	{
		if(c2i >= r_iter->second)
		{
			CommMCS = r_iter->first;
			break;
		}
	}
	//if(CommMCS != -1)
	//{
		return CommMCS;
	//}
	//else
	//{
	//	cerr<<"错误的MCS等级选择"<<endl;
	//	system("pause");
	//	return -1;
	//}
}

int CLinkPrediction::GetMCStoC2IDL( double c2i )
{
	MCS_Type CommMCS = MCSUNDEFINED;
	std::map<int,double>::reverse_iterator r_iter = m_MCStoC2IDL.rbegin();
	for(;r_iter != m_MCStoC2IDL.rend();++r_iter)
	{
		if(c2i >= r_iter->second)
		{
			CommMCS = (MCS_Type)r_iter->first;
			break;
		}
	}
	//if(CommMCS != -1)
	//{
	return (int)CommMCS;
	//}
	//else
	//{
	//	cerr<<"错误的MCS等级选择"<<endl;
	//	system("pause");
	//	return -1;
	//}
}
double CLinkPrediction::GetHSUPABLER(double fMeanEb2N0, int mcs)
{
	std::vector<HSUPATABLE_TYPE>::iterator IterBegin = HSUPAFERTABLE[mcs].begin();
	std::vector<HSUPATABLE_TYPE>::iterator IterItem = HSUPAFERTABLE[mcs].end();
	while(IterBegin != IterItem)
	{
		if(IterBegin->Eb2N0 > fMeanEb2N0)
		{
			break;
		}
		++IterBegin;
	}
	if (IterBegin==HSUPAFERTABLE[mcs].begin())
	{
		return HSUPAFERTABLE[mcs].begin()->FER;
	} 
	else
	{
		return (--IterBegin)->FER;
	}
}

double CLinkPrediction::GetTarC2ItoMCSDL( int mcs ) const
{
	std::map<int,double>::const_iterator iter = m_MCStoC2IDL.find(mcs);
	if(iter != m_MCStoC2IDL.end())
	{
		return iter->second;
	}
	else
	{
		cout << "it's here  " << mcs << endl;
		cerr<<"错误的输入MCS等级"<<endl;
		system("pause");
		return -1;
	}
}