#include "StdAfx.h"
#include "ClientSession.h"

#define MY_INLINE 

MY_INLINE ClientSession::~ClientSession()
{
}

void ClientSession::NotifyMsg(UINT msg, WPARAM wParam/*=NULL*/, LPARAM lParam/*=NULL*/)
{
	for (std::vector<HWND>::iterator iter = m_pNotifyWndVec.begin(); iter != m_pNotifyWndVec.end(); )
	{
		if (::IsWindow(*iter))
		{
			::SendMessage(*iter, msg, wParam, lParam);
			iter++;
		}
		else
		{
			iter = m_pNotifyWndVec.erase(iter);
		}
	}
}

MY_INLINE int ClientSession::ISendTo(BYTE* pBuf, int nLen)
{
	int nCmdLen;
	BYTE pCmdBuf[1024];
	MakeCmd(pBuf, nLen, pCmdBuf, nCmdLen);
	return CAsyncSocket::Send(pCmdBuf, nCmdLen);
}

// MY_INLINE int ClientSession::Receive(void* lpBuf, int nBufLen, int nFlags /* = 0 */)
// {
// 	if (nBufLen > 0 )
// 	{
// 		//TRACE1("CommandPool::ReceiveChar - %d \n", nBufLen);
// 		CommandPool::ReceiveChar((BYTE*)lpBuf, nBufLen);
// 	}
// }

void ClientSession::PostCmd(BYTE* pBuf, int nLen)
{
	m_cmdCriticalSection.Lock();
	m_iRecvLen = nLen;
	memcpy(m_recvBuf, pBuf, m_iRecvLen);

	static BYTE szHead[] = {'i','t','c'/*,'b','r','o','a','d'*/};
	if (memcmp(pBuf, szHead, 3) == 0)
	{
		// 解析设备信息
		ISession::IReceive(pBuf, nLen);
		//TRACE("ISession::IReceive\n");
		if (!m_bDevOnline)
		{
			m_bDevOnline = TRUE;
			NotifyMsg(WM_USER_START + CF_ERR_CONNECT, 0, 0);
		}

		// 有改变的话，通知更新

		int nPos = 8;
		// 1.命令解析
		if ('B' == pBuf[3])
		{
			//设备状态处理
			if (nLen != 396)
			{
				//return -1;
			}
			// MP3
			// Radio
			// CD
			// 6823 火警采集器
			// 6807 信号矩阵
			// 6812 呼叫站
			// 6830 火警播放器
			// 6816 电源
			// 6811 强切电源控制器
			// 6825你 电话呼叫控制器
			
			NotifyMsg(WM_USER_START + CF_GET_HOST_STATE);
		}
		else
		{
			// TB6800应答
			switch (pBuf[nPos])
			{
			case CF_PROGRAM_DOWN_ENTRY:
				break;
			case CF_PROGRAM_DOWN:
				break;
			case CF_PROGRAM_END:
				break;
			case CF_GET_IP_ADDR: // 返回IP和物理地址
			case CF_SET_IP_ADDR:
				break;
			case CF_SERIAL_NUM_RET: // 返回序列号
				break;
			case CF_SET_REMOTE_CONTROL: // 设置遥控数据
				break;
			}

			NotifyMsg(WM_USER_START + pBuf[nPos]);
		}
	}
	else
	{
		switch (pBuf[0])
		{
		case CF_ERR_CMD:
			AfxMessageBox(_T("命令错误！"));
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		case CF_ERR_CONNECT:
			m_bDevOnline = FALSE;
			//AfxMessageBox(_T("设备连接中断，请确定设备工作是否正常"));
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		case CF_GET_IP_ADDR:
		case CF_SET_IP_ADDR:
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		case CF_GET_SERIAL_NUM:
		case CF_SET_SERIAL_NUM:
		case CF_SET_REGISTER_NUM:
			NotifyMsg(WM_USER_START + CF_SERIAL_NUM_RET, pBuf[1], pBuf[2]);
			break;
		case CF_USER:
			switch (pBuf[1])
			{
			case CF_SUB_USER_CMD_LOGIN:
				if (_ERR_SUCCESS == pBuf[2])
				{
					m_bLogin = true;
					int nPos = 3;
					S_DB_USER user;
					nPos += ParseByteToUser(user, &pBuf[nPos]);
					if (nPos != nLen)
					{
						ASSERT(FALSE);
					}
					if (strcmp(m_logon_user.name, user.name) != 0)
					{
						ASSERT(FALSE);
					}
					m_logon_user = user;
				}
				break;
			case CF_SUB_USER_CMD_LOGOUT:
				if (_ERR_SUCCESS == pBuf[2])
				{
					m_bLogin = false;
				}
				break;
			case CF_SUB_USER_GET:
				ParseUsers(pBuf, nLen);
				break;
			}
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		case CF_ALARM_MODE:
			switch (pBuf[1])
			{
			case CF_SUB_ALARM_MODE_GET:
				ParseAlarmMode(pBuf, nLen);
				break;
			case CF_SUB_ALARM_MODE_SET:
				break;
			}
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		case CF_LINE_INFO:
			switch (pBuf[1])
			{
			case CF_SUB_LINE_INFO_GET:
				ParseLineInfo(pBuf, nLen);
				break;
			case CF_SUB_LINE_INFO_SET:
				break;
			}
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		case CF_ZONE_INFO:
			switch (pBuf[1])
			{
			case CF_SUB_ZONE_NAME_GET:
				ParseZoneName(pBuf, nLen);
				break;
			case CF_SUB_ZONE_NAME_SET:
				break;
			}
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		case CF_LINK_IP:
			switch (pBuf[1])
			{
			case CF_SUB_LINK_IP_GET:
				ParseLinkIP(pBuf, nLen);
				break;
			case CF_SUB_LINK_IP_SET:
				break;
			}
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		case CF_TASK:
			switch (pBuf[1])
			{
			case CF_SUB_GROUP_GET:
				ParseGroup(pBuf, nLen);
				break;
			case CF_SUB_PROGRAM_CONFIG_GET:
				PraseProgramConfig(pBuf, nLen);
				break;
			case CF_SUB_TASK_GET:
				ParseGroupTask(pBuf, nLen);
				break;
			case CF_SUB_REMOTE_GET:
				ParseRemoteTask(pBuf, nLen);
				break;
			case CF_SUB_REMOTE_SET:
				break;
			case CF_SUB_TASK_SET:
				break;
			case CF_SUB_GROUP_ADD:
				break;
			case CF_SUB_GROUP_DEL:
				break;
			case CF_SUB_GROUP_EDIT_NAME:
				break;
			}
			NotifyMsg(WM_USER_START + pBuf[0], pBuf[1], pBuf[2]);
			break;
		default:
			break;
		}

// 		if (__GetCmdRetByte(m_sendBuf[0]) == m_recvBuf[0])
// 		{
// 			// 命令完成
// 			TRACE1("Complate Command: %x\n", m_sendBuf[0]);
// 		}
	}
	m_cmdCriticalSection.Unlock();
}

MY_INLINE int ClientSession::UserCmd(int cmdType /*= CF_USER_SUB_CMD_LOGIN*/, S_DB_USER& user)
{
	ZeroMemory(m_sendBuf, 64);
	m_sendBuf[0] = CF_USER;
	switch (cmdType)
	{
	case CF_SUB_USER_CMD_LOGIN:
	case CF_SUB_USER_CMD_LOGOUT:
	case CF_SUB_USER_CMD_ADD:
	case CF_SUB_USER_CMD_DEL:
	case CF_SUB_USER_CMD_EDIT:
		m_sendBuf[1] = cmdType;
		break;
	default:
		ASSERT(FALSE);
		return -1;
		break;
	}
	if (CF_SUB_USER_CMD_LOGIN == cmdType)
	{
		if (m_bLogin)
		{
			return false;
		}
		m_logon_user = user;
	}

	int nPos = 2;
	nPos +=	UserToByte(user, &m_sendBuf[2], true);
	ISendTo(m_sendBuf, nPos);
	return 1;
}

int ClientSession::GetUsers()
{
	ZeroMemory(m_sendBuf, 64);
	m_sendBuf[0] = CF_USER;
	m_sendBuf[1] = CF_SUB_USER_GET;
	m_sendBuf[2] = 0;
	return ISendTo(m_sendBuf, 3);
}

int ClientSession::ParseUsers(BYTE* pBuf, int nLen)
{
	//
	m_user.m_userVec.clear();
	//
	int nCount = pBuf[2];
	int nPos = 3;
	for (int i=0; i<nCount; i++)
	{
		S_DB_USER user;
		nPos += ParseByteToUser(user, &pBuf[nPos], false);
		if (nPos > nLen)
		{
			ASSERT(FALSE);
		}
		else
		{
			m_user.m_userVec.push_back(user);
		}
	}
	return nPos;
}


int ClientSession::GetLinkIP()
{
	int nPos = 0;
	m_sendBuf[nPos++] = CF_LINK_IP;
	m_sendBuf[nPos++] = CF_SUB_LINK_IP_GET;
	m_sendBuf[nPos++] = 0;
	return ISendTo(m_sendBuf, 3);
}

int ClientSession::SetLinkIP(ULONG net_addr)
{
	int nPos = 0;
	m_sendBuf[nPos++] = CF_LINK_IP;
	m_sendBuf[nPos++] = CF_SUB_LINK_IP_SET;
	IN_ADDR sinAddr;
	sinAddr.S_un.S_addr = net_addr;
	m_sendBuf[nPos++] = sinAddr.S_un.S_un_b.s_b1;
	m_sendBuf[nPos++] = sinAddr.S_un.S_un_b.s_b2;
	m_sendBuf[nPos++] = sinAddr.S_un.S_un_b.s_b3;
	m_sendBuf[nPos++] = sinAddr.S_un.S_un_b.s_b4;
	m_sendBuf[nPos++] = 0; // 端口设置暂不开放
	m_sendBuf[nPos++] = 0;
	return ISendTo(m_sendBuf, nPos);
}

int ClientSession::ParseLinkIP(BYTE* pBuf, int nLen)
{
	if (8!=nLen)
	{
		return 0;
	}
	IN_ADDR sin_addr;
	USHORT port;
	int nPos = 2;
	sin_addr.S_un.S_un_b.s_b1 = pBuf[nPos++];
	sin_addr.S_un.S_un_b.s_b2 = pBuf[nPos++];
	sin_addr.S_un.S_un_b.s_b3 = pBuf[nPos++];
	sin_addr.S_un.S_un_b.s_b4 = pBuf[nPos++];
 	port = pBuf[nPos+1];
 	port = (port << 8) | pBuf[nPos];
	m_saLinkSet.sin_port = port;
	m_saLinkSet.sin_addr.S_un.S_addr = sin_addr.S_un.S_addr;
	return 1;
}

int ClientSession::GetAlarmMode()
{
	ZeroMemory(m_sendBuf, 24);
	m_sendBuf[0] = CF_ALARM_MODE;
	m_sendBuf[1] = CF_SUB_ALARM_MODE_GET;
	m_sendBuf[2] = 0x00;
	return ISendTo(m_sendBuf, 3);
}

int ClientSession::SetAlarmMode(E_ALARM_MODEL alarmMode/*=AM_CURRENT_ZONE*/, BYTE broadVolumeAdd/*=0*/)
{
	ZeroMemory(m_sendBuf, 24);
	m_sendBuf[0] = CF_ALARM_MODE;
	m_sendBuf[1] = CF_SUB_ALARM_MODE_SET;
	m_sendBuf[2] = alarmMode;
	m_sendBuf[3] = broadVolumeAdd;
	return ISendTo(m_sendBuf, 4);
}

int ClientSession::GetLineInfo()
{
	ZeroMemory(m_sendBuf, 24);
	m_sendBuf[0] = CF_LINE_INFO;
	m_sendBuf[1] = CF_SUB_LINE_INFO_GET;
	m_sendBuf[2] = 0x00;
	return ISendTo(m_sendBuf, 3);
}

int ClientSession::SetLineInfo(SAudioInLineConfig* pConfig)
{
	ZeroMemory(m_sendBuf, 1024);
	m_sendBuf[0] = CF_LINE_INFO;
	m_sendBuf[1] = CF_SUB_LINE_INFO_SET;
	int nLen = sizeof(SAudioInLineConfig);
	memcpy(&m_sendBuf[2], pConfig, nLen);
	return ISendTo(m_sendBuf, 2+nLen);
}

int ClientSession::GetZoneName()
{
	ZeroMemory(m_sendBuf, 24);
	m_sendBuf[0] = CF_ZONE_INFO;
	m_sendBuf[1] = CF_SUB_ZONE_NAME_GET;
	m_sendBuf[2] = 0x00;
	return ISendTo(m_sendBuf, 3);
}

int ClientSession::SetZoneName(int nZone, CString name)
{
	if (nZone<0 || nZone>=_MAX_ZONEA)
	{
		ASSERT(FALSE);
		return 0;
	}
	SZoneUnitA* pIt = m_sys.m_zone.it;
	StringToBuf(name, (BYTE*)pIt[nZone].name);

	m_sendBuf[0] = CF_ZONE_INFO;
	m_sendBuf[1] = CF_SUB_ZONE_NAME_SET;
	int nRet = __hton_ZoneName(&m_sendBuf[2], _MAX_NAME_LEN+16, pIt, nZone, nZone);
	if (nRet)
	{
		return ISendTo(m_sendBuf, 2+nRet);
	}
	return 0;
}


int ClientSession::SetZoneName(CString name[])
{
	SZoneUnitA* pIt = m_sys.m_zone.it;
	for (int i=0; i<_MAX_ZONEA; i++)
	{
		StringToBuf(name[i++], (BYTE*)pIt[i].name);
	}

	for (int i=0; i<4; i++)
	{
		m_sendBuf[0] = CF_ZONE_INFO;
		m_sendBuf[1] = CF_SUB_ZONE_NAME_SET;
		int nRet = __hton_ZoneName(&m_sendBuf[2], 1024, pIt, i*16, (i+1)*16-1);
		if (nRet)
		{
			ISendTo(m_sendBuf, nRet);
		}
	}
	return 1;
}

int ClientSession::ParseAlarmMode(BYTE* pBuf, int nLen)
{
	if (nLen != 4 || pBuf[2]>5 || pBuf[3]>15)
	{
		ASSERT(FALSE);
		return 0;
	}
	m_sys.alarmMode = ByteToAlarmModel(pBuf[2]);
	m_sys.broadVolumeAdd = pBuf[3];
	return 1;
}

int ClientSession::ParseLineInfo(BYTE* pBuf, int nLen)
{
	int nStructLen = sizeof(SAudioInLineConfig);
	if ( (2+nStructLen) == nLen)
	{
		memcpy(&m_sys.m_audioInLine.config, &pBuf[2], nStructLen);
		return 1;
	}
	ASSERT(0);
	return 0;
}

int ClientSession::ParseZoneName(BYTE* pBuf, int nLen)
{
	return __ParseZoneName(pBuf, nLen, m_sys.m_zone);
}


// 获取项目(不包含详细任务)
int ClientSession::GetGroup() 
{
	ZeroMemory(m_sendBuf, 3);
	m_sendBuf[0] = CF_TASK;
	m_sendBuf[1] = CF_SUB_GROUP_GET;
	m_sendBuf[2] = 0x00;
	return ISendTo(m_sendBuf, 3);
}

// 获取运行程序配置信息
int ClientSession::GetProgramConfig() 
{
	ZeroMemory(m_sendBuf, 3);
	m_sendBuf[0] = CF_TASK;
	m_sendBuf[1] = CF_SUB_PROGRAM_CONFIG_GET;
	m_sendBuf[2] = 0x00;
	return ISendTo(m_sendBuf, 3);
}


// 设置运行程序配置信息
int ClientSession::SetProgramConfig(CString szGroup[7], bool bConfig/* = false*/)
{
	ZeroMemory(m_sendBuf, 136);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_PROGRAM_CONFIG_SET;
	m_sendBuf[nPos++] = bConfig ? 1 : 0;
	for (int i=0; i<7; i++)
	{
		nPos += StringToBuf(szGroup[i], &m_sendBuf[nPos]);
	}
	return ISendTo(m_sendBuf, nPos);
}

// 获取运行程序配置信息
int ClientSession::_DownloadProgramEx(BYTE iDayOfWeek) 
{
	ASSERT(iDayOfWeek < 7);
	m_sendBuf[0] = CF_TASK;
	m_sendBuf[1] = CF_SUB_TASK_DOWN;
	m_sendBuf[2] = iDayOfWeek;
	return ISendTo(m_sendBuf, 3);
}



int ClientSession::PraseProgramConfig(BYTE*pBuf, int nLen)
{
	int nPos = 3;
	m_bProgConfig = pBuf[2] ? true : false;
	for (int i=0; i<7; i++)
	{
		int nNameLen = strlen((char*)&pBuf[nPos]);
		memcpy_s(&m_szProgConfig[i][0], nNameLen+1, &pBuf[nPos], nNameLen+1);
		nPos += (nNameLen+1);
		if (nPos > nLen)
		{
			return -1;
		}
	}
	return nPos;
}



// 添加项目
int ClientSession::GroupAdd(CString& name, BOOL bCopy/* =FALSE */, CString* cpy_name/* =NULL */)
{
	if (!name.GetLength())
	{
		return -1;
	}

	ZeroMemory(m_sendBuf, 24);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_GROUP_ADD;
	m_sendBuf[nPos++] = bCopy ? 0x01 : 0x00;
	//
	nPos += StringToBuf(name, &m_sendBuf[nPos]);
	
	if (bCopy)
	{
		ASSERT(cpy_name);
		if (cpy_name == NULL)
		{
			return -2;
		}
		//
		nPos += StringToBuf(*cpy_name, &m_sendBuf[nPos]);
	}

	return ISendTo(m_sendBuf, nPos);
}

// 删除某项目
int ClientSession::GroupDel(CString& name)
{
	if (!name.GetLength())
	{
		return -1;
	}

	ZeroMemory(m_sendBuf, 24);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_GROUP_DEL;
	m_sendBuf[nPos++] = 0x00;
	//
	nPos += StringToBuf(name, &m_sendBuf[nPos]);
	return ISendTo(m_sendBuf, nPos);
}

int ClientSession::GroupEditName(CString& name, CString& newName) // 修改项目名称
{
	if (!name.GetLength() || !newName.GetLength())
	{
		ASSERT(0);
		return -1;
	}

	ZeroMemory(m_sendBuf, 100);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_GROUP_EDIT_NAME;
	m_sendBuf[nPos++] = 0x00;
	//
	nPos += StringToBuf(name, &m_sendBuf[nPos]);
	//
	nPos += StringToBuf(newName, &m_sendBuf[nPos]);
	return ISendTo(m_sendBuf, nPos);
}

// 获取某项目下任务
int ClientSession::GetGroupTask(CString& name)
{
	return GetGroupTask(CT2A(name));
// 	ZeroMemory(m_sendBuf, 60);
// 	int nPos = 0;
// 	m_sendBuf[nPos++] = CF_TASK;
// 	m_sendBuf[nPos++] = CF_SUB_TASK_GET;
// 	m_sendBuf[nPos++] = 0;
// 	//
// 	nPos += StringToBuf(name, &m_sendBuf[nPos]);
// 	return ISendTo(m_sendBuf, nPos);
}

int ClientSession::GetGroupTask(char* name)
{
	std::string stdName = name;
	if (m_recvGroupTempMap.count(stdName))
	{
		if (m_recvGroupTempMap[stdName] != 1)
		{
			return 1;
		}
	}

	ZeroMemory(m_sendBuf, 60);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_TASK_GET;
	m_sendBuf[nPos++] = 0;
	//
	nPos += StringToBuf(name, &m_sendBuf[nPos]);
	return ISendTo(m_sendBuf, nPos);
}

// 添加任务
// int ClientSession::TaskAdd(CString group_name, S_DB_TASK* pTask)
// {
// 	ZeroMemory(m_sendBuf, 60);
// 	int nPos = 0;
// 	m_sendBuf[nPos++] = CF_TASK;
// 	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
// 	m_sendBuf[nPos++] = TST_NEW;
// 	//
// 	nPos += StringToBuf(group_name, &m_sendBuf[nPos]);
// 	//
// 	ASSERT(pTask);
// 	nPos += __TaskToBytes(pTask, &m_sendBuf[nPos]);
// 
// 	return ISendTo(m_sendBuf, nPos);
// }

int ClientSession::TaskAdd(char* group_name, S_DB_TASK* pTask)
{
	ZeroMemory(m_sendBuf, 60);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
	m_sendBuf[nPos++] = TST_NEW;
	//
	nPos += StringToBuf(group_name, &m_sendBuf[nPos]);
	//
	ASSERT(pTask);
	nPos += __TaskToBytes(pTask, &m_sendBuf[nPos]);

	return ISendTo(m_sendBuf, nPos);
}

//删除任务
// int ClientSession::TaskDel(CString group_name, UINT nTimeSec)
// {
// 	ZeroMemory(m_sendBuf, 60);
// 	int nPos = 0;
// 	m_sendBuf[nPos++] = CF_TASK;
// 	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
// 	m_sendBuf[nPos++] = TST_DEL;
// 	//
// 	nPos += StringToBuf(group_name, &m_sendBuf[nPos]);
// 	nPos += Int2Byte(nTimeSec, &m_sendBuf[nPos]);
// 	return ISendTo(m_sendBuf, nPos);
// }
int ClientSession::TaskDel(char* group_name, UINT nTimeSec)
{
	ZeroMemory(m_sendBuf, 60);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
	m_sendBuf[nPos++] = TST_DEL;
	//
	nPos += StringToBuf(group_name, &m_sendBuf[nPos]);
	nPos += Int2Byte(nTimeSec, &m_sendBuf[nPos]);
	return ISendTo(m_sendBuf, nPos);
}

//删除全部任务
// int ClientSession::TaskDel(CString group_name) 
// {
// 	ZeroMemory(m_sendBuf, 60);
// 	int nPos = 0;
// 	m_sendBuf[nPos++] = CF_TASK;
// 	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
// 	m_sendBuf[nPos++] = TST_DEL_ALL;
// 	//
// 	nPos += StringToBuf(group_name, &m_sendBuf[nPos]);
// 	return ISendTo(m_sendBuf, nPos);
// }

int ClientSession::TaskDel(char* group_name) 
{
	ZeroMemory(m_sendBuf, 60);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
	m_sendBuf[nPos++] = TST_DEL_ALL;
	//
	nPos += StringToBuf(group_name, &m_sendBuf[nPos]);
	return ISendTo(m_sendBuf, nPos);
}

// int ClientSession::TaskEdit(CString group_name, UINT nTimeSec, S_DB_TASK* pTask)
// {
// 	ZeroMemory(m_sendBuf, 60);
// 	int nPos = 0;
// 	m_sendBuf[nPos++] = CF_TASK;
// 	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
// 	m_sendBuf[nPos++] = TST_EDIT;
// 	//
// 	nPos += StringToBuf(group_name, &m_sendBuf[nPos]);
// 	//
// 	nPos += Int2Byte(nTimeSec, &m_sendBuf[nPos]);
// 	//
// 	ASSERT(pTask);
// 	nPos += __TaskToBytes(pTask, &m_sendBuf[nPos]);
// 	return ISendTo(m_sendBuf, nPos);
// }
int ClientSession::TaskEdit(char* group_name, UINT nTimeSec, S_DB_TASK* pTask)
{
	ZeroMemory(m_sendBuf, 60);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_TASK_SET;
	m_sendBuf[nPos++] = TST_EDIT;
	//
	nPos += StringToBuf(group_name, &m_sendBuf[nPos]);
	//
	nPos += Int2Byte(nTimeSec, &m_sendBuf[nPos]);
	//
	ASSERT(pTask);
	nPos += __TaskToBytes(pTask, &m_sendBuf[nPos]);
	return ISendTo(m_sendBuf, nPos);
}

// 获取项目(不包含详细任务)
int ClientSession::ParseGroup(BYTE*pBuf, int nLen)
{
	m_taskGroupVec.clear();
	int nCount = pBuf[2];
	for (int i=0, nPos=3; i<nCount; i++)
	{
		if (nPos < nLen)
		{
			S_DB_TASK_GROUP group;
			int nNameLen = strlen((char*)&pBuf[nPos]);
			group.SetName((char*)&pBuf[nPos]);
			nPos += nNameLen+1;
			m_taskGroupVec.push_back(group);
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

// 获取某项目下任务
int ClientSession::ParseGroupTask(BYTE*pBuf, int nLen)
{
	int nTaskCount, nStart, nEnd;
	int nPos = 3;
	_DbTaskGroupVec::iterator group_find_iter = FindGroup(m_taskGroupVec, (char*)&pBuf[nPos]);
	if (group_find_iter == m_taskGroupVec.end())
	{
		return -1;
	}
	nPos += strlen((char*)&pBuf[nPos])+1;
	nTaskCount = pBuf[nPos++];
	nStart = pBuf[nPos++];
	nEnd = pBuf[nPos++];

	if (1 == nStart)
	{
		group_find_iter->m_taskVec.clear();
	}

	std::map<std::string, int>::const_iterator map_it = m_recvGroupTempMap.begin();
	while( map_it != m_recvGroupTempMap.end())
	{
		if (1 == map_it->second)
		{
			m_recvGroupTempMap.erase(map_it++);
		}
		else
			map_it++;
	}
	m_recvGroupTempMap[std::string(group_find_iter->name)] = (nEnd != nTaskCount) ? -1 : 1;
	TRACE("ClientSession::ParseGroupTask:");
	TRACE(group_find_iter->name);
	TRACE("\n");

	for (int nCur = nStart; nCur <= nEnd && nPos<nLen;  nCur++)
	{
		//
		S_DB_TASK task;
		int nTaskLen = __BytesToTask(&task, &pBuf[nPos]);
		nPos += nTaskLen;
		if (nPos <= nLen)
		{
			group_find_iter->m_taskVec.push_back(task);
		}
		else
		{
			ASSERT(0);
			break;
		}
	}
	return nPos;
}


// 遥控器
int ClientSession::GetRemoteTask() 
{
	ZeroMemory(m_sendBuf, 3);
	m_sendBuf[0] = CF_TASK;
	m_sendBuf[1] = CF_SUB_REMOTE_GET;
	m_sendBuf[2] = 0x00;
	return ISendTo(m_sendBuf, 3);
}

// 遥控器
int ClientSession::SetRemoteTask(_DbTaskGroupVec& remoteVec)
{
	ZeroMemory(m_sendBuf, 136);
	int nPos = 0;
	m_sendBuf[nPos++] = CF_TASK;
	m_sendBuf[nPos++] = CF_SUB_REMOTE_SET;
	m_sendBuf[nPos++] = _MAX_REMOTE_FUNC;
	nPos += RemoteTaskToByte(remoteVec, &m_sendBuf[nPos]);
	return ISendTo(m_sendBuf, nPos);
}

// 遥控器
int ClientSession::ParseRemoteTask(BYTE*pBuf, int nLen)
{
	_DbTaskGroupVec remoteVec;
	int nPos = 3;
	if (ByteToRemoteTask(remoteVec, &pBuf[nPos], nLen-3) != _MAX_REMOTE_FUNC)
	{
		return 0;
	}
	m_remoteVec = remoteVec;
	return nPos;
}


// MY_INLINE void ClientSession::OnClose(int nErrorCode)
// {
// 	m_bConnect = false;
// 	m_bLogin = false;
// 	CString strMsg;
// 	switch(nErrorCode)
// 	{
// 	case 0:
// 		strMsg = TEXT("The server function executed.");
// 		break;
// 	case WSAENETDOWN:
// 		strMsg = TEXT("The Windows Sockets implementation detected that the network subsystem failed.");
// 		break;
// 	case WSAECONNRESET:
// 		strMsg = TEXT("The connection was reset by the remote side.");
// 		break;
// 	case WSAECONNABORTED:
// 		strMsg = TEXT("The connection was aborted due to timeout or other failure.");
// 		break;
// 	}
// 
// 	//if(nErrorCode != 0)
// 	{
// 		AfxMessageBox(strMsg + TEXT(" Please Close this function"));
// 	}
// }
// 
void ClientSession::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	BYTE  buff[1024];
	int nRead = CAsyncSocket::Receive(buff, 1024);
	switch (nRead)
	{
	case 0:
		Close();
		break;
	case SOCKET_ERROR:
		if (GetLastError() != WSAEWOULDBLOCK) 
		{
			AfxMessageBox (_T("Error occurred"));
			Close();
		}
		break;
	default:
		CommandPool::ReceiveChar(buff, nRead);
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

void AlarmConnectError(int nErrorCode)
{
	switch(nErrorCode)
	{
	case 0:
		break;
	case WSAEADDRINUSE: 
		AfxMessageBox(_T("The specified address is already in use.\n"));
		break;
	case WSAEADDRNOTAVAIL: 
		AfxMessageBox(_T("The specified address is not available from ")
			_T("the local machine.\n"));
		break;
	case WSAEAFNOSUPPORT: 
		AfxMessageBox(_T("Addresses in the specified family cannot be ")
			_T("used with this socket.\n"));
		break;
	case WSAECONNREFUSED: 
		AfxMessageBox(_T("The attempt to connect was forcefully rejected.\n"));
		break;
	case WSAEDESTADDRREQ: 
		AfxMessageBox(_T("A destination address is required.\n"));
		break;
	case WSAEFAULT: 
		AfxMessageBox(_T("The lpSockAddrLen argument is incorrect.\n"));
		break;
	case WSAEINVAL: 
		AfxMessageBox(_T("The socket is already bound to an address.\n"));
		break;
	case WSAEISCONN: 
		AfxMessageBox(_T("The socket is already connected.\n"));
		break;
	case WSAEMFILE: 
		AfxMessageBox(_T("No more file descriptors are available.\n"));
		break;
	case WSAENETUNREACH: 
		AfxMessageBox(_T("The network cannot be reached from this host ")
			_T("at this time.\n"));
		break;
	case WSAENOBUFS: 
		AfxMessageBox(_T("No buffer space is available. The socket ")
			_T("cannot be connected.\n"));
		break;
	case WSAENOTCONN: 
		AfxMessageBox(_T("The socket is not connected.\n"));
		break;
	case WSAENOTSOCK: 
		AfxMessageBox(_T("The descriptor is a file, not a socket.\n"));
		break;
	case WSAETIMEDOUT: 
		AfxMessageBox(_T("The attempt to connect timed out without ")
			_T("establishing a connection. \n"));
		break;
	default:
		TCHAR szError[256];
		_stprintf_s(szError, _T("OnConnect error: %d"), nErrorCode);
		AfxMessageBox(szError);
		break;
	}
}

void ClientSession::OnConnect(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	if (0 != nErrorCode)
	{
		m_bConnect = FALSE;
	}
	else
	{
		m_bConnect = TRUE;
	}
	NotifyMsg(WM_ASYNCSOCKET_CONNECT, nErrorCode);

	CAsyncSocket::OnConnect(nErrorCode);
}

void ClientSession::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	m_bConnect = false;
	m_bLogin = false;
	m_bDevOnline = false;
	NotifyMsg(WM_ASYNCSOCKET_CLOSE, nErrorCode);
	CAsyncSocket::OnClose(nErrorCode);
}
