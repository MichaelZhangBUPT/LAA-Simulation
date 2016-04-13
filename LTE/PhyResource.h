#pragma once
#include <iostream>
using std::ostream;
namespace LTESim
{

class PhyResource    //物理资源
{
public:
	PhyResource( int FrameIndex, int SlotIndex, int AntennaPortIndex , int RBIndex );
	PhyResource ();
public:
    virtual ~PhyResource(void);
    virtual int GetRB() const;	//得到RB号
    virtual int GetSlot() const;	//得到时隙号
    virtual int GetFrames() const;	//得到帧号
    virtual int GetAntennaPort() const;	//得到天线端口
	void		 SetTxPower(double txpower);	//设置发射功率
	double		GetTxPower()const;	//获得发射功率
	bool operator==(const PhyResource& res2) const;	//重载“==”
	friend ostream& operator<<(ostream& os,const PhyResource &res);	//重载输出“<<”
protected:
    int RB;				/*RB号*/
    int slot;			/*时隙号*/
    int Frames;			/*帧号*/ 
    int AntennaPort;	/*天线端口*/
	double m_TxPower;  /*发送功率*/
};
//重载“<”。要注意顺序Frames,Slot,RB,AntennaPert
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
