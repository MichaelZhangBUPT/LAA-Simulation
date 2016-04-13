#pragma once

#include <iostream>

namespace LTESim
{

	class HSPAPhyResource
	{
		public:
			HSPAPhyResource( int CarrierIndex, int CodeChIndex, int AntennaPortIndex, int iFrameIndex, int iSlotIndex );
			HSPAPhyResource();
		public:
			virtual ~HSPAPhyResource(void);
			int			GetCarrierIndex() const;			//得到载波编号
			int			GetCodeChIndex() const;		//得到码道编号
			int			GetAntennaPort() const;			//得到天线端口
			int			GetFrame() const;
			int			GetSlot() const;

			void		SetTxPower(double txpower);	//设置发射功率
			double	GetTxPower()const;				//获得发射功率
			bool operator == (const HSPAPhyResource& res2) const;	//重载“==”
			bool operator < (const HSPAPhyResource& res ) const;
			friend std::ostream& operator << (std::ostream& os,const HSPAPhyResource& res);	//重载输出“<<”
		protected:
			int m_CarrierIndex;				/*载波编号*/
			int m_CodeChIndex;				/*码道编号*/ 
			int m_AntennaPortIndex;		/*天线端口*/
			double m_TxPower;				/*发送功率*/
			int m_iFrameIndex;
			int m_iSlotIndex;
	};
}