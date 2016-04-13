#pragma once
#include "UE.h"
#include "HSPAUE.h"
#include "DualModeLTEHSPAUE.h"
#include "WiFiUE.h"
#include "LTEWiFiUE.h"
#include "LAAUE.h"

namespace LTESim
{
	class UEMaker
	{
	public:
		UEMaker(void);
	public:
		virtual ~UEMaker(void);
		ILTEUE::PTR CreateLTEUE(int ueid, double posx, double posy, bool coexist, string servicetype);								//�����ҵ���UE�ĳ�ʼ�������ص�ǰ��UE
		IHSPAUE::PTR CreateHSPAUE(int iUEid, double dPosX, double dPosY);
		IDualModeLTEHSPAUE::PTR CreateDualModeLTEHSPAUE(int iUEid, double dPosX, double dPosY);
		IWiFiUE::PTR CreateWiFiUE(int iUEid, double dPosX, double dPosY, bool coexist, string servicetype);
		ILTEWiFiUE::PTR CreateLTEWiFiUE(int iUEid, double dPosX, double dPosY);
		ILAAUE::PTR CreateLAAUE(int iUEid, double dposX, double dposY, bool coexist, string servicetype);

	protected:
		bool m_bFTPVoIPCoexist;	//LTE�û�FTP��VoIPҵ�񹲴�
		int m_iFTPLTEUENum;
		int m_iVoIPLTEUENum;
	};

}
