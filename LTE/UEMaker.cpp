#include "commondef.h"
#include "UEMaker.h"
#include "DLFTPService.h"
#include "Streamservice.h"
#include "VoIPService.h"
#include "FullBufferService.h"
#include "System.h"
#include "RunParameters.h"
#include "HSPAUE.h"
#include "DiversityAntenna.h"
#include "DualModeFTPService.h"

namespace LTESim
{

	UEMaker::UEMaker(void)
	{
	}

	UEMaker::~UEMaker(void)
	{
	}
	
	//完成有业务的UE的初始化并返回当前的UE
	ILTEUE::PTR UEMaker::CreateLTEUE( int ueid, double posx, double posy, bool coexist, string servicetype )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

        double antennaGain = dBToReal(pSystem->GetConfig().GetDouble("UE_Antenna_Gain(db)"));
		int iUEAntennaPortNum = pSystem->GetConfig().GetInt("UEAntennaPort");
		shared_ptr<Antenna> pRxAntenna(new DiversityAntenna(antennaGain));
		pRxAntenna->SetAntennaPortNum(iUEAntennaPortNum);//接收天线
		ILTEUE::PTR pUE(new UE(ueid, POS(posx, posy), pRxAntenna));

		string szServiceType;
		if ( coexist ) //如果FTP和VoIP业务共存，则业务类型由传递进来的参数决定
			szServiceType = servicetype;
		else			//不共存，即单一业务类型，则由配置文件的参数决定
			szServiceType = pSystem->GetConfig().GetString( "Traffic_Model" );

		if ( szServiceType == "FullBuffer" )
		{
			shared_ptr<Service> pService( new FullBufferService( pUE, DOWNLINK ) );
			pUE->AddService( pService );
		}
		else if ( szServiceType == "FTP_Model_2" )
		{
			double dFileSize = pSystem->GetConfig().GetInt( "FTP_Model2_File_Size(byte)" );
			double dReadingTimeMean = pSystem->GetConfig().GetDouble( "FTP_Model2_Reading_Time_Mean(s)" );
			shared_ptr<Service> pFTPService( new DLFTPService(pUE, dFileSize, dReadingTimeMean ) );
			pUE->AddService(pFTPService);		//添加服务
		}
		else if ( szServiceType == "FTP_Model_1" )
		{

		}
		else if ( szServiceType == "VoIP" )
		{
			double period = pSystem->GetConfig().GetInt( "VoIP_Sample_Period(ms)" );
			double delay = pSystem->GetConfig().GetInt("VoIP_Packet_Delay(ms)");
			shared_ptr<Service> pService( new VoIPService( pUE, delay, period ) );
			pUE->AddService(pService);
		}
		else if (szServiceType == "Stream")
		{
			double PacketSize = pSystem->GetConfig().GetInt( "Stream_Packet_Size(byte)" );
			shared_ptr<Service> pService( new StreamService( pUE, PacketSize ));
			pUE->AddService( pService );
		}
		else
			throw std::invalid_argument( "未识别的Service类型" );

		pUE->Initialization();
		
		//还需要将FTP业务加入UE
		return pUE;
	}

	IHSPAUE::PTR UEMaker::CreateHSPAUE( int iUEid, double dPosX, double dPosY )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		double antennaGain = dBToReal(pSystem->GetConfig().GetDouble("UE_Antenna_Gain(db)"));
		int iUEAntennaPortNum = pSystem->GetConfig().GetInt("UEAntennaPort");
		shared_ptr<Antenna> pRxAntenna(new DiversityAntenna(antennaGain));
		pRxAntenna->SetAntennaPortNum(iUEAntennaPortNum);//接收天线
		IHSPAUE::PTR pUE( new HSPAUE( iUEid, POS( dPosX, dPosY ) ) );
		pUE->SetAntenna( pRxAntenna );
		const std::string& szServiceName = pSystem->GetConfig().GetString("HSPA_UE_Service");
		if ( szServiceName == "FTP_Model_2" )
		{
			double dFileSize = pSystem->GetConfig().GetInt( "FTP_Model2_File_Size(byte)" );
			double dReadingTimeMean = pSystem->GetConfig().GetDouble( "FTP_Model2_Reading_Time_Mean(s)" );
			shared_ptr<Service> pService( new DLFTPService( pUE, dFileSize, dReadingTimeMean ) );
			pUE->AddService( pService );
		}
		else if (szServiceName == "FullBuffer")
		{
			shared_ptr<Service> pService( new FullBufferService( pUE, DOWNLINK ) );
			pUE->AddService( pService );
		}
		else if ( szServiceName == "VoIP" )
		{
			double period = pSystem->GetConfig().GetInt( "VoIP_Sample_Period(ms)" );
			double delay = pSystem->GetConfig().GetInt("VoIP_Packet_Delay(ms)");
			shared_ptr<Service> pService( new VoIPService( pUE, delay, period ) );
			pUE->AddService(pService);
		}
		else if ( szServiceName == "Stream" )
		{
			double PacketSize = pSystem->GetConfig().GetInt( "Stream_Packet_Size(byte)" );
			shared_ptr<Service> pService( new StreamService( pUE, PacketSize ));
			pUE->AddService( pService );
		}
		else
			throw std::invalid_argument( "未识别的HSPA UE的业务类型" );

		pUE->Initialization();
		return pUE;
	}

	IDualModeLTEHSPAUE::PTR UEMaker::CreateDualModeLTEHSPAUE(int iUEid, double dPosX, double dPosY)
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		//Dual Mode UE在LTE端的天线配置
		double dAntennaGain = dBToReal(pSystem->GetConfig().GetDouble("UE_Antenna_Gain(db)"));
		int iUEAntennaPortNum = pSystem->GetConfig().GetInt("UEAntennaPort");
		shared_ptr<Antenna> pRxAntenna(new DiversityAntenna(dAntennaGain));
		pRxAntenna->SetAntennaPortNum(iUEAntennaPortNum);//接收天线

		IDualModeLTEHSPAUE::PTR pUE( new DualModeLTEHSPAUE( iUEid, dPosX, dPosY, pRxAntenna ) );
		//设置业务
		const std::string& szServiceName = pSystem->GetConfig().GetString("DualMode_UE_Service");
		if ( szServiceName == "FTP_Model_2" )
		{
			double dFileSize = pSystem->GetConfig().GetInt( "FTP_Model2_File_Size(byte)" );
			double dReadingTimeMean = pSystem->GetConfig().GetDouble( "FTP_Model2_Reading_Time_Mean(s)" );
			shared_ptr<Service> pService( DualModeFTPService::CreateInstance( pUE, dFileSize, dReadingTimeMean ) );
			pUE->AddService( pService );
		}
		else if ( szServiceName == "FullBuffer" )
		{
			double dFileSize = std::numeric_limits<double>::max();
			double dReadingTimeMean = pSystem->GetConfig().GetDouble( "FTP_Model2_Reading_Time_Mean(s)" );
			shared_ptr<Service> pService( DualModeFTPService::CreateInstance( pUE, dFileSize, dReadingTimeMean ) );
			pUE->AddService( pService );
		}
		else
			throw std::invalid_argument( "Unknown Service for Dual Mode UE" );

		pUE->Initialization();
		return pUE;
	}

	IWiFiUE::PTR UEMaker::CreateWiFiUE( int iUEid, double dPosX, double dPosY, bool coexist, string servicetype )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		double antennaGain = dBToReal(pSystem->GetConfig().GetDouble("WiFi_UE_Antenna_Gain(dB)"));
		int iUEAntennaPortNum = pSystem->GetConfig().GetInt("WiFi_UE_Antenna_Port");
		shared_ptr<Antenna> pRxAntenna(new DiversityAntenna(antennaGain));
		pRxAntenna->SetAntennaPortNum(iUEAntennaPortNum);//接收天线
		IWiFiUE::PTR pUE( new WiFiUE( iUEid, POS( dPosX, dPosY ), pRxAntenna) );

		string szServiceName;
		if ( coexist ) //如果FTP和VoIP业务共存，则业务类型由传递进来的参数决定
			szServiceName = servicetype;
		else			//不共存，即单一业务类型，则由配置文件的参数决定
			szServiceName = pSystem->GetConfig().GetString( "WiFi_UE_Service" );

		if ( szServiceName == "FTP_Model_2" )
		{
			double dFileSize = pSystem->GetConfig().GetInt( "FTP_Model2_File_Size(byte)" );
			double dReadingTimeMean = pSystem->GetConfig().GetDouble( "FTP_Model2_Reading_Time_Mean(s)" );
			shared_ptr<Service> pService( new DLFTPService( pUE, dFileSize, dReadingTimeMean ) );
			pUE->AddService( pService );
		}
		else if (szServiceName == "FullBuffer")
		{
			shared_ptr<Service> pService( new FullBufferService( pUE, DOWNLINK ) );
			pUE->AddService( pService );
		}
		else if ( szServiceName == "Stream" )
		{
			double PacketSize = pSystem->GetConfig().GetInt( "Stream_Packet_Size(byte)" );
			shared_ptr<Service> pService( new StreamService( pUE, PacketSize ));
			pUE->AddService( pService );
		}
		else if ( szServiceName == "VoIP" )
		{
			double period = pSystem->GetConfig().GetInt( "VoIP_Sample_Period(ms)" );
			double delay = pSystem->GetConfig().GetInt("VoIP_Packet_Delay(ms)");
			shared_ptr<Service> pService( new VoIPService( pUE, delay, period ) );
			pUE->AddService(pService);
		}
		else
			throw std::invalid_argument( "未识别的WiFi UE的业务类型" );

		pUE->Initialization();
		return pUE;
	}

	ILTEWiFiUE::PTR UEMaker::CreateLTEWiFiUE(int iUEid, double dPosX, double dPosY)
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		//Dual Mode UE在LTE端的天线配置
		double dAntennaGain = dBToReal(pSystem->GetConfig().GetDouble("UE_Antenna_Gain(db)"));
		int iUEAntennaPortNum = pSystem->GetConfig().GetInt("UEAntennaPort");
		shared_ptr<Antenna> pRxAntenna(new DiversityAntenna(dAntennaGain));
		pRxAntenna->SetAntennaPortNum(iUEAntennaPortNum);//接收天线

		ILTEWiFiUE::PTR pUE( new LTEWiFiUE( iUEid, dPosX, dPosY, pRxAntenna ) );
		//设置业务
		const std::string& szServiceName = pSystem->GetConfig().GetString("DualMode_UE_Service");
		if ( szServiceName == "FTP_Model_2" )
		{
			double dFileSize = pSystem->GetConfig().GetInt( "FTP_Model2_File_Size(byte)" );
			double dReadingTimeMean = pSystem->GetConfig().GetDouble( "FTP_Model2_Reading_Time_Mean(s)" );
			shared_ptr<Service> pService( DualModeFTPService::CreateInstance( pUE, dFileSize, dReadingTimeMean ) );
			pUE->AddService( pService );
		}
		else if ( szServiceName == "FullBuffer" )
		{
			double dFileSize = std::numeric_limits<double>::max();
			double dReadingTimeMean = pSystem->GetConfig().GetDouble( "FTP_Model2_Reading_Time_Mean(s)" );
			shared_ptr<Service> pService( DualModeFTPService::CreateInstance( pUE, dFileSize, dReadingTimeMean ) );
			pUE->AddService( pService );
		}
		else
			throw std::invalid_argument( "Unknown Service for Dual Mode UE" );

		pUE->Initialization();
		return pUE;
	}

	ILAAUE::PTR UEMaker::CreateLAAUE( int iUEid, double dposX, double dposY, bool coexist, string servicetype )
	{
		shared_ptr<System> pSystem = System::GetCurrentSystem();

		double antennaGain = dBToReal(pSystem->GetConfig().GetDouble("LAA_UE_Antenna_Gain(dB)"));
		int iUEAntennaPortNum = pSystem->GetConfig().GetInt("LAA_UE_Antenna_Port");
		shared_ptr<Antenna> pRxAntenna(new DiversityAntenna(antennaGain));
		pRxAntenna->SetAntennaPortNum(iUEAntennaPortNum);//接收天线
		ILAAUE::PTR pUE( new LAAUE( iUEid, POS( dposX, dposY ), pRxAntenna) );

		string szServiceName;
		if ( coexist ) //如果FTP和VoIP业务共存，则业务类型由传递进来的参数决定
			szServiceName = servicetype;
		else			//不共存，即单一业务类型，则由配置文件的参数决定
			szServiceName = pSystem->GetConfig().GetString( "LAA_UE_Service" );

		if ( szServiceName == "FTP_Model_2" )
		{
			double dFileSize = pSystem->GetConfig().GetInt( "FTP_Model2_File_Size(byte)" );
			double dReadingTimeMean = pSystem->GetConfig().GetDouble( "FTP_Model2_Reading_Time_Mean(s)" );
			shared_ptr<Service> pService( new DLFTPService( pUE, dFileSize, dReadingTimeMean ) );
			pUE->AddService( pService );
		}
		else if (szServiceName == "FullBuffer")
		{
			shared_ptr<Service> pService( new FullBufferService( pUE, DOWNLINK ) );
			pUE->AddService( pService );
		}
		else if ( szServiceName == "Stream" )
		{
			double PacketSize = pSystem->GetConfig().GetInt( "Stream_Packet_Size(byte)" );
			shared_ptr<Service> pService( new StreamService( pUE, PacketSize ));
			pUE->AddService( pService );
		}
		else if ( szServiceName == "VoIP" )
		{
			double period = pSystem->GetConfig().GetInt( "VoIP_Sample_Period(ms)" );
			double delay = pSystem->GetConfig().GetInt("VoIP_Packet_Delay(ms)");
			shared_ptr<Service> pService( new VoIPService( pUE, delay, period ) );
			pUE->AddService(pService);
		}
		else
			throw std::invalid_argument( "未识别的LAA UE的业务类型" );

		pUE->Initialization();
		return pUE;
	}

}

