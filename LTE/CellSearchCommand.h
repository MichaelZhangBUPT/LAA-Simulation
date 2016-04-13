#pragma once
#include "Command.h"



namespace LTESim
{
//前置声明
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

//小区搜索命令，按优先级准备一组适合接入的小区
class CellSearchCommand :
            public Command
{
public:
    /**用到的一些指针类型定义*/
    typedef weak_ptr<ILTEUE> UEPTR;
    typedef weak_ptr<NodeB> NBPTR;
    typedef weak_ptr<System> SYSPTR;
public:
    ///构造函数
    /*
     * @param pSys, 要搜索的System对象，取得其所有NodeB进行小区搜索
     * @param pUE, 发起小区搜索的UE对象
     */
    CellSearchCommand(UEPTR pUE);
public:
    /**析构函数*/
    ~CellSearchCommand(void);

    ///Command接口，执行小区搜索命令
    /*
     * @param clk, 触发此Do命令的Clock对象，辅助进行定时判断，此命令不需要
     */
    void Do(const Clock& clk);

protected:
    //准备合适的小区列表
    /*
     *	@param NBListToAccess，从System获得要搜索的小区列表
     */
    void PrepareNBList(std::vector< shared_ptr<NodeB> >& NBListToAccess);
	
	//尝试接纳进指定NodeB
    /*
     * @param pNodeB, 对此NodeB尝试进行接纳申请
     * @return 如果NodeB允许接纳返回true，否则false
     */
    bool AttemptToAccess(const shared_ptr<NodeB>& pNodeB);
	
	//当全体NodeB接纳尝试都失败后执行此函数
    void FailedToAccess();

	//当接纳成功后执行此函数
    /*
     * @param pNodeB, 最终接入的NodeB对象
     */
    void SuccessToAccess( const shared_ptr<NodeB>& pNodeB );
protected:
    UEPTR m_pUE;//执行搜索的UE
    NBPTR m_searchedNb;//搜索到的最适合的NodeB
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
