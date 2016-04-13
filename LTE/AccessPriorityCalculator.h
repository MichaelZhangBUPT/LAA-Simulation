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
//���ฺ��������ṩNB���ȼ�׼��
class AccessPriorityCalculator
{
protected:
    /**�õ���UE������ָ������*/
    typedef shared_ptr<ILTEUE> UEPTR;
    typedef shared_ptr<IHSPAUE> HSPAUEPTR;
	typedef shared_ptr<IWiFiUE> WiFiUEPTR;
	typedef shared_ptr<ILAAUE> LAAUEPTR;
	
	/**�õ���NodeB������ָ������*/
    typedef shared_ptr<NodeB> NBPTR;
	typedef shared_ptr<HSPANodeB> HSPANBPTR;
	typedef shared_ptr<WiFiAP> WiFiAPPTR;
	typedef shared_ptr<LAANodeB> LAANBPTR;
public:
    ///���캯��
    /*
     *	@param pUE, Ҫִ�н����UE
     */
    AccessPriorityCalculator(const UEPTR& pUE);
	AccessPriorityCalculator(const HSPAUEPTR& pUE);
	AccessPriorityCalculator(const WiFiUEPTR& pUE);
	AccessPriorityCalculator(const LAAUEPTR& pUE);
public:
    ///��������
    virtual ~AccessPriorityCalculator(void);

    ///����()��Ϊ�ȽϺ���
    /*
     * @param pNB1,pNB2, ���Ƚϵ�����NodeB
     * @return ���pNB1��pNB2�Ľ������ȼ����򷵻�true������false
     */
    virtual bool operator() (const NBPTR& pNB1, const NBPTR& pNB2) = 0;
	virtual bool operator() (const HSPANBPTR& pNB1, const HSPANBPTR& pNB2) = 0;
	virtual bool operator() (const WiFiAPPTR& pAP1, const WiFiAPPTR& pAP2) = 0;
	virtual bool operator() (const LAANBPTR& pNB1, const LAANBPTR& pNB2) = 0;
protected:
    /**����Ĵ����ɵ�UE*/
    weak_ptr<ILTEUE> m_pLTEUE;
	weak_ptr<IHSPAUE> m_pHSPAUE;
	weak_ptr<IWiFiUE> m_pWiFiUE;
	weak_ptr<ILAAUE> m_pLAAUE;
};

}
