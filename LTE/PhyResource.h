#pragma once
#include <iostream>
using std::ostream;
namespace LTESim
{

class PhyResource    //������Դ
{
public:
	PhyResource( int FrameIndex, int SlotIndex, int AntennaPortIndex , int RBIndex );
	PhyResource ();
public:
    virtual ~PhyResource(void);
    virtual int GetRB() const;	//�õ�RB��
    virtual int GetSlot() const;	//�õ�ʱ϶��
    virtual int GetFrames() const;	//�õ�֡��
    virtual int GetAntennaPort() const;	//�õ����߶˿�
	void		 SetTxPower(double txpower);	//���÷��书��
	double		GetTxPower()const;	//��÷��书��
	bool operator==(const PhyResource& res2) const;	//���ء�==��
	friend ostream& operator<<(ostream& os,const PhyResource &res);	//���������<<��
protected:
    int RB;				/*RB��*/
    int slot;			/*ʱ϶��*/
    int Frames;			/*֡��*/ 
    int AntennaPort;	/*���߶˿�*/
	double m_TxPower;  /*���͹���*/
};
//���ء�<����Ҫע��˳��Frames,Slot,RB,AntennaPert
inline bool operator <(const shared_ptr<PhyResource>  &res1, const shared_ptr<PhyResource>  &res2)	 
{
	if (res1->GetFrames() < res2->GetFrames())
	{
		return true;
	}
	else if(res1->GetFrames() > res2->GetFrames())
	{
		return false;
	}

	if (res1->GetSlot() < res2->GetSlot())
	{
		return true;
	}
	else if (res1->GetSlot() > res2->GetSlot())
	{
		return false;
	}

	if (res1->GetRB() < res2->GetRB())
	{
		return true;
	}
	else if (res1->GetRB() > res2->GetRB())
	{
		return false;
	}

	if (res1->GetAntennaPort() < res2->GetAntennaPort())
	{
		return true;
	}
	return false;
}
} 
