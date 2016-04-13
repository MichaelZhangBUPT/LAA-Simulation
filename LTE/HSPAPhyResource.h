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
			int			GetCarrierIndex() const;			//�õ��ز����
			int			GetCodeChIndex() const;		//�õ�������
			int			GetAntennaPort() const;			//�õ����߶˿�
			int			GetFrame() const;
			int			GetSlot() const;

			void		SetTxPower(double txpower);	//���÷��书��
			double	GetTxPower()const;				//��÷��书��
			bool operator == (const HSPAPhyResource& res2) const;	//���ء�==��
			bool operator < (const HSPAPhyResource& res ) const;
			friend std::ostream& operator << (std::ostream& os,const HSPAPhyResource& res);	//���������<<��
		protected:
			int m_CarrierIndex;				/*�ز����*/
			int m_CodeChIndex;				/*������*/ 
			int m_AntennaPortIndex;		/*���߶˿�*/
			double m_TxPower;				/*���͹���*/
			int m_iFrameIndex;
			int m_iSlotIndex;
	};
}