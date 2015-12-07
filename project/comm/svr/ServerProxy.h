#include <vector>
#include "ServerSession.h"
#include "..\database.h"
#include "..\..\publish\session\CommandPool.h"
#pragma once


// ServerProxy command target
class ServerProxy : public CAsyncSocket, public CommandPool
{
public:
	ServerProxy();
	~ServerProxy() {}
	BYTE	m_sendBuf[1024]; // the send buffer
	int		m_iSendLen; // the length of last send buffer 
	WPARAM	m_wParam;
public:
	S_DB_USER m_user;
	virtual int ISendTo(BYTE* pBuf, int nLen);
 	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void PostCmd(BYTE* pBuf, int nLen);
	//
	int SendCmdParseErrMsg();
	//
	BOOL ParseSessionCmd(BYTE* pBuf, int nLen);
	// �û��������
	int ParseUserLogin(BYTE* pBuf, int nLen);
	int ParseUserLogout(BYTE* pBuf, int nLen);
	int ParseUserAdd(BYTE* pBuf, int nLen);
	int ParseUserDel(BYTE* pBuf, int nLen);
	int ParseUserEdit(BYTE* pBuf, int nLen);
	int ParseUserGetUser(BYTE* pBuf, int nLen);
	int ParseUserGetUserCount(BYTE* pBuf, int nLen);
	int ParseUserGetUsers(BYTE* pBuf, int nLen);
	// ����IP���豸��
	int ParseLinkIPGet(BYTE* pBuf, int nLen);
	int ParseLinkIPSet(BYTE* pBuf, int nLen);
	// 
	int ParseGetAlarmMode(BYTE* pBuf, int nLen);
	int ParseSetAlarmMode(BYTE* pBuf, int nLen);
	int ParseGetLineInfo(BYTE* pBuf, int nLen);
	int ParseSetLineInfo(BYTE* pBuf, int nLen);
	int ParseGetZoneName(BYTE* pBuf, int nLen);
	int ParseSetZoneName(BYTE* pBuf, int nLen);
	// ��ʱ��Ŀ
	int ParseGetGroup(BYTE*pBuf, int nLen); // ��ȡ��Ŀ(��������ϸ����)
	int ParseGroupAdd(BYTE*pBuf, int nLen); // �����Ŀ
	int ParseGroupDel(BYTE*pBuf, int nLen); // ɾ��ĳ��Ŀ
	int ParseGroupEditName(BYTE*pBuf, int nLen); // ɾ��ĳ��Ŀ
	int ParseGetGroupTask(BYTE*pBuf, int nLen); // ��ȡĳ��Ŀ������
	int RetGroupTask(_DbTaskGroupVec::iterator& iter);
	int ParseSetGroupTask(BYTE*pBuf, int nLen); // �޸�ĳ��Ŀ����
	int ParseGetProgramConfig(BYTE*pBuf, int nLen); // ��ȡ���г���������Ϣ
	int ParseSetProgramConfig(BYTE*pBuf, int nLen); // �������г���������Ϣ
	int ParseProgramDown(BYTE*pBuf, int nLen); // ��������
	// ң����
	int ParseGetRemoteTask(BYTE*pBuf, int nLen);
	int ParseSetRemoteTask(BYTE*pBuf, int nLen);
};

//////////////////////////////////////////////////////////////////////////
typedef std::vector<ServerProxy*> SvrProVec;
class CProxyListenSocket : public CAsyncSocket
{
public:
	CProxyListenSocket() {}
	virtual ~CProxyListenSocket();
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	void ClearInvalidSocket();
public:
	SvrProVec m_serverVec;
};

inline CProxyListenSocket::~CProxyListenSocket() 
{
	for (SvrProVec::size_type ix=0; ix<m_serverVec.size(); ix++)
	{
		if (m_serverVec[ix]->m_hSocket != INVALID_SOCKET)
		{
			m_serverVec[ix]->Close();
		}
		delete m_serverVec[ix];
	}
	m_serverVec.clear();
}

inline void CProxyListenSocket::ClearInvalidSocket()
{
	for (SvrProVec::iterator iter = m_serverVec.begin(); iter != m_serverVec.end(); )
	{
		if ((*iter)->m_hSocket != INVALID_SOCKET)
		{
			iter++;
		}
		else
		{
			delete (*iter);
			iter = m_serverVec.erase(iter);
		}
	}
}

inline void CProxyListenSocket::OnAccept(int nErrorCode)
{
	if (0 == nErrorCode)
	{
		ServerProxy* pClientSocket = new ServerProxy;
		if(this->Accept(*pClientSocket))
		{
			pClientSocket->AsyncSelect();
			BOOL bNoDelay = TRUE;
			pClientSocket->SetSockOpt(TCP_NODELAY, &bNoDelay, sizeof(bNoDelay), IPPROTO_TCP);

			int nRecvBuf=8*1024;
			pClientSocket->SetSockOpt(SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int), SOL_SOCKET);
			int nSendBuf=4*1024;
			pClientSocket->SetSockOpt(SO_SNDBUF, (const char*)&nSendBuf, sizeof(int), SOL_SOCKET);
			//
			pClientSocket->m_wParam = (WPARAM)this;
			m_serverVec.push_back(pClientSocket);
		}
		else
		{
			delete pClientSocket;
		}
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

inline void CProxyListenSocket::OnClose(int nErrorCode)
{
	CAsyncSocket::OnClose(nErrorCode);
}
