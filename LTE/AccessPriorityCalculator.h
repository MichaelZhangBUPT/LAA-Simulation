#pragma once

namespace LTESim
{
class ILTEUE;
class IHSPAUE;
class IWiFiUE;
class ILAAUE;
class NodeB;
class HSPANodeB;
class WiFiAP;
class LAANodeB;
//该类负责处理接纳提供NB优先级准则
class AccessPriorityCalculator
{
protected:
    /**用到的UE的智能指针类型*/
    typedef shared_ptr<ILTEUE> UEPTR;
    typedef shared_ptr<IHSPAUE> HSPAUEPTR;
	typedef shared_ptr<IWiFiUE> WiFiUEPTR;
	typedef shared_ptr<ILAAUE> LAAUEPTR;
	
	/**用到的NodeB的智能指针类型*/
    typedef shared_ptr<NodeB> NBPTR;
	typedef shared_ptr<HSPANodeB> HSPANBPTR;
	typedef shared_ptr<WiFiAP> WiFiAPPTR;
	typedef shared_ptr<LAANodeB> LAANBPTR;
public:
    ///构造函数
    /*
     *	@param pUE, 要执行接入的UE
     */
    AccessPriorityCalculator(const UEPTR& pUE);
	AccessPriorityCalculator(const HSPAUEPTR& pUE);
	AccessPriorityCalculator(const WiFiUEPTR& pUE);
	AccessPriorityCalculator(const LAAUEPTR& pUE);
public:
    ///析构函数
    virtual ~AccessPriorityCalculator(void);

    ///重载()作为比较函数
    /*
     * @param pNB1,pNB2, 待比较的两个NodeB
     * @return 如果pNB1比pNB2的接纳优先级高则返回true，否则false
     */
    virtual bool operator() (const NBPTR& pNB1, const NBPTR& pNB2) = 0;
	virtual bool operator() (const HSPANBPTR& pNB1, const HSPANBPTR& pNB2) = 0;
	virtual bool operator() (const WiFiAPPTR& pAP1, const WiFiAPPTR& pAP2) = 0;
	virtual bool operator() (const LAANBPTR& pNB1, const LAANBPTR& pNB2) = 0;
protected:
    /**保存的待接纳的UE*/
    weak_ptr<ILTEUE> m_pLTEUE;
	weak_ptr<IHSPAUE> m_pHSPAUE;
	weak_ptr<IWiFiUE> m_pWiFiUE;
	weak_ptr<ILAAUE> m_pLAAUE;
};

}
