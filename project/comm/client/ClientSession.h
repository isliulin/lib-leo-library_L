#include "..\ISession.h"
#include "..\..\publish\session\MySocket.h"
#include "..\..\publish\session\CommandPool.h"
#include <map>
#pragma once

// ClientSession command target
class ClientSession : public CAsyncSocket, public ISession, public CommandPool
{
public:
	ClientSession() {
		m_bConnect = false;
		m_bLogin = false;
		m_bDevOnline = FALSE;
		m_recvGroupTempMap.clear();
		ZeroMemory(&m_szProgConfig[0][0], 7*(_MAX_NAME_LEN+1));
		for (int i=0; i<_MAX_REMOTE_FUNC; i++)
		{
			S_DB_TASK_GROUP group;
			m_remoteVec.push_back(group);
		}
	}
	~ClientSession();

	std::vector<HWND> m_pNotifyWndVec;
	void AddNotifyer(const HWND hWnd)
	{
		if (hWnd && ::IsWindow(hWnd))
		{
			if (find( m_pNotifyWndVec.begin( ), m_pNotifyWndVec.end( ), hWnd) == m_pNotifyWndVec.end())
			{
				m_pNotifyWndVec.push_back(hWnd);
			}
		}
	}

	void RemoveNotifyWnd(HWND hWnd)
	{
		std::vector<HWND>::iterator iter = find(m_pNotifyWndVec.begin(), m_pNotifyWndVec.end(), hWnd);
		if (iter != m_pNotifyWndVec.end())
			m_pNotifyWndVec.erase(iter);
	}

	void NotifyMsg(UINT msg, WPARAM wParam=NULL, LPARAM lParam=NULL);

public:
	//
	S_DB_USER_VEC m_user;
	int UserCmd(int cmdType /*= CF_USER_SUB_CMD_LOGIN*/, S_DB_USER& user);
	int GetUsers();
	int ParseUsers(BYTE* pBuf, int nLen);

	//
	int GetLinkIP();
	int SetLinkIP(ULONG net_addr);
	int ParseLinkIP(BYTE* pBuf, int nLen);

	//
	int GetAlarmMode();
	int SetAlarmMode(E_ALARM_MODEL alarmMode=AM_CURRENT_ZONE, BYTE broadVolumeAdd=0);
	int ParseAlarmMode(BYTE* pBuf, int nLen);

	//
	int GetLineInfo();
	int SetLineInfo(SAudioInLineConfig* pConfig);
	int ParseLineInfo(BYTE* pBuf, int nLen);

	//
	int GetZoneName();
	int SetZoneName(int nZone, CString name);
	int SetZoneName(CString name[_MAX_ZONEA]);
	int ParseZoneName(BYTE* pBuf, int nLen);

	// 定时节目
	_DbTaskGroupVec m_taskGroupVec;
	bool m_bProgConfig; // 判断是否下载了
	char m_szProgConfig[7][_MAX_NAME_LEN+1]; // 程序每天配置
	std::map<std::string, int> m_recvGroupTempMap;//临时

	int GetGroup(); // 获取项目(不包含详细任务)
	int GroupAdd(CString& name, BOOL bCopy=FALSE, CString* cpy_name=NULL); // 添加项目
	int GroupDel(CString& name); // 删除某项目
	int GroupEditName(CString& name, CString& newName); // 修改项目名称
	int GetGroupTask(CString& name); // 获取某项目下任务
	int GetGroupTask(char* name); // 获取某项目下任务
	int TaskAdd(char* group_name, S_DB_TASK* pTask); // 添加任务
	int TaskDel(char* group_name, UINT nTimeSec); //删除任务
	int TaskDel(char*  group_name); //删除全部任务
	int TaskEdit(char*  group_name, UINT nTimeSec, S_DB_TASK* pTask); //修改任务
// 	int TaskAdd(CString group_name, S_DB_TASK* pTask); // 添加任务
// 	int TaskDel(CString group_name, UINT nTimeSec); //删除任务
// 	int TaskDel(CString group_name); //删除全部任务
// 	int TaskEdit(CString group_name, UINT nTimeSec, S_DB_TASK* pTask); //修改任务
	int GetProgramConfig(); // 获取运行程序配置信息
	int SetProgramConfig(CString szGroup[7], bool bConfig = false); // 获取运行程序配置信息
	int _DownloadProgramEx(BYTE iDayOfWeek); //星期一：0，....
	int PraseProgramConfig(BYTE*pBuf, int nLen); // 获取运行程序配置信息

	int ParseGroup(BYTE*pBuf, int nLen); // 获取项目(不包含详细任务)
	int ParseGroupTask(BYTE*pBuf, int nLen); // 获取某项目下任务

	// 遥控器功能
	_DbTaskGroupVec m_remoteVec;
	int GetRemoteTask();
	int SetRemoteTask(_DbTaskGroupVec& remoteVec);
	int ParseRemoteTask(BYTE*pBuf, int nLen);
public:
	bool m_bLogin;
	BOOL m_bConnect;
	BOOL m_bDevOnline;
	S_DB_USER m_logon_user;

	sockaddr_in m_saLinkSet; // 服务器连接使用的设备IP地址

	BOOL CreateEx()
	{
		if (CAsyncSocket::Create())
		{
			CAsyncSocket::AsyncSelect();

			BOOL bNoDelay = TRUE;
			SetSockOpt(TCP_NODELAY, &bNoDelay, sizeof(bNoDelay), IPPROTO_TCP);
			int nRecvBuf=8*1024;
			SetSockOpt(SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int), SOL_SOCKET);
			int nSendBuf=4*1024;
			SetSockOpt(SO_SNDBUF, (const char*)&nSendBuf, sizeof(int), SOL_SOCKET);
			return TRUE;
		}
		return FALSE;
	}
	virtual int ISendTo(BYTE* pBuf, int nLen);
	// 	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	CCriticalSection m_cmdCriticalSection;
	virtual void PostCmd(BYTE* pBuf, int nLen);
	// 	virtual void OnClose(int nErrorCode);

	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};

#define WM_ASYNCSOCKET_CONNECT (WM_USER+2100)
#define WM_ASYNCSOCKET_CLOSE (WM_USER+2101)
extern void AlarmConnectError(int nErrorCode);

__declspec(selectany) ClientSession theSes;

