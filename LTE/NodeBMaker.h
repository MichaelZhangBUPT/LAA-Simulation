#pragma once

#include "NodeB.h"
#include "HSPANodeB.h"
#include "WiFiAP.h"
#include "LAANodeB.h"

namespace LTESim
{

class NodeB;
class HSPANodeB;
class NodeBMaker
{
public:
    NodeBMaker(void);
public:
    virtual ~NodeBMaker(void);
    NodeB::PTR			CreateNodeB(int nbid, double posx, double posy);	//���NodeB�ĳ�ʼ������õ�ǰNodeB
	HSPANodeB::PTR	CreateHSPANodeB(int iNBID, double dPosX, double dPoxY);
	WiFiAP::PTR		CreateWiFiAP(int iAPID, double dPosX, double dPosY);
	LAANodeB::PTR  CreateLAANodeB(int iLAANBID, double posX, double posY);
};

}
