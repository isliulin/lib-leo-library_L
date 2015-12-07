#pragma once

// CMySocket command target

class CMySocket
{
public:
	CMySocket();
	virtual ~CMySocket();
public:
	BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM, LPCTSTR lpszSocketAddress = NULL);
	void Close();
	void DetachHandle(SOCKET hSocket);
	BOOL AttachHandle(SOCKET hSocket);
	BOOL Listen(int nConnectionBacklog=5);
	virtual void OnAccept(){}
	BOOL Accept(CMySocket& rConnectedSocket,
		SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);
	BOOL Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void Receive(void* lpBuf, int nBufLen, int nFlags = 0){}
	virtual void OnClose(int nErrorCode){}
	BOOL SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel);
	enum { receives = 0, sends = 1, both = 2 };
	BOOL ShutDown(int nHow = sends);
	BOOL Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);
	BOOL Bind (const SOCKADDR* lpSockAddr, int nSockAddrLen);
private:
	BOOL BeginThread();
	void KillThread();
	static UINT TreadProc(LPVOID pParam);
private:
	HANDLE m_hThread;
	bool m_bThreadStop;
	bool m_bListenSocket;
public:
	SOCKET m_hSocket;
};

inline BOOL CMySocket::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel)
{ return (SOCKET_ERROR != setsockopt(m_hSocket, nLevel, nOptionName, (LPCSTR)lpOptionValue, nOptionLen)); }
inline BOOL CMySocket::ShutDown(int nHow)
{ return (SOCKET_ERROR != shutdown(m_hSocket,nHow)); }

// CMySocket

inline CMySocket::CMySocket()
{
	m_bListenSocket = false;
	m_hThread = NULL;
	m_hSocket = INVALID_SOCKET;
}

inline CMySocket::~CMySocket()
{
	Close();
}

inline BOOL CMySocket::BeginThread()
{
	ASSERT(!m_hThread);

	m_bThreadStop = false;
	CWinThread* pThread = AfxBeginThread(TreadProc, (LPVOID)this);
	if (pThread)
	{
		m_hThread = pThread->m_hThread;
		return TRUE;
	}
	return FALSE;
}

inline void CMySocket::KillThread()
{
	m_bThreadStop = true;
	if (m_hThread)
	{
		::WaitForSingleObject(m_hThread, 10000);
		m_hThread = NULL;
	}
}

inline BOOL CMySocket::Create(UINT nSocketPort /*= 0*/, int nSocketType/*=SOCK_STREAM*/, LPCTSTR lpszSocketAddress)
{
	ASSERT(m_hSocket == INVALID_SOCKET);
	m_hSocket = socket(PF_INET, nSocketType, 0);
	if (m_hSocket != INVALID_SOCKET)
	{
		int nRecvBuf=6*1024;
		SetSockOpt(SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int), SOL_SOCKET);
		int nSendBuf=4*1024;
		SetSockOpt(SO_SNDBUF, (const char*)&nSendBuf, sizeof(int), SOL_SOCKET);

 		if (Bind(nSocketPort,lpszSocketAddress))
			if (BeginThread())
				return TRUE;
		int nResult = GetLastError();
		Close();
		WSASetLastError(nResult);
	}
	return FALSE;
}

inline BOOL CMySocket::Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{ return (SOCKET_ERROR != bind(m_hSocket, lpSockAddr, nSockAddrLen)); }

inline BOOL CMySocket::Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
	USES_CONVERSION_EX;

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	LPSTR lpszAscii;
	if (lpszSocketAddress != NULL)
	{
		lpszAscii = T2A_EX((LPTSTR)lpszSocketAddress, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
		if (lpszAscii == NULL)
		{
			// OUT OF MEMORY
			WSASetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return FALSE;
		}
	}
	else
	{
		lpszAscii = NULL;
	}

	sockAddr.sin_family = AF_INET;

	if (lpszAscii == NULL)
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
	{
		DWORD lResult = inet_addr(lpszAscii);
		if (lResult == INADDR_NONE)
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
		sockAddr.sin_addr.s_addr = lResult;
	}

	sockAddr.sin_port = htons((u_short)nSocketPort);

	return Bind((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

inline void CMySocket::DetachHandle(SOCKET hSocket)
{
	ASSERT(m_hSocket != INVALID_SOCKET);
	KillThread();
	hSocket = m_hSocket;
	m_hSocket = INVALID_SOCKET;
	m_bListenSocket = false;
}

inline BOOL CMySocket::AttachHandle(SOCKET hSocket)
{
	ASSERT(m_hSocket == INVALID_SOCKET);
	ASSERT(hSocket == INVALID_SOCKET);
	m_hSocket = hSocket;
	if (m_hSocket != INVALID_SOCKET)
	{
		if (BeginThread())
			return TRUE;
		int nResult = GetLastError();
		Close();
		WSASetLastError(nResult);
	}
}

inline BOOL CMySocket::Listen(int nConnectionBacklog/*=5*/)
{
	if ((SOCKET_ERROR != listen(m_hSocket, nConnectionBacklog)))
	{
		m_bListenSocket = true;
		return TRUE;
	} 
	return FALSE;
}

inline BOOL CMySocket::Accept(CMySocket& rConnectedSocket,
						  SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{
	ASSERT(rConnectedSocket.m_hSocket == INVALID_SOCKET);

	SOCKET hTemp = accept(m_hSocket, lpSockAddr, lpSockAddrLen);	

	if (hTemp == INVALID_SOCKET)
	{
		DWORD dwProblem = GetLastError();
		rConnectedSocket.m_hSocket = INVALID_SOCKET;
		SetLastError(dwProblem);
	}
	else
	{
		rConnectedSocket.m_hSocket = hTemp;
		rConnectedSocket.AttachHandle(hTemp);
	}

	return (hTemp != INVALID_SOCKET);
}

inline BOOL CMySocket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	if (connect(m_hSocket, (sockaddr*)lpSockAddr, nSockAddrLen)	!= SOCKET_ERROR)
	{
		return TRUE;
	}
	return FALSE;
}

inline void CMySocket::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		VERIFY(SOCKET_ERROR != closesocket(m_hSocket));
		m_hSocket = INVALID_SOCKET;
		m_bListenSocket = false;
		KillThread();
	}
}

inline int CMySocket::Send(const void* lpBuf, int nBufLen, int nFlags/* = 0*/)
{
	if (m_hSocket != INVALID_SOCKET)
		return send(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
	return 0;
}

inline UINT CMySocket::TreadProc(LPVOID pParam)
{
	fd_set fdread;
	int ret;
	timeval tv = {0, 500};
	CMySocket* pSocket = (CMySocket*)pParam;
	while( (!pSocket->m_bThreadStop) && (pSocket->m_hSocket != INVALID_SOCKET) )
	{
		// Always clear the read set before calling select()
		FD_ZERO(&fdread);
		// Add socket m_hSocket to the read set
		FD_SET(pSocket->m_hSocket, &fdread);
		ret=select(0, &fdread, NULL, NULL, &tv);
		if ( 0 == ret )
		{
			// time out
			continue;
		}

		if (FD_ISSET(pSocket->m_hSocket, &fdread))
		{
			if (pSocket->m_bListenSocket)
			{ 
				// Accept 监听套节字接收到新连接
				pSocket->OnAccept();
			} 
			else
			{
				// A read event has occurred on socket s
				char strRecv[1024];
				ret = recv(pSocket->m_hSocket, strRecv, 1024, 0);
				if ( ret == 0 || (ret == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET) )
				{
					//处理发生错误的时候
					pSocket->m_hThread = NULL;
					pSocket->Close();
					pSocket->OnClose(WSAGetLastError());
					return 0;
				}
				else
				{
					if (ret > 0)
					{
						pSocket->Receive(strRecv, ret);
					}
				}
			}
		}
		if (SOCKET_ERROR == ret)
		{
			// Error condition
		} 
		else if (ret > 0)
		{
		}
	}
	pSocket->m_hThread = NULL;
	return 0;
}
