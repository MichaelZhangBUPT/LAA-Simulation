#include "commondef.h"

#include "VoIPService.h"
#include "ServiceStatistic.h"
#include "UE.h"
#include "WiFiUE.h"
#include "WiFiAP.h"
#include "ClockManager.h"
#include "TimedCommand.h"
#include "System.h"
#include "RunParameters.h"
#include "SimLog.h"

#include <limits>

namespace LTESim
{

    VoIPService::VoIPService(shared_ptr<IUE> pUE, double delay, double period)
		:	Service( pUE ), m_dPacketDelay(delay),m_iSamplePeriod(period),
		m_UniformGen(RandomManager::GetRandomManager().GetUniformRealGen(0, 1))
    {
		m_SystemType = pUE->GetType();
		m_dLastPacketTime = -m_iSamplePeriod;
		m_ServiceType = VoIP;
		m_iPayload = m_iSamplePeriod*8;	//采用G.729编码方式，后续可改为外部配置
		m_IPPacketList.clear();
		//m_IPPacketList.insert( std::make_pair(m_dLastPacketTime,m_iPayload) );	//初始化第一个包
    }

    VoIPService::~VoIPService(void)
    {
    }

    void VoIPService::UpdateThroughput( DATASIZE throughput ) //更新吞吐量
    {
		map<double,double>::iterator iter = m_IPPacketList.begin();
		if ( throughput>0 && iter!=m_IPPacketList.end() )
		{
			if ( throughput >= iter->second )
			{
				m_SumThroughput += iter->second;
				double dCurrentTime = ClockManager::Instance().GetCurrentTime();
				m_vecPacketTransTime.push_back(dCurrentTime-iter->first);
				m_IPPacketList.erase(iter);
			} 
			else
			{
				m_SumThroughput += throughput;
				iter->second -= throughput;
			}
			++m_iPacketNum;
			m_SumIdealThroughput += throughput;
		}
		else if ( throughput==0 && iter!=m_IPPacketList.end() )	//如果吞吐量为0，即WiFi传输达到最大重传次数，丢弃此包
		{
			m_IPPacketList.erase(iter);
			++m_iDiscardPacketNum;
			++m_iPacketNum;
		}

		
    }

    void VoIPService::UpdateBuffer(unsigned int timespan/*经过的时间间隔ms*/)
    {
		//SetIsEnd(false);
    }

    void VoIPService::UpdateDiscardThroughput( DATASIZE throughput )		//更新丢弃数据包的吞吐量
    {
        m_SumDiscardThroughput += throughput;
		++m_iDiscardPacketNum;
		++m_iPacketNum;
		if( !m_IPPacketList.empty() )
			m_IPPacketList.erase(m_IPPacketList.begin());
    }

    void VoIPService::Accept( ServiceStatistic& visit )										//数据统计访问接口
    {
        visit.Visit(*this);
    }

	void VoIPService::UpdateMCSIndex( int mcsindex )									//更新MCS等级
	{
		++m_MCSIndex[mcsindex];
	}

	void VoIPService::UpdateGenerateInterval( void )
	{
	}

	void VoIPService::UpdateService()
	{
		double dCurrentTime = ClockManager::Instance().GetCurrentTime();
		while ( dCurrentTime - m_dLastPacketTime >= m_iSamplePeriod )
		{
			m_dLastPacketTime += m_iSamplePeriod;
			if ( m_SystemType == SYSTEM_LTE )	//LTE系统内一次产生两个包，代表上下行
			{
				m_IPPacketList.insert( std::make_pair(m_dLastPacketTime,m_iPayload) );
				m_IPPacketList.insert( std::make_pair(m_dLastPacketTime-0.001,m_iPayload) );
			} 
			//else if ( m_SystemType == SYSTEM_WIFI )	//WiFi系统中，为AP的虚拟UE生成包
			//{
			//	m_IPPacketList.insert( std::make_pair(m_dLastPacketTime,m_iPayload) );
			//	shared_ptr<System> pSystem = System::GetCurrentSystem();
			//	size_t uenum = pSystem->GetWiFiUEList().size();
			//	if ( m_pUE.lock() -> GetID() >= uenum )	//AP的虚拟UE
			//	{
			//		int iWiFiVoIPUENo = 0;
			//		bool bWiFiFTPVoIPCoexist = pSystem->GetConfig().GetBool("WiFi_FTP_VoIP_Coexist");
			//		string szServiceName = pSystem->GetConfig().GetString( "WiFi_UE_Service" );
			//		if ( bWiFiFTPVoIPCoexist )
			//		{
			//			iWiFiVoIPUENo = (floor( pSystem->GetConfig().GetInt("WiFi_UE_Number")*pSystem->GetConfig().GetDouble("WiFi_VoIP_UE_Percentage")+0.5 ));
			//		} 
			//		else if ( szServiceName == "VoIP" )
			//		{
			//			iWiFiVoIPUENo = pSystem->GetConfig().GetInt("WiFi_UE_Number");
			//		}

			//		if ( !iWiFiVoIPUENo )	//WiFiUE没有VoIP业务的话，AP的虚拟UE清空业务包
			//		{
			//			m_IPPacketList.clear();
			//		} 
			//		else
			//		{
			//			for ( int i=1; i<iWiFiVoIPUENo; i++ )
			//			{
			//				m_IPPacketList.insert( std::make_pair(m_dLastPacketTime-0.001*i,m_iPayload) );
			//			}
			//		}
			//	}
			//}

		}

		map<double,double>::iterator iter = m_IPPacketList.begin();
		while ( iter != m_IPPacketList.end() )
		{
			if ( dCurrentTime - iter->first >= m_dPacketDelay )
			{	//超过最大延迟就丢弃这个IP包
				m_IPPacketList.erase(iter++);
				++m_iDiscardPacketNum;
				++m_iPacketNum;
				//std::cout<<endl<<"beyond VoIP packet delay"<<endl;
			}
			else
			{
				break;
			}
		}
		//检查是否有IP包需要传
		if ( iter != m_IPPacketList.end() )
		{
			SetIsEnd(false);
			SetData( iter->second );
		} 
		else
		{
			SetIsEnd(true);
			SetData(0.0);
		}
	}

	double VoIPService::GetData() const
	{
		if ( m_IPPacketList.empty() )
		{
			return 0.0;
		}
		return m_IPPacketList.begin() -> second;
	}

}

