#include "..\ISession.h"
#include <list>
#pragma once

struct S_SESSION_CMD
{
	LPARAM lParam; // 存放相关信息，这里存放ServerProxy的实例的指针
	BYTE buf[1024];
	BYTE nLen;
	BYTE nUseableSendTimes;
	S_SESSION_CMD & operator = (S_SESSION_CMD& sec){
		memcpy_s(this, sizeof(S_SESSION_CMD), &sec, sizeof(S_SESSION_CMD));
		return *this;
	}
	void LoadDefault(){
		ZeroMemory(buf, sizeof(buf));
		nLen = 0;
		nUseableSendTimes = 3;
	}
};
typedef std::list<S_SESSION_CMD> TSessionCmdList;

class ServerSession;
typedef CMap<UINT, UINT, ServerSession*, ServerSession*> CTimerMap; 
enum E_ID_TIMER{
	_IDActTimer = 100,
	_IDSendTimer = 200,
	_IDAutoGetHostTimer = 300
};
enum E_CMD_FLAG{
	_CmdFlag_Idle=0, // 空闲
	_CmdFlag_Busy, // 忙（正在通信中）
	_CmdFlag_Wait, // 等待
};

// ServerSession command target
class ServerSession : public CAsyncSocket, public ISession
{
public:
	ServerSession() {
		m_cmdFlag = _CmdFlag_Idle;
		m_bNeedSetTime = true;
		m_countdownToSetHostTime = 10000;
		m_bBoradcast=FALSE;
		m_bDevOnline = true;
	}
	~ServerSession() {}
public:
	int m_countdownToSetHostTime;
	// 声明静态数据成员，映射表类，用于保存所有的定时器信息
	static CTimerMap m_timeMap;
	static void CALLBACK MyTimerActProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
	static void CALLBACK MyTimerSendProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
	static void CALLBACK MyTimerAutoGetHostProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
	void MySetTimer(UINT idEvent, UINT nElapse);
	void MyKillTimer(UINT idEvent);
	void MySetSendTimer(UINT nElapse=300);
	void MySetActTimer(UINT nElapse=300);
	void MySetAutoGetHostStateTimer(UINT nElapse=800);

	TSessionCmdList m_cmdList;
	E_CMD_FLAG m_cmdFlag;
	BOOL AddCmd(BYTE* pBuf, int nLen, LPARAM lParam=NULL, BYTE nUseableSendTimes=3);
	void StartExecuteCmd();
	void CompleteOneCmd();

	void SendToClient(BYTE* pBuf, int nLen, BOOL bAll=TRUE);
public:
	BOOL m_bBoradcast; //是否广播
	virtual int ISendTo(BYTE* pBuf, int nLen);
	virtual int IReceive(BYTE* pBuf, int nLen);
	virtual void OnReceive(int nErrorCode);

	bool m_bNeedSetTime;
	bool m_bDevOnline;
};



__declspec(selectany) ServerSession theSes;

__declspec(selectany) CTimerMap ServerSession::m_timeMap;
#define _afxSockThreadState AfxGetModuleThreadState()
#define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE




class SimulateSession : public ISession
{
public:
	SimulateSession(){
		m_pServerProxy = NULL;
		m_bAddCmdList = FALSE;
	}
	SimulateSession(LPARAM pServerProxy){SetParam(pServerProxy);}
	LPARAM m_pServerProxy;
	BOOL m_bAddCmdList;
	void SetParam(LPARAM pServerProxy, BOOL bAddCmdList = TRUE){
		m_pServerProxy = pServerProxy;
		m_bAddCmdList = bAddCmdList;
	}
	virtual int ISendTo(BYTE* pBuf, int nLen)
	{
		if (m_bAddCmdList)
		{
			theSes.AddCmd(pBuf, nLen, m_pServerProxy);
		}
		return nLen;
	}
};

