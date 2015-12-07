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

	// ��ʱ��Ŀ
	_DbTaskGroupVec m_taskGroupVec;
	bool m_bProgConfig; // �ж��Ƿ�������
	char m_szProgConfig[7][_MAX_NAME_LEN+1]; // ����ÿ������
	std::map<std::string, int> m_recvGroupTempMap;//��ʱ

	int GetGroup(); // ��ȡ��Ŀ(��������ϸ����)
	int GroupAdd(CString& name, BOOL bCopy=FALSE, CString* cpy_name=NULL); // �����Ŀ
	int GroupDel(CString& name); // ɾ��ĳ��Ŀ
	int GroupEditName(CString& name, CString& newName); // �޸���Ŀ����
	int GetGroupTask(CString& name); // ��ȡĳ��Ŀ������
	int GetGroupTask(char* name); // ��ȡĳ��Ŀ������
	int TaskAdd(char* group_name, S_DB_TASK* pTask); // �������
	int TaskDel(char* group_name, UINT nTimeSec); //ɾ������
	int TaskDel(char*  group_name); //ɾ��ȫ������
	int TaskEdit(char*  group_name, UINT nTimeSec, S_DB_TASK* pTask); //�޸�����
// 	int TaskAdd(CString group_name, S_DB_TASK* pTask); // �������
// 	int TaskDel(CString group_name, UINT nTimeSec); //ɾ������
// 	int TaskDel(CString group_name); //ɾ��ȫ������
// 	int TaskEdit(CString group_name, UINT nTimeSec, S_DB_TASK* pTask); //�޸�����
	int GetProgramConfig(); // ��ȡ���г���������Ϣ
	int SetProgramConfig(CString szGroup[7], bool bConfig = false); // ��ȡ���г���������Ϣ
	int _DownloadProgramEx(BYTE iDayOfWeek); //����һ��0��....
	int PraseProgramConfig(BYTE*pBuf, int nLen); // ��ȡ���г���������Ϣ

	int ParseGroup(BYTE*pBuf, int nLen); // ��ȡ��Ŀ(��������ϸ����)
	int ParseGroupTask(BYTE*pBuf, int nLen); // ��ȡĳ��Ŀ������

	// ң��������
	_DbTaskGroupVec m_remoteVec;
	int GetRemoteTask();
	int SetRemoteTask(_DbTaskGroupVec& remoteVec);
	int ParseRemoteTask(BYTE*pBuf, int nLen);
public:
	bool m_bLogin;
	BOOL m_bConnect;
	BOOL m_bDevOnline;
	S_DB_USER m_logon_user;

	sockaddr_in m_saLinkSet; // ����������ʹ�õ��豸IP��ַ

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

