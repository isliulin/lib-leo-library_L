#include "stdafx.h"
#include "ServerSession.h"
#include "ServerProxy.h"
#include "..\..\Server\Server.h"

#define MY_INLINE 

MY_INLINE void CALLBACK ServerSession::MyTimerAutoGetHostProc(HWND hwnd,UINT uMsg,UINT idEvent, DWORD dwTime)
{
	if (!m_timeMap.GetSize())
		return;

	//
	ServerSession* pRt;
	if (!m_timeMap.Lookup(idEvent, pRt))
		return;
	pRt->m_countdownToSetHostTime--;
	if (pRt->m_countdownToSetHostTime <= 0)
	{
		pRt->m_countdownToSetHostTime = 10000;
		pRt->m_bNeedSetTime = true;
	}
	// 没有命令处理，关闭定时器
	if (pRt->m_cmdList.size() == 0)
	{
		SimulateSession ss;
		ss.SetParam(NULL);

		if (pRt->m_bDevOnline)
		{
			ss._GetHostState();
		} 
		else
		{
			ss._GetSerialNumAndMac();
		}
	}
}

MY_INLINE void CALLBACK ServerSession::MyTimerSendProc(HWND hwnd,UINT uMsg,UINT idEvent, DWORD dwTime)
{
	if (!m_timeMap.GetSize())
		return;
	//
	ServerSession* pRt;
	if (!m_timeMap.Lookup(idEvent, pRt))
		return;
	// 没有命令处理，关闭定时器
	if (pRt->m_cmdList.size() == 0)
	{
		pRt->MyKillTimer(idEvent);
		return;
	}
	//
	if (_CmdFlag_Idle == pRt->m_cmdFlag)
	{
		// 执行下一条命令
		pRt->StartExecuteCmd();
	}
}

MY_INLINE void CALLBACK ServerSession::MyTimerActProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	if (!m_timeMap.GetSize())
		return;
	ServerSession* pRt;
	if (!m_timeMap.Lookup(idEvent, pRt))
		return;
	//
	if (pRt->m_cmdList.size() == 0)
	{
		ASSERT(FALSE);
		pRt->MyKillTimer(idEvent);
		return;
	}
	//
	TSessionCmdList::iterator iter = pRt->m_cmdList.begin();
	if (iter->nUseableSendTimes != 0)
	{
		//
		iter->nUseableSendTimes--;
		pRt->ISendTo(iter->buf, iter->nLen);
	}
	else
	{
		pRt->m_bDevOnline = false;
		pRt->m_cmdFlag = _CmdFlag_Idle;
		pRt->MyKillTimer(idEvent);
		// 通知发送未成功
		TSessionCmdList::iterator iterCmd = pRt->m_cmdList.begin();
		BYTE buf[] = {CF_ERR_CONNECT, iterCmd->buf[0], iterCmd->buf[1]};
		pRt->SendToClient(buf, 3);
		// 清空命令堆
		pRt->m_cmdList.clear();
	}
}

MY_INLINE void ServerSession::MySetTimer(UINT idEvent, UINT nElapse)
{
	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
	ASSERT(pState->m_hSocketWindow != NULL);

	UINT nTimerID;
	if (_IDSendTimer == idEvent)
	{
		nTimerID = ::SetTimer(pState->m_hSocketWindow, _IDSendTimer, nElapse, MyTimerSendProc);
		ASSERT(_IDSendTimer == nTimerID);
	} 
	else if (_IDActTimer == idEvent)
	{
		nTimerID = ::SetTimer(pState->m_hSocketWindow, _IDActTimer, nElapse, MyTimerActProc);
		ASSERT(_IDActTimer == nTimerID);
	}
	else if (_IDAutoGetHostTimer == idEvent)
	{
		nTimerID = ::SetTimer(pState->m_hSocketWindow, _IDAutoGetHostTimer, nElapse, MyTimerAutoGetHostProc);
		ASSERT(_IDAutoGetHostTimer == nTimerID);
	}
	else
	{
		ASSERT(FALSE);
		return;
	}
	m_timeMap[nTimerID] = this;
}

MY_INLINE void ServerSession::MyKillTimer(UINT idEvent)
{
	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
	ASSERT(pState->m_hSocketWindow != NULL);

	KillTimer(pState->m_hSocketWindow, idEvent);
	m_timeMap.RemoveKey(idEvent);
}

MY_INLINE void ServerSession::MySetSendTimer(UINT nElapse/*=300*/)
{
	MySetTimer(_IDSendTimer, nElapse);
}

MY_INLINE void ServerSession::MySetActTimer(UINT nElapse/* =300 */)
{
	MySetTimer(_IDActTimer, nElapse);
}

MY_INLINE void ServerSession::MySetAutoGetHostStateTimer(UINT nElapse/*=500*/)
{
	MySetTimer(_IDAutoGetHostTimer, nElapse);
}


BOOL ServerSession::AddCmd(BYTE* pBuf, int nLen, LPARAM lParam/* =NULL */, BYTE nUseableSendTimes/* =3 */)
{
	if (nLen>=1024)
	{
		return FALSE;
	}

	if (pBuf[0] == CF_GET_HOST_STATE) // 未处理独占模式时的问题
	{
		if (m_cmdList.size())
			return FALSE;
	}

// 	ServerProxy* pProxy = (ServerProxy*) lParam;
// 	if (INVALID_SOCKET == pProxy->m_hSocket || _UT_NULL == pProxy->m_user.type)
// 	{
// 		ASSERT(FALSE);
// 		return FALSE;
// 	}
	S_SESSION_CMD cmd;
	cmd.lParam = lParam;
	memcpy_s(cmd.buf, sizeof(cmd.buf), pBuf, nLen);
	cmd.nLen = nLen;
	cmd.nUseableSendTimes = nUseableSendTimes;

	switch (cmd.buf[0])
	{
	case CF_GET_IP_ADDR:
	case CF_SET_IP_ADDR:
	case CF_GET_SERIAL_NUM:
	case CF_SET_SERIAL_NUM:
	case CF_SET_REGISTER_NUM:
		m_cmdList.push_front(cmd);
		break;
	default:
		m_cmdList.push_back(cmd);
		break;
	}

	// 立即发送命令
	StartExecuteCmd();
	return TRUE;
}

MY_INLINE void ServerSession::StartExecuteCmd()
{
	if (_CmdFlag_Idle == m_cmdFlag && m_cmdList.size())
	{
		TSessionCmdList::iterator iter = m_cmdList.begin();
		if (m_bNeedSetTime && iter->buf[0] == CF_GET_HOST_STATE) 
		{
			_GetHostState(TRUE, &CTime::GetCurrentTime(), m_sys.alarmMode, m_sys.broadVolumeAdd);
			m_bNeedSetTime = false;
		}
		else
			ISendTo(iter->buf, iter->nLen);
		MySetActTimer(__GetElapseTimer(iter->buf[0]));
		// 如果为控制命令，直接完成该命令
		if (!__IsRetCmd(iter->buf[0]))
		{
			CompleteOneCmd();
		}

		MySetSendTimer(); // 启动定时器，以便有后续命令来到。若已经启动则重置，开始新的计时周期
	}
}

MY_INLINE void ServerSession::CompleteOneCmd()
{
	if (m_cmdList.size())
	{
		MyKillTimer(_IDActTimer);
		m_cmdFlag = _CmdFlag_Idle;

		// 发送到客户端确认完成命令
		TSessionCmdList::iterator iterCmd = m_cmdList.begin();

		switch (iterCmd->buf[0])
		{
		case CF_PROGRAM_DOWN_ENTRY:
		case CF_PROGRAM_DOWN:
		case CF_PROGRAM_END:
			Sleep(10);
			break;
		}

		SendToClient(iterCmd->buf, 1, FALSE);

		m_cmdList.pop_front();
		if (0 == m_cmdList.size())
		{
			MyKillTimer(_IDSendTimer);
		}
	}
}

MY_INLINE int ServerSession::IReceive(BYTE* pBuf, int nLen)
{
	ISession::IReceive(pBuf, nLen);

	// 发送到客户端
	SendToClient(pBuf, nLen);
	
	// 2.判断命令是否完成
	TSessionCmdList::iterator iter = m_cmdList.begin();
	//
	if (iter != m_cmdList.end())
	{
		if (iter->nUseableSendTimes != 0)
		{
			if ('B' == pBuf[3] && CF_GET_HOST_STATE == iter->buf[0])
			{
				CompleteOneCmd();
			}
			else if (pBuf[8] == __GetCmdRetByte(iter->buf[0]))
			{
				CompleteOneCmd();
			}
		}
	}
	return 0;
}

MY_INLINE int ServerSession::ISendTo(BYTE* pBuf, int nLen)
{
	int iRet = -1;
	BYTE sendBuf[1024] = {'i','t','c'};
	memcpy_s(&sendBuf[3], 1020, pBuf, nLen);

	// 判断是否广播
	static sockaddr_in saBorad;
	switch (pBuf[0])
	{
	case CF_GET_IP_ADDR:
	case CF_SET_IP_ADDR:
	case CF_GET_SERIAL_NUM:
	case CF_SET_SERIAL_NUM:
	case CF_SET_REGISTER_NUM:
		if (!m_bBoradcast)
		{
			m_bBoradcast = TRUE;
			if(!CAsyncSocket::SetSockOpt(SO_BROADCAST,&m_bBoradcast,sizeof(m_bBoradcast),SOL_SOCKET))
			{
				ASSERT(FALSE);
				return 0; 
			}
		}
		saBorad.sin_family = m_saTarget.sin_family;
		saBorad.sin_addr.S_un.S_addr = inet_addr("255.255.255.255");
		saBorad.sin_port = m_saTarget.sin_port;
		iRet = CAsyncSocket::SendTo(sendBuf, nLen+3, (SOCKADDR*)&saBorad, sizeof(saBorad));
		break;
	default:
		if (m_bBoradcast)
		{
			m_bBoradcast = FALSE;
			if (!CAsyncSocket::SetSockOpt(SO_BROADCAST, &m_bBoradcast, sizeof(m_bBoradcast), SOL_SOCKET))
			{
				ASSERT(FALSE);
				return 0;
			}
		}
		iRet = CAsyncSocket::SendTo(sendBuf, nLen+3, (SOCKADDR*)&m_saTarget, sizeof(m_saTarget));
	}
	m_cmdFlag = _CmdFlag_Busy;
	return iRet;
}

#define PR_HEAD_LEN 3
MY_INLINE void ServerSession::OnReceive(int nErrorCode)
{
	sockaddr_in saTarget;
	saTarget.sin_family = m_saTarget.sin_family ;
	saTarget.sin_port = m_saTarget.sin_port;
	saTarget.sin_addr.S_un.S_addr = m_saTarget.sin_addr.S_un.S_addr;
	if (m_bBoradcast)
	{
		saTarget.sin_addr.S_un.S_addr = inet_addr("255.255.255.255");
	}
	int saClientLen = sizeof(m_saTarget);
	m_iRecvLen = ReceiveFrom(m_recvBuf, 1460, (SOCKADDR*)&saTarget, &saClientLen);

	static BYTE szHead[] = {'i','t','c'/*,'b','r','o','a','d'*/};
	if (m_iRecvLen>PR_HEAD_LEN)
	{
		if (memcmp(m_recvBuf, szHead, PR_HEAD_LEN) == 0)
		{
			if (saTarget.sin_family != m_saTarget.sin_family ||
				saTarget.sin_port != m_saTarget.sin_port ||
				saTarget.sin_addr.S_un.S_addr != m_saTarget.sin_addr.S_un.S_addr)
			{
				TRACE("ServerSession::OnReceive DEV IP CHANGE \n");
				SetTargetSockAddr(saTarget.sin_addr.S_un.S_addr, saTarget.sin_port);
				theDataBase.SetNetParameter(saTarget.sin_addr.S_un.S_addr, saTarget.sin_port);
				m_bNeedSetTime = true;
				int nPos = 0;
				BYTE buf[50];
				buf[nPos++] = CF_LINK_IP;
				buf[nPos++] = CF_SUB_LINK_IP_GET;
				buf[nPos++] = m_saTarget.sin_addr.S_un.S_un_b.s_b1;
				buf[nPos++] = m_saTarget.sin_addr.S_un.S_un_b.s_b2;
				buf[nPos++] = m_saTarget.sin_addr.S_un.S_un_b.s_b3;
				buf[nPos++] = m_saTarget.sin_addr.S_un.S_un_b.s_b4;
				buf[nPos++] = (char)m_saTarget.sin_port;
				buf[nPos++] = (char)(m_saTarget.sin_port>>8);
				SendToClient(buf, nPos);
			}

			m_bDevOnline = true;
			IReceive(m_recvBuf, m_iRecvLen);
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

MY_INLINE void ServerSession::SendToClient(BYTE* pBuf, int nLen, BOOL bAll/*=TRUE*/)
{
	if (bAll)
	{
		for (SvrProVec::iterator iter = theApp.m_listenSocket.m_serverVec.begin(); 
			iter != theApp.m_listenSocket.m_serverVec.end();
			iter++)
		{
			ServerProxy* pSvr = (ServerProxy*)(*iter);
			if (INVALID_SOCKET != pSvr->m_hSocket &&
				pSvr->m_user.type != _UT_NULL)
			{
				pSvr->ISendTo(pBuf, nLen);
			}
		}
	}
	else
	{
		TSessionCmdList::iterator iterCmd = m_cmdList.begin();
		ServerProxy* pSvr = (ServerProxy*)(iterCmd->lParam);
		if (pSvr!=NULL && INVALID_SOCKET != pSvr->m_hSocket)
		{
			pSvr->ISendTo(pBuf, nLen);
		}
	}
}
