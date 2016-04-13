#pragma once
#include "Command.h"



namespace LTESim
{
//ǰ������
class ILTEUE;
class NodeB;
class System;
class Clock;
class IHSPAUE;
class HSPANodeB;
class IWiFiUE;
class WiFiAP;
class ILAAUE;
class LAANodeB;

//С��������������ȼ�׼��һ���ʺϽ����С��
class CellSearchCommand :
            public Command
{
public:
    /**�õ���һЩָ�����Ͷ���*/
    typedef weak_ptr<ILTEUE> UEPTR;
    typedef weak_ptr<NodeB> NBPTR;
    typedef weak_ptr<System> SYSPTR;
public:
    ///���캯��
    /*
     * @param pSys, Ҫ������System����ȡ��������NodeB����С������
     * @param pUE, ����С��������UE����
     */
    CellSearchCommand(UEPTR pUE);
public:
    /**��������*/
    ~CellSearchCommand(void);

    ///Command�ӿڣ�ִ��С����������
    /*
     * @param clk, ������Do�����Clock���󣬸������ж�ʱ�жϣ��������Ҫ
     */
    void Do(const Clock& clk);

protected:
    //׼�����ʵ�С���б�
    /*
     *	@param NBListToAccess����System���Ҫ������С���б�
     */
    void PrepareNBList(std::vector< shared_ptr<NodeB> >& NBListToAccess);
	
	//���Խ��ɽ�ָ��NodeB
    /*
     * @param pNodeB, �Դ�NodeB���Խ��н�������
     * @return ���NodeB������ɷ���true������false
     */
    bool AttemptToAccess(const shared_ptr<NodeB>& pNodeB);
	
	//��ȫ��NodeB���ɳ��Զ�ʧ�ܺ�ִ�д˺���
    void FailedToAccess();

	//�����ɳɹ���ִ�д˺���
    /*
     * @param pNodeB, ���ս����NodeB����
     */
    void SuccessToAccess( const shared_ptr<NodeB>& pNodeB );
protected:
    UEPTR m_pUE;//ִ��������UE
    NBPTR m_searchedNb;//�����������ʺϵ�NodeB
};


class HSPACellSearchCommand
	: public Command
{
protected:
	typedef weak_ptr<IHSPAUE> UEPTR;
	typedef weak_ptr<HSPANodeB> NBPTR;
public:
	HSPACellSearchCommand( shared_ptr<IHSPAUE> pUE );
	virtual ~HSPACellSearchCommand(){};
	virtual void Do( const Clock& clk );
protected:
	void PrepareNBList( std::vector<shared_ptr<HSPANodeB>>& refNBList );
	bool AttemptToAccess( shared_ptr<HSPANodeB> pNodeB );
	void FailedToAccess();
	void SuccessToAccess( shared_ptr<HSPANodeB> pNodeB );
protected:
	UEPTR m_pMasterUE;
	NBPTR m_pMatchNodeB;
};

class WiFiCellSearchCommand
	: public Command
{
protected:
	typedef weak_ptr<IWiFiUE> UEPTR;
	typedef weak_ptr<WiFiAP> APPTR;
public:
	WiFiCellSearchCommand( shared_ptr<IWiFiUE> pUE );
	virtual ~WiFiCellSearchCommand() {};
	virtual void Do( const Clock& clk );
protected:
	void PrepareAPList( std::vector<shared_ptr<WiFiAP>>& refAPList );
	bool AttemptToAccess( shared_ptr<WiFiAP> pAP );
	void FailedToAccess();
	void SuccessToAccess( shared_ptr<WiFiAP> pAP );
protected:
	UEPTR m_pMasterUE;
	APPTR m_pMatchAP;
};

class LAACellSearchCommand
	: public Command
{
protected:
	typedef weak_ptr<ILAAUE> UEPTR;
	typedef weak_ptr<LAANodeB> NBPTR;
public:
	LAACellSearchCommand( shared_ptr<ILAAUE> pUE );
	virtual ~LAACellSearchCommand() {};
	virtual void Do( const Clock& clk );
protected:
	void PrepareNBList( std::vector<shared_ptr<LAANodeB>>& refNBList );
	bool AttemptToAccess( shared_ptr<LAANodeB> pNB );
	void FailedToAccess();
	void SuccessToAccess( shared_ptr<LAANodeB> pNB );
protected:
	UEPTR m_pMasterUE;
	NBPTR m_pMatchNB;
};

}
