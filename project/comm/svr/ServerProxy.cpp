#include "stdafx.h"
#include "ServerProxy.h"


ServerProxy::ServerProxy() 
{

}

int ServerProxy::ISendTo(BYTE* pBuf, int nLen)
{
	BYTE cmdBuf[1024];
	int nCmdLen;
	MakeCmd(pBuf, nLen, cmdBuf, nCmdLen);
	return CAsyncSocket::Send(cmdBuf, nCmdLen);
}

void ServerProxy::OnClose(int nErrorCode)
{
	CAsyncSocket::OnClose(nErrorCode);
	CProxyListenSocket* para = (CProxyListenSocket*)m_wParam;
	para->ClearInvalidSocket();
}

void ServerProxy::OnReceive(int nErrorCode)
{
	BYTE pBuf[1024];
	int nRet = Receive(pBuf, 1024, 0);
	if (nRet != SOCKET_ERROR)
	{
		CommandPool::ReceiveChar(pBuf, nRet);
	}
	else
	{
		CString errMsg;
		errMsg.Format(TEXT("Revceive Data Failed! WSAGetLastError: %d. Please close the CONNECTTION!"), WSAGetLastError());
// 		ASSERT(FALSE);
		CAsyncSocket::Close();
		return;
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

void ServerProxy::PostCmd(BYTE* pBuf, int nLen)
{
	if (_UT_NULL == m_user.type)
	{
		if ( !(CF_USER == pBuf[0] && CF_SUB_USER_CMD_LOGIN == pBuf[1]) )
		{
			// 用户不是登录命令，发送错误消息
			SendCmdParseErrMsg();
			return;
		}
	}

	switch (pBuf[0])
	{
	case CF_USER:
		TRACE("PostCmd CF_USER\n");
		switch (pBuf[1])
		{
		case CF_SUB_USER_CMD_LOGIN :
			ParseUserLogin(pBuf, nLen);
			break;
		case CF_SUB_USER_CMD_LOGOUT:
			ParseUserLogout(pBuf, nLen);
			break;
		case CF_SUB_USER_CMD_ADD:
			ParseUserAdd(pBuf, nLen);
			break;
		case CF_SUB_USER_CMD_DEL:
			ParseUserDel(pBuf, nLen);
			break;
		case CF_SUB_USER_CMD_EDIT:
			ParseUserEdit(pBuf, nLen);
			break;
		case CF_SUB_USER_GET:
			ParseUserGetUser(pBuf, nLen);
			break;
		case CF_SUB_USER_GET_COUNT:
			ParseUserGetUserCount(pBuf, nLen);
			break;
		case CF_SUB_USER_GET_USERS:
			ParseUserGetUsers(pBuf, nLen);
			break;
		default:
			SendCmdParseErrMsg();
			break;
		}
		break;
	case CF_LINK_IP:
		TRACE("PostCmd CF_LINK_IP\n");
		switch (pBuf[1])
		{
		case CF_SUB_LINK_IP_GET:
			ParseLinkIPGet(pBuf, nLen);
			break;
		case CF_SUB_LINK_IP_SET:
			ParseLinkIPSet(pBuf, nLen);
			break;
		default:
			SendCmdParseErrMsg();
			break;
		}
		break;
	case CF_ALARM_MODE:
		TRACE("PostCmd CF_ALARM_MODE\n");
		switch (pBuf[1])
		{
		case CF_SUB_ALARM_MODE_GET:
			ParseGetAlarmMode(pBuf, nLen);
			break;
		case CF_SUB_ALARM_MODE_SET:
			ParseSetAlarmMode(pBuf, nLen);
			break;
		default:
			SendCmdParseErrMsg();
			break;
		}
		break;
	case CF_LINE_INFO:
		TRACE("PostCmd CF_LINE_INFO\n");
		switch (pBuf[1])
		{
		case CF_SUB_LINE_INFO_GET:
			ParseGetLineInfo(pBuf, nLen);
			break;
		case CF_SUB_LINE_INFO_SET:
			ParseSetLineInfo(pBuf, nLen);
			break;
		default:
			SendCmdParseErrMsg();
			break;
		}
		break;
	case CF_ZONE_INFO:
		TRACE("PostCmd CF_ZONE_INFO\n");
		switch (pBuf[1])
		{
		case CF_SUB_ZONE_NAME_GET:
			ParseGetZoneName(pBuf, nLen);
			break;
		case CF_SUB_ZONE_NAME_SET:
			ParseSetZoneName(pBuf, nLen);
			break;
		default:
			SendCmdParseErrMsg();
			break;
		}
		break;
	case CF_TASK:
		TRACE("PostCmd CF_TASK ");
		switch (pBuf[1])
		{
		case CF_SUB_GROUP_GET:
			TRACE(" CF_SUB_GROUP_GET\n");
			ParseGetGroup(pBuf, nLen);
			break;
		case CF_SUB_TASK_GET:
			TRACE(" CF_SUB_TASK_GET\n");
			ParseGetGroupTask(pBuf, nLen);
			break;
		case CF_SUB_TASK_SET:
			TRACE(" CF_SUB_TASK_SET\n");
			ParseSetGroupTask(pBuf, nLen);
			break;
		case CF_SUB_GROUP_ADD:
			TRACE(" CF_SUB_GROUP_ADD\n");
			ParseGroupAdd(pBuf, nLen);
			break;
		case CF_SUB_GROUP_DEL:
			TRACE(" CF_SUB_GROUP_DEL\n");
			ParseGroupDel(pBuf, nLen);
			break;
		case CF_SUB_GROUP_EDIT_NAME:
			TRACE(" CF_SUB_GROUP_EDIT_NAME\n");
			ParseGroupEditName(pBuf, nLen);
			break;
		case CF_SUB_PROGRAM_CONFIG_GET:
			TRACE(" CF_SUB_PROGRAM_CONFIG_GET\n");
			ParseGetProgramConfig(pBuf, nLen);
			break;
		case CF_SUB_PROGRAM_CONFIG_SET:
			TRACE(" CF_SUB_PROGRAM_CONFIG_SET\n");
			ParseSetProgramConfig(pBuf, nLen);
			break;
		case CF_SUB_TASK_DOWN:
			TRACE(" CF_SUB_TASK_DOWN\n");
			ParseProgramDown(pBuf, nLen);
			break;
		case CF_SUB_REMOTE_GET:
			TRACE(" CF_SUB_REMOTE_GET\n");
			ParseGetRemoteTask(pBuf, nLen);
			break;
		case CF_SUB_REMOTE_SET:
			TRACE(" CF_SUB_REMOTE_SET\n");
			ParseSetRemoteTask(pBuf, nLen);
			break;
		default:
			SendCmdParseErrMsg();
			break;
		}
	default:
		if (!ParseSessionCmd(pBuf, nLen))
		{
			SendCmdParseErrMsg();
		}
		break;
	}
}

BOOL ServerProxy::ParseSessionCmd(BYTE* pBuf, int nLen)
{
	// 校验合格后添加到命令队列
	theSes.AddCmd(pBuf, nLen, (LPARAM)this);
	return TRUE;
}

inline int ServerProxy::SendCmdParseErrMsg()
{
	m_sendBuf[0] = CF_ERR_CMD;
	m_sendBuf[1] = _ERR_NOT_SPECIFY;
	m_sendBuf[2] = 0;
	return ISendTo(m_sendBuf, 3);
}

int ServerProxy::ParseGetAlarmMode(BYTE* pBuf, int nLen)
{
	m_sendBuf[0] = CF_ALARM_MODE;
	m_sendBuf[1] = CF_SUB_ALARM_MODE_GET;
	m_sendBuf[2] = theSes.m_sys.alarmMode;
	m_sendBuf[3] = theSes.m_sys.broadVolumeAdd;
	return ISendTo(m_sendBuf, 4);
}

int ServerProxy::ParseSetAlarmMode(BYTE* pBuf, int nLen)
{
	if (nLen != 4 || pBuf[2]>5 || pBuf[3]>15)
	{
		ASSERT(FALSE);
		return SendCmdParseErrMsg();
	}
	theSes.m_sys.alarmMode = ByteToAlarmModel(pBuf[2]);
	theSes.m_sys.broadVolumeAdd = pBuf[3];
	theDataBase.SetAlarmMode(theSes.m_sys.alarmMode, theSes.m_sys.broadVolumeAdd);
	theSes.m_bNeedSetTime = true;

	m_sendBuf[0] = CF_ALARM_MODE;
	m_sendBuf[1] = CF_SUB_ALARM_MODE_SET;
	m_sendBuf[2] = _ERR_SUCCESS;
	m_sendBuf[3] = 0;
	return ISendTo(m_sendBuf, 4);
}

int ServerProxy::ParseGetLineInfo(BYTE* pBuf, int nLen)
{
	m_sendBuf[0] = CF_LINE_INFO;
	m_sendBuf[1] = CF_SUB_LINE_INFO_GET;
	int nStructLen = sizeof(SAudioInLineConfig);
	memcpy(&m_sendBuf[2], &theSes.m_sys.m_audioInLine.config, nStructLen);
	return ISendTo(m_sendBuf, 2+nStructLen);
}

int ServerProxy::ParseSetLineInfo(BYTE* pBuf, int nLen)
{
	int nStructLen = sizeof(SAudioInLineConfig);
	if ( (2+nStructLen) != nLen)
	{
		ASSERT(0);
		return 0;
	}
	memcpy(&theSes.m_sys.m_audioInLine.config, &pBuf[2], nStructLen);

	theDataBase.WriteAudioInLine();

	ZeroMemory(m_sendBuf, 1024);
	m_sendBuf[0] = CF_LINE_INFO;
	m_sendBuf[1] = CF_SUB_LINE_INFO_SET;
	m_sendBuf[2] = _ERR_SUCCESS;
	m_sendBuf[3] = 0;
	return ISendTo(m_sendBuf, 4);
}

int ServerProxy::ParseGetZoneName(BYTE* pBuf, int nLen)
{
	for (int i=0; i<4; i++)
	{
		m_sendBuf[0] = CF_ZONE_INFO;
		m_sendBuf[1] = CF_SUB_ZONE_NAME_GET;
		int nRet = __hton_ZoneName(&m_sendBuf[2], 1000, theSes.m_sys.m_zone.it, i*16, (i+1)*16-1);
		if (nRet)
		{
			ISendTo(m_sendBuf, 2+nRet);
		}
	}
	return 1;
}

int ServerProxy::ParseSetZoneName(BYTE* pBuf, int nLen)
{
	if (__ParseZoneName(pBuf, nLen, theSes.m_sys.m_zone) > 0)
	{
		theDataBase.WriteZone();
	}

	ZeroMemory(m_sendBuf, 1024);
	m_sendBuf[0] = CF_ZONE_INFO;
	m_sendBuf[1] = CF_SUB_ZONE_NAME_SET;
	m_sendBuf[2] = _ERR_SUCCESS;
	m_sendBuf[3] = 0;
	return ISendTo(m_sendBuf, 4);
}


inline int ServerProxy::ParseUserLogin(BYTE* pBuf, int nLen)
{
	S_DB_USER user;
	int nPos = 2;
	nPos += ParseByteToUser(user, &pBuf[nPos]);
	if (nPos != nLen)
	{
		ASSERT(FALSE);
		return SendCmdParseErrMsg();
	}

	m_sendBuf[0] = CF_USER;
	m_sendBuf[1] = CF_SUB_USER_CMD_LOGIN;
	nPos = 2;
	_DbUserVec::iterator iter;
	if (theDataBase.m_user.FindUser(user.name, iter))
	{
		if (strcmp(user.password, iter->password) == 0)
		{
			m_user = *iter;
			m_sendBuf[nPos++] = _ERR_SUCCESS;
			nPos +=	UserToByte(m_user, &m_sendBuf[nPos], true);
			return ISendTo(m_sendBuf, nPos);
		}
		else
		{
			m_sendBuf[nPos++] = _ERR_USER_PASSWORD;
		}
	}
	else
	{
		// 返回用户名错误
		m_sendBuf[nPos++] = _ERR_USER_NAME;
	}
	return ISendTo(m_sendBuf, nPos);
}

inline int ServerProxy::ParseUserLogout(BYTE* pBuf, int nLen)
{
	S_DB_USER user;
	int nPos = 2;
	nPos += ParseByteToUser(user, &pBuf[nPos]);
	if (nPos != nLen)
	{
		ASSERT(FALSE);
		return SendCmdParseErrMsg();
	}

	m_sendBuf[0] = CF_USER;
	m_sendBuf[1] = CF_SUB_USER_CMD_LOGOUT;
	m_sendBuf[2] = _ERR_SUCCESS;

	m_user.LoadDefault();
	return ISendTo(m_sendBuf, 3);
}

inline int ServerProxy::ParseUserAdd(BYTE* pBuf, int nLen)
{
	S_DB_USER user;
	int nPos = 2;
	nPos += ParseByteToUser(user, &pBuf[nPos]);
	if (nPos != nLen)
	{
		ASSERT(FALSE);
		return SendCmdParseErrMsg();
	}

	m_sendBuf[0] = CF_USER;
	m_sendBuf[1] = CF_SUB_USER_CMD_ADD;
	m_sendBuf[2] = _ERR_DB_OPERATE;
	if (theDataBase.m_user.m_userVec.size() < _MAX_USER_ACCOUNTS)
	{
		if (theDataBase.m_user.AddUser(user))
		{
			m_sendBuf[2] = _ERR_SUCCESS;
		}
	}
	if (m_sendBuf[2] != _ERR_DB_OPERATE)
	{
		theDataBase.WriteUsers();
	}
	return ISendTo(m_sendBuf, 3);
}

inline int ServerProxy::ParseUserDel(BYTE* pBuf, int nLen)
{
	S_DB_USER user;
	int nPos = 2;
	nPos += ParseByteToUser(user, &pBuf[nPos]);
	if (nPos != nLen)
	{
		ASSERT(FALSE);
		return SendCmdParseErrMsg();
	}

	m_sendBuf[0] = CF_USER;
	m_sendBuf[1] = CF_SUB_USER_CMD_DEL;
	if (theDataBase.m_user.DelUser((char*)&pBuf[2]))
	{
		m_sendBuf[2] = _ERR_SUCCESS;
	}
	else
	{
		m_sendBuf[2] = _ERR_DB_OPERATE;
	}
	theDataBase.WriteUsers();
	return ISendTo(m_sendBuf, 3);
}

inline int ServerProxy::ParseUserGetUser(BYTE* pBuf, int nLen)
{
	m_sendBuf[0] = CF_USER;
	m_sendBuf[1] = CF_SUB_USER_GET;
	m_sendBuf[2] = (char)theDataBase.m_user.m_userVec.size();
	if (m_sendBuf[2] > _MAX_USER_ACCOUNTS)
	{
		m_sendBuf[2] = _MAX_USER_ACCOUNTS;
	}
	int nPos = 3;
	for (_DbUserVec::iterator iter = theDataBase.m_user.m_userVec.begin();
		iter != theDataBase.m_user.m_userVec.end();
		iter++)
	{
		nPos += UserToByte(*iter, &m_sendBuf[nPos], false);
	}
	return ISendTo(m_sendBuf, nPos);
}

inline int ServerProxy::ParseUserGetUserCount(BYTE* pBuf, int nLen)
{
	m_sendBuf[0] = CF_USER;
	m_sendBuf[1] = CF_SUB_USER_GET_COUNT;
	m_sendBuf[2] = (char)theDataBase.m_user.m_userVec.size();
	return ISendTo(m_sendBuf, 3);
}

inline int ServerProxy::ParseUserGetUsers(BYTE* pBuf, int nLen)
{
	if (nLen != 4)
	{
		return SendCmdParseErrMsg();
	}

	ASSERT(pBuf[3] < pBuf[4]);
	if (pBuf[3] > pBuf[4])
	{
		return SendCmdParseErrMsg();
	}

	for (_DbUserVec::size_type ix = pBuf[3];
		ix<pBuf[4] && ix<theDataBase.m_user.m_userVec.size();
		ix++)
	{
		int nPos = 0;
		m_sendBuf[nPos++] = CF_USER;
		m_sendBuf[nPos++] = CF_SUB_USER_GET_USERS;
		memcpy_s(&m_sendBuf[nPos], _MAX_NAME_LEN+1, theDataBase.m_user.m_userVec[ix].name, _MAX_NAME_LEN+1);
		nPos += _MAX_NAME_LEN+1;
		memcpy_s(&m_sendBuf[nPos], _MAX_PASSWORD_LEN+1, theDataBase.m_user.m_userVec[ix].password, _MAX_PASSWORD_LEN+1);
		nPos += _MAX_PASSWORD_LEN+1;
		m_sendBuf[nPos++] = theDataBase.m_user.m_userVec[ix].type;
		ISendTo(m_sendBuf, nPos);
		//Sleep(10);
	}
	return pBuf[4] - pBuf[3];
}

inline int ServerProxy::ParseLinkIPGet(BYTE* pBuf, int nLen)
{
	int nPos = 0;
	m_sendBuf[nPos++] = CF_LINK_IP;
	m_sendBuf[nPos++] = CF_SUB_LINK_IP_GET;
	m_sendBuf[nPos++] = theSes.m_saTarget.sin_addr.S_un.S_un_b.s_b1;
	m_sendBuf[nPos++] = theSes.m_saTarget.sin_addr.S_un.S_un_b.s_b2;
	m_sendBuf[nPos++] = theSes.m_saTarget.sin_addr.S_un.S_un_b.s_b3;
	m_sendBuf[nPos++] = theSes.m_saTarget.sin_addr.S_un.S_un_b.s_b4;
	m_sendBuf[nPos++] = (char)theSes.m_saTarget.sin_port;
	m_sendBuf[nPos++] = (char)(theSes.m_saTarget.sin_port>>8);
	return ISendTo(m_sendBuf, nPos);
}

inline int ServerProxy::ParseLinkIPSet(BYTE* pBuf, int nLen)
{
	if (8!=nLen)
	{
		return SendCmdParseErrMsg();
	}

	IN_ADDR sin_addr;
	USHORT port;
	int nPos = 2;
	sin_addr.S_un.S_un_b.s_b1 = pBuf[nPos++];
	sin_addr.S_un.S_un_b.s_b2 = pBuf[nPos++];
	sin_addr.S_un.S_un_b.s_b3 = pBuf[nPos++];
	sin_addr.S_un.S_un_b.s_b4 = pBuf[nPos++];
// 	port = pBuf[nPos+1];
// 	port = (port<<8) | pBuf[nPos];
	port = theSes.m_saTarget.sin_port; // 端口设置不开放
	theSes.SetTargetSockAddr(sin_addr.S_un.S_addr, port);
	// 保存
	theDataBase.SetNetParameter(sin_addr.S_un.S_addr, port);
	theSes.m_bNeedSetTime = true;

	nPos = 0;
	m_sendBuf[nPos++] = CF_LINK_IP;
	m_sendBuf[nPos++] = CF_SUB_LINK_IP_SET;
	m_sendBuf[nPos++] = _ERR_SUCCESS;
	return ISendTo(m_sendBuf, nPos);
}

//////////////////////////////////////////////////////////////////////////

// 获取项目(不包含详细任务)
int ServerProxy::ParseGetGroup(BYTE*pBuf, int nLen)
{
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_GROUP_GET;
	m_sendBuf[nPos++] = (BYTE)theTaskDB.m_groupVec.size();
	for (_DbTaskGroupVec::const_iterator iter = theTaskDB.m_groupVec.begin();
		iter != theTaskDB.m_groupVec.end();
		iter++)
	{
		nPos += StringToBuf((char*)iter->name, &m_sendBuf[nPos]);
	}
	return ISendTo(m_sendBuf, nPos);
}

// 添加项目
int ServerProxy::ParseGroupAdd(BYTE*pBuf, int nLen)
{
	if (strlen((char*)&pBuf[3]) <= 0)
	{
		return SendCmdParseErrMsg();
	} 

	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_GROUP_ADD;

	if (FindGroup(theTaskDB.m_groupVec, (char*)&pBuf[3]) != theTaskDB.m_groupVec.end())
	{
		m_sendBuf[nPos++] = _ERR_DB_OPERATE;
		return ISendTo(m_sendBuf, nPos);
	}

	S_DB_TASK_GROUP group;
	group.SetName((char*)&pBuf[3]);

	if (pBuf[2] == 0x01) // copy from ...
	{
		int nLen = strlen((char*)&pBuf[3]);
		_DbTaskGroupVec::iterator cpy_iter = FindGroup(theTaskDB.m_groupVec, (char*)&pBuf[3+nLen+1]);
		if (cpy_iter == theTaskDB.m_groupVec.end())
		{
			// no serach the copy group
			m_sendBuf[nPos++] = _ERR_DB_OPERATE;
			return ISendTo(m_sendBuf, nPos);
		}
		group.m_taskVec = cpy_iter->m_taskVec;
	}

	theTaskDB.m_groupVec.push_back(group);
	theTaskDB.WriteTasks();

 	m_sendBuf[nPos++] = _ERR_SUCCESS;
	return ISendTo(m_sendBuf, nPos);
}

// 删除某项目
int ServerProxy::ParseGroupDel(BYTE*pBuf, int nLen)
{
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_GROUP_DEL;

	_DbTaskGroupVec::iterator group_find_iter = FindGroup(theTaskDB.m_groupVec, (char*)&pBuf[3]);
	if (group_find_iter != theTaskDB.m_groupVec.end())
	{
		theTaskDB.m_groupVec.erase(group_find_iter);
		theTaskDB.WriteTasks();

		m_sendBuf[nPos++] = _ERR_SUCCESS;
		return ISendTo(m_sendBuf, nPos);
	}

	m_sendBuf[nPos++] = _ERR_DB_OPERATE;
	return ISendTo(m_sendBuf, nPos);
}

// 修改某项目名称
int ServerProxy::ParseGroupEditName(BYTE*pBuf, int nLen)
{
	if (strlen((char*)&pBuf[3]) <= 0)
	{
		return SendCmdParseErrMsg();
	}

	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_GROUP_EDIT_NAME;

	_DbTaskGroupVec::iterator group_find_iter = FindGroup(theTaskDB.m_groupVec, (char*)&pBuf[3]);
	if (group_find_iter != theTaskDB.m_groupVec.end())
	{
		char* pName1 = (char*)&pBuf[3];
		int nLen1 = strlen(pName1);
		char* pName2 = (char*)&pBuf[3+nLen1+1];
		int nLen2 = strlen(pName2);
		if (nLen2 <=0 || nLen2 > _MAX_NAME_LEN || (3+nLen1+1+nLen2+1)>nLen)
		{
			return SendCmdParseErrMsg();
		}
		if (FindGroup(theTaskDB.m_groupVec, pName2) 
			== theTaskDB.m_groupVec.end())
		{
			group_find_iter->SetName((char*)&pBuf[3+nLen1+1]);
			theTaskDB.WriteTasks();
			m_sendBuf[nPos++] = _ERR_SUCCESS;
			return ISendTo(m_sendBuf, nPos);
		}
	}

	return SendCmdParseErrMsg();
}

int ServerProxy::RetGroupTask(_DbTaskGroupVec::iterator& iter)
{
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_TASK_GET;
	m_sendBuf[nPos++] = 0;

	nPos += StringToBuf(iter->name, (BYTE*)&m_sendBuf[nPos]);

	m_sendBuf[nPos++] = iter->m_taskVec.size(); // 总数
	int nBeginPos = nPos;
	m_sendBuf[nPos++] = 1; // 起始任务位置
	int nEndPos = nPos;
	m_sendBuf[nPos++] = 0; // 结束任务位置

	int nContentStartPos = nPos;

	if (iter->m_taskVec.size() == 0)
	{
		ISendTo(m_sendBuf, nPos);
	}

	for (_DbTaskVec::size_type ix =0;
		ix != iter->m_taskVec.size();
		ix++)
	{
		nPos += __TaskToBytes(&iter->m_taskVec[ix], &m_sendBuf[nPos]);
		m_sendBuf[nEndPos]++;

		if ((ix+1) == iter->m_taskVec.size())
		{
			m_sendBuf[nPos++] = 0xEF;
			TRACE("ParseGetGroupTask end\n");
			return ISendTo(m_sendBuf, nPos);
		}

		if (nPos > 800)
		{
			ISendTo(m_sendBuf, nPos);
			m_sendBuf[nBeginPos] = m_sendBuf[nEndPos]+1;
			nPos = nContentStartPos;
			Sleep(10);
		}
	}
	return nPos;
}

// 获取某项目下任务
int ServerProxy::ParseGetGroupTask(BYTE*pBuf, int nLen)
{
	_DbTaskGroupVec::iterator group_find_iter = FindGroup(theTaskDB.m_groupVec, (char*)&pBuf[3]);
	if (group_find_iter == theTaskDB.m_groupVec.end())
	{
		return SendCmdParseErrMsg();
	}

	TRACE("ParseGetGroupTask start: ");
	TRACE(group_find_iter->name);
	TRACE("\n");
	return RetGroupTask(group_find_iter);

// 	int nPos = 0;
// 	m_sendBuf[nPos++] = CF_TASK;
// 	m_sendBuf[nPos++] = CF_SUB_TASK_GET;
// 	m_sendBuf[nPos++] = 0;
// 
// 	nPos += StringToBuf((char*)&pBuf[3], (BYTE*)&m_sendBuf[nPos]);
// 
// 	m_sendBuf[nPos++] = group_find_iter->m_taskVec.size(); // 总数
// 	int nBeginPos = nPos;
// 	m_sendBuf[nPos++] = 1; // 起始任务位置
// 	int nEndPos = nPos;
// 	m_sendBuf[nPos++] = 0; // 结束任务位置
// 
// 	int nContentStartPos = nPos;
// 
// 	if (group_find_iter->m_taskVec.size() == 0)
// 	{
// 		ISendTo(m_sendBuf, nPos);
// 	}
// 
// 	for (_DbTaskVec::size_type ix =0;
// 		ix != group_find_iter->m_taskVec.size();
// 		ix++)
// 	{
// 		nPos += __TaskToBytes(&group_find_iter->m_taskVec[ix], &m_sendBuf[nPos]);
// 		m_sendBuf[nEndPos]++;
// 
// 		if ((ix+1) == group_find_iter->m_taskVec.size())
// 		{
// 			m_sendBuf[nPos++] = 0xEF;
// 			TRACE("ParseGetGroupTask end\n");
// 			return ISendTo(m_sendBuf, nPos);
// 		}
// 
// 		if (nPos > 900)
// 		{
// 			ISendTo(m_sendBuf, nPos);
// 			m_sendBuf[nBeginPos] = m_sendBuf[nEndPos];
// 			nPos = nContentStartPos;
// 		}
// 	}
// 	TRACE("ParseGetGroupTask end\n");
// 	return nPos;
}

// 修改某项目任务
int ServerProxy::ParseSetGroupTask(BYTE*pBuf, int nLen)
{
	_DbTaskGroupVec::iterator group_find_iter = FindGroup(theTaskDB.m_groupVec, (char*)&pBuf[3]);
	if (group_find_iter == theTaskDB.m_groupVec.end())
	{
		return SendCmdParseErrMsg();
	}
	int nPos = 3 + strlen((char*)&pBuf[3])+1;

	if (pBuf[2] == TST_NEW)
	{
		S_DB_TASK task;
		nPos += __BytesToTask(&task, &pBuf[nPos]);
		if (nPos > nLen || !task.IsInvalid())
		{
			return SendCmdParseErrMsg();
		}
		if (FindTask(group_find_iter->m_taskVec, task.nTimeSec) != group_find_iter->m_taskVec.end())
		{
			return SendCmdParseErrMsg();
		}
		group_find_iter->m_taskVec.push_back(task);
		//
		group_find_iter->SortTask();
	} 
	else if (pBuf[2] == TST_DEL)
	{
		UINT nInt = Byte2Int(&pBuf[nPos]);
		nPos += 4;
		_DbTaskVec::iterator task_find_iter = FindTask(group_find_iter->m_taskVec, nInt);
		if (nPos > nLen || task_find_iter == group_find_iter->m_taskVec.end())
		{
			return SendCmdParseErrMsg();
		}
		group_find_iter->m_taskVec.erase(task_find_iter);
	}
	else if (pBuf[2] == TST_DEL_ALL)
	{
		group_find_iter->m_taskVec.clear();
	}
	else if (pBuf[2] == TST_EDIT)
	{
		UINT nInt = Byte2Int(&pBuf[nPos]);
		nPos += 4;
		S_DB_TASK task;
		nPos += __BytesToTask(&task, &pBuf[nPos]);
		_DbTaskVec::iterator task_find_iter = FindTask(group_find_iter->m_taskVec, nInt);
		if (nPos > nLen || 
			!task.IsInvalid() ||
			task_find_iter == group_find_iter->m_taskVec.end())
		{
			return SendCmdParseErrMsg();
		}
		*task_find_iter = task;
		//
		group_find_iter->SortTask();
	}
	else
	{
		return SendCmdParseErrMsg();
	}

	theTaskDB.WriteTasks();

	nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
	m_sendBuf[nPos++] = pBuf[2];
	m_sendBuf[nPos++] = _ERR_SUCCESS;
	return ISendTo(m_sendBuf, nPos);
}

int ServerProxy::ParseGetProgramConfig(BYTE*pBuf, int nLen) // 获取运行程序配置信息
{
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_PROGRAM_CONFIG_GET;
	m_sendBuf[nPos++] = 	theTaskDB.m_bProgConfig ? 1 : 0;
	for (int i=0; i<7; i++)
	{
		if (theTaskDB.m_szProgConfig[i] != NULL && strlen(theTaskDB.m_szProgConfig[i]) > 0)
		{
			if (FindGroup(theTaskDB.m_groupVec, theTaskDB.m_szProgConfig[i]) == theTaskDB.m_groupVec.end())
			{
				theTaskDB.m_szProgConfig[i][0] = NULL;
			}
		}

		nPos += StringToBuf((char*)&theTaskDB.m_szProgConfig[i][0], &m_sendBuf[nPos]);
	}
	return ISendTo(m_sendBuf, nPos);
}


int ServerProxy::ParseSetProgramConfig(BYTE*pBuf, int nLen) // 设置运行程序配置信息
{
	char szProgConfig[7][_MAX_NAME_LEN+1];
	int nPos = 3;
	theTaskDB.m_bProgConfig = pBuf[2] ? true : false;
	for (int i=0; i<7; i++)
	{
		int nNameLen = strlen((char*)&pBuf[nPos]);
		memcpy_s(szProgConfig[i], nNameLen+1, &pBuf[nPos], nNameLen+1);
		nPos += (nNameLen+1);
		if (nPos > nLen)
		{
			return SendCmdParseErrMsg();
		}
		if (strlen(szProgConfig[i]) > 0)
		{
			_DbTaskGroupVec::iterator group_find_iter = FindGroup(theTaskDB.m_groupVec, szProgConfig[i]);
			if (group_find_iter != theTaskDB.m_groupVec.end())
			{
				//RetGroupTask(group_find_iter);
			}
			else
			{
				return SendCmdParseErrMsg();
			}
	
		}
	}
	//
	for (int i=0; i<7; i++)
	{
		memcpy_s(theTaskDB.m_szProgConfig[i], _MAX_NAME_LEN+1, szProgConfig[i], _MAX_NAME_LEN+1);
	}
	theTaskDB.WriteConfig();
	m_sendBuf[0] = CF_TASK;
	m_sendBuf[1] = CF_SUB_PROGRAM_CONFIG_SET;
	m_sendBuf[2] = pBuf[2];
	m_sendBuf[3] = _ERR_SUCCESS;
	ISendTo(m_sendBuf, 4);
	return 1;
}


int ServerProxy::ParseProgramDown(BYTE*pBuf, int nLen)
{
	int iDayOfWeek = pBuf[2];
	if (iDayOfWeek >= 7)
	{
		return SendCmdParseErrMsg();
	}

	_DbTaskVec taskVec;

	if (strlen(theTaskDB.m_szProgConfig[iDayOfWeek]) > 0)
	{
		_DbTaskGroupVec::iterator group_find_iter = FindGroup(theTaskDB.m_groupVec, theTaskDB.m_szProgConfig[iDayOfWeek]);
		if (group_find_iter != theTaskDB.m_groupVec.end())
		{
			taskVec = group_find_iter->m_taskVec;
		}
	}
	SimulateSession ss;
	int iSendPages = ss._DownloadProgram(iDayOfWeek, taskVec); // SimulateSession没导入参数，计算发送次数
	if (iSendPages > 0)
	{
		m_sendBuf[0] = CF_TASK;
		m_sendBuf[1] = CF_SUB_TASK_DOWN;
		m_sendBuf[2] = iSendPages;
		m_sendBuf[3] = pBuf[2];
		ISendTo(m_sendBuf, 4);

		ss.SetParam((LPARAM)this);
		ss._DownloadProgram(iDayOfWeek, taskVec);
	}
	else
	{
		return SendCmdParseErrMsg();
	}
	//
	return 1;
}



// 遥控器
int ServerProxy::ParseGetRemoteTask(BYTE*pBuf, int nLen)
{
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_REMOTE_GET;
	m_sendBuf[nPos++] = _MAX_REMOTE_FUNC;

	nPos += RemoteTaskToByte(theTaskDB.m_remoteVec, &m_sendBuf[nPos]);

	return ISendTo(m_sendBuf, nPos);
}


int ServerProxy::ParseSetRemoteTask(BYTE*pBuf, int nLen)
{
	_DbTaskGroupVec remoteVec;
	int nPos = 3;

	if (ByteToRemoteTask(remoteVec, &pBuf[nPos], nLen-3) != _MAX_REMOTE_FUNC)
	{
		return SendCmdParseErrMsg();
	}

	theTaskDB.m_remoteVec = remoteVec;
	theTaskDB.WriteRemote();

	nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_REMOTE_SET;
	m_sendBuf[nPos++] = pBuf[2];
	m_sendBuf[nPos++] = _ERR_SUCCESS;
	return ISendTo(m_sendBuf, nPos);
}

int ServerProxy::ParseUserEdit( BYTE* pBuf, int nLen )
{
	S_DB_USER user;
	int nPos = 2;
	nPos += ParseByteToUser(user, &pBuf[nPos]);
	if (nPos != nLen)
	{
// 		ASSERT(FALSE);
		return SendCmdParseErrMsg();
	}

	m_sendBuf[0] = CF_USER;
	m_sendBuf[1] = CF_SUB_USER_CMD_EDIT;
	m_sendBuf[2] = _ERR_DB_OPERATE;

	_DbUserVec::iterator iter;
	if (theDataBase.m_user.FindUser(user.name, iter))
	{
		if (strlen(user.password) == 0)
		{
			user.SetPassword(iter->password);
		}
		*iter = user;
		m_sendBuf[2] = _ERR_SUCCESS;
	}
	else
	{
		return SendCmdParseErrMsg();
	}

	if (m_sendBuf[2] != _ERR_DB_OPERATE)
	{
		theDataBase.WriteUsers();
	}
	return ISendTo(m_sendBuf, 3);
}