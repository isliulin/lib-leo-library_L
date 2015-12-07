#include "stdafx.h"
#include "ISession.h"

#define MY_INLINE

BOOL __IsRetCmd(BYTE cmd)
{
	switch (cmd)
	{
	case CF_GET_HOST_STATE:
	case CF_GET_IP_ADDR:
	case CF_SET_IP_ADDR:
	case CF_GET_SERIAL_NUM:
	case CF_SET_SERIAL_NUM:
	case CF_SET_REGISTER_NUM:
		return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}


BYTE __GetCmdRetByte(BYTE cmd)
{
	BYTE iRet = cmd;
	switch (cmd)
	{
	case CF_GET_HOST_STATE:
		break;
	case CF_GET_IP_ADDR:
	case CF_SET_IP_ADDR: 
		//iRet = CF_IP_ADDR_RET;
		break;
	case CF_GET_SERIAL_NUM:
	case CF_SET_SERIAL_NUM:
	case CF_SET_REGISTER_NUM:
		iRet = CF_SERIAL_NUM_RET;
		break;
		// mp3 cd radio
	case CF_QUERY_MP3_1:
	case CF_QUERY_MP3_1+1:
	case CF_QUERY_MP3_1+2:
	case CF_QUERY_MP3_1+3:
	case CF_QUERY_CD_1:
	case CF_QUERY_CD_1+1:
	case CF_QUERY_CD_1+2:
	case CF_QUERY_CD_1+3:
	case CF_QUERY_RADIO_1:
	case CF_QUERY_RADIO_1+1:
	case CF_QUERY_RADIO_1+2:
	case CF_QUERY_RADIO_1+3:
	case CF_CONTROL_MP3_1:
	case CF_CONTROL_MP3_1+1:
	case CF_CONTROL_MP3_1+2:
	case CF_CONTROL_MP3_1+3:
	case CF_CONTROL_CD_1:
	case CF_CONTROL_CD_1+1:
	case CF_CONTROL_CD_1+2:
	case CF_CONTROL_CD_1+3:
	case CF_CONTROL_RADIO_1:
	case CF_CONTROL_RADIO_1+1:
	case CF_CONTROL_RADIO_1+2:
	case CF_CONTROL_RADIO_1+3:
		break;
	case CF_CONTROL_POWER:
	case CF_CONTROL_CALLSTATION:
	case CF_CONTROL_BORADCAST:
		break;
		// program
	case CF_PROGRAM_DOWN_ENTRY:
	case CF_PROGRAM_DOWN:
	case CF_PROGRAM_END:
		break;
	default:
		iRet = 0xFF;
		break;
	}
	return iRet;
}

UINT __GetElapseTimer(BYTE cmd)
{
	switch (cmd)
	{
	case CF_GET_IP_ADDR:
	case CF_SET_IP_ADDR:
	case CF_GET_SERIAL_NUM:
	case CF_SET_SERIAL_NUM:
	case CF_SET_REGISTER_NUM:
		return 500;
	default:
		return 300;
		break;
	}
	return 300;
}

int __hton_ZoneName(OUT BYTE* pBuf, int nBufLen, SZoneUnitA* pZoneUnit, int nZoneStart, int nZoneEnd)
{
	if (!(nZoneStart>=0 && nZoneEnd<_MAX_ZONEA && nZoneStart<=nZoneEnd))
	{
		ASSERT(FALSE);
		return 0;
	}
	ZeroMemory(pBuf, nBufLen);
	int nPos=0;
	for (int iZone=nZoneStart; iZone<=nZoneEnd; iZone++)
	{
		pBuf[nPos++] = iZone;
		memcpy_s(&pBuf[nPos], _MAX_NAME_LEN, pZoneUnit[iZone].name, _MAX_NAME_LEN);
		nPos += _MAX_NAME_LEN;
	}
	return nPos;
}

int __ParseZoneName(BYTE* pBuf, int nLen, SZoneA& zone)
{
	if ( ((nLen-2)%(1+_MAX_NAME_LEN)) != 0)
	{
		return 0;
	}
	for (int nPos=2; nPos<nLen; nPos+=(1+_MAX_NAME_LEN))
	{
		if (pBuf[nPos] >= _MAX_ZONEA)
		{
			return 0;
		}
	}

	for (int nPos=2; nPos<nLen; nPos+=(1+_MAX_NAME_LEN))
	{
		memcpy_s(zone.it[ pBuf[nPos] ].name, _MAX_NAME_LEN, &pBuf[nPos+1], _MAX_NAME_LEN);
	}
	return 1;
}

int __TaskToBytes(S_DB_TASK* pTask, BYTE* pBuf)
{
	ASSERT(pBuf);

	int nPos = 0;
	Int2Byte(pTask->nTimeSec, &pBuf[nPos]);
	nPos += 4;

	int nLen = strlen(pTask->name);
	memcpy_s(&pBuf[nPos], _MAX_NAME_LEN+1, pTask->name, nLen);
	nPos += nLen;
	pBuf[nPos++] = NULL;

	pBuf[nPos++] = pTask->devType;

	if (_DEV_DEFAULT == pTask->devType)
	{
		;
	} else if (_DEV_TYPE_POWER == pTask->devType)
	{
		pBuf[nPos++] = (BYTE) pTask->bPowerOnVec.to_ulong();
	} 
	else if (_DEV_TYPE_MP3 == pTask->devType || 
		_DEV_TYPE_CD == pTask->devType)
	{
		pBuf[nPos++] = pTask->nDev; 
		if (_DEV_TYPE_MP3 == pTask->devType&&(pTask->playMode == PCC_FILEON||pTask->playMode == 0x07))
		{
			pTask->playMode = 7;
			pBuf[nPos++] = pTask->playMode;
			for(int i = 0;i<6;i++)
			{
				pBuf[nPos++] = pTask->multisong[i];
			}
		}
		else
		{
			pBuf[nPos++] = pTask->playMode;
			pBuf[nPos++] = pTask->song; 
		}
		
	}
	else if (_DEV_TYPE_RADIO == pTask->devType)
	{
		pBuf[nPos++] = pTask->nDev; 
		pBuf[nPos++] = pTask->is_FM; 
		pBuf[nPos++] = pTask->is_channel; 
		if (pTask->is_channel)
		{
			pBuf[nPos++] = pTask->nChannel; 
		} 
		else
		{
			float frate = pTask->rate*100;
			int tmp = (int)frate;
			int nRet = Int2Byte(tmp, &pBuf[nPos]);
			nPos +=	nRet;
		}
	}
	else if (_TYPE_ZONE_LINE == pTask->devType)
	{
		pBuf[nPos++] = pTask->nLine;
	}
	else if (_TYPE_ZONE_VOLUME == pTask->devType)
	{
		pBuf[nPos++] = pTask->nVolume;
	}
	else
		ASSERT(FALSE);

	//
	if (_TYPE_ZONE_LINE == pTask->devType ||
		_TYPE_ZONE_VOLUME == pTask->devType)
	{
		int nRet = __CompressSelectZoneToBytes(pTask->bSelZoneVec, &pBuf[nPos]);
		nPos += nRet;
	}

	return nPos;
}

int __BytesToTask(S_DB_TASK* pTask, BYTE* pBuf)
{
	ASSERT(pBuf);

	int nPos = 0;
	pTask->nTimeSec = Byte2Int(&pBuf[nPos]);
	nPos += 4;

	nPos += BufToString(pTask->name, &pBuf[nPos]);

	pTask->devType = pBuf[nPos++];


	if (_DEV_DEFAULT == pTask->devType)
	{
		;
	} else if (_DEV_TYPE_POWER == pTask->devType)
	{
		Byte2BitSet(pBuf[nPos++], pTask->bPowerOnVec);
	} 
	else if (_DEV_TYPE_MP3 == pTask->devType || 
		_DEV_TYPE_CD == pTask->devType)
	{
		pTask->nDev = pBuf[nPos++]; 
		pTask->playMode = pBuf[nPos++]; 
		//pTask->song = pBuf[nPos++]; 
		if (pTask->devType == _DEV_TYPE_MP3&&pTask->playMode == 0x07)
		{
			pTask->playMode = PCC_FILEON;
			for (int i = 0;i<6;i++)
			{
				pTask->multisong[i] = pBuf[nPos++];
			}
		}
		else
		{
			pTask->song = pBuf[nPos++];
		}
	}
	else if (_DEV_TYPE_RADIO == pTask->devType)
	{
		pTask->nDev = pBuf[nPos++]; 
		pTask->is_FM = pBuf[nPos++]; 
		pTask->is_channel = pBuf[nPos++]; 
		if (pTask->is_channel)
		{
			pTask->nChannel = pBuf[nPos++]; 
		} 
		else
		{
			int tmp = Byte2Int(&pBuf[nPos]);
			float frate = (float)tmp;
			pTask->rate =  frate/100;
			nPos += 4;
		}
	}
	else if (_TYPE_ZONE_LINE == pTask->devType)
	{
		pTask->nLine = pBuf[nPos++];
	}
	else if (_TYPE_ZONE_VOLUME == pTask->devType)
	{
		pTask->nVolume = pBuf[nPos++];
	}
	else
	{
		ASSERT(FALSE);
	}

	//
	if (_TYPE_ZONE_LINE == pTask->devType ||
		_TYPE_ZONE_VOLUME == pTask->devType)
	{
		int nRet = __DecompressBytesToSelectZone(pTask->bSelZoneVec, &pBuf[nPos]);
		nPos += nRet;
	}

	return nPos;
}


int __MakeCmd_MP3(OUT BYTE* pBuf, int nDev, PLAYER_CTRL_MODE code, int nSong)
{
	ASSERT(pBuf);
	ASSERT(nDev < 4);
	pBuf[0] = CF_CONTROL_MP3_1+nDev;
	pBuf[1] = code!=PCC_FILEON ? code : 7;//新增播放方式，更改
		pBuf[2] = (BYTE)nSong;
		return 3;
}
int __MakeCmd_MutliMP3(OUT BYTE* pBuf, int nDev, PLAYER_CTRL_MODE code, BYTE* nSong)
{
	ASSERT(pBuf);
	ASSERT(nDev < 4);
	pBuf[0] = CF_CONTROL_MP3_1+nDev;
	pBuf[1] = code!=PCC_FILEON ? code : 7;//新增播放方式，更改
	for(int i = 0;i< 6 ;i++)
		pBuf[2 + i] = nSong[i];
	return 8;
}

int __MakeCmd_CD(OUT BYTE* pBuf, int nDev, PLAYER_CTRL_MODE code, int nSong)
{
	ASSERT(nDev < 4);
	pBuf[0] = CF_CONTROL_CD_1+nDev;
	pBuf[1] = code!=PCC_FILEON ? (code-1) : 4;//新增播放方式，更改，CD无此项
	pBuf[2] = (BYTE)nSong;
	return 3;
}

int __MakeCmd_Radio(OUT BYTE* pBuf, int nDev, RADIO_CTRL_CODE code, BYTE param1/*=0*/, BYTE param2/*=0*/)
{
	ASSERT(nDev < 4);
	pBuf[0] = CF_CONTROL_RADIO_1+nDev;
	pBuf[1] = code;
	pBuf[2] = param1;
	pBuf[3] = param2;
	return 4;
}

int __MakeCmd_Power(OUT BYTE* pBuf, std::bitset<_MAX_POWER>& bPowerOnVec)
{
	pBuf[0] = CF_CONTROL_POWER;
	pBuf[1] = 0;
	for (int i=0; i<_MAX_POWER; i++)
	{
		if (!bPowerOnVec[i])
		{
			pBuf[1] |= ( 0x01 << i);
		}
	}
	return 2;
}

// nLineVec[8]: 0~7：正常线路；0x0f：线路不变
int __MakeCmd_MatrixLine(OUT BYTE* pBuf, int nMatrix, BYTE nLineVec[8])
{
	if (nMatrix>_SINGLE_LINE_MATRIXS)
	{
		ASSERT(FALSE);
		return 0;
	}
	for (int i=0; i<8; i++)
	{
		if (nLineVec[i] > _MAX_AUDIO_LINE && nLineVec[i] != _DEV_TYPE_MUTE_VOLUME && nLineVec[i] != 0X0F)
		{
			ASSERT(FALSE);
			return 0;
		}
	}

	int nPos=0;
	pBuf[nPos++] = CF_CONTROL_ZONE_LINE_MATRIX_1 + nMatrix;
	for (int i=0; i<8; i+=2)
	{
		pBuf[nPos++] = (nLineVec[i]&0x0f) | ( (nLineVec[i+1]&0x0f)<<4 );
	}
	return nPos;
}

// nVolVec[]: 0~_MAX_ZONE_VOLUME; 0x0f：音量不变
int __MakeCmd_MatrixVolume(OUT BYTE* pBuf, int nMatrix, BYTE nVolVec[8])
{
	if (nMatrix>_SINGLE_LINE_MATRIXS)
	{
		ASSERT(FALSE);
		return 0;
	}
	for (int i=0; i<8; i++)
	{
		if (nVolVec[i] > _MAX_ZONE_VOLUME && nVolVec[i] != 0X0F)
		{
			ASSERT(FALSE);
			return 0;
		}
	}

	int nPos=0;
	pBuf[nPos++] = CF_CONTROL_ZONE_VOLUME_MATRIX_1 + nMatrix;
	for (int i=0; i<8; i+=2)
	{
		pBuf[nPos++] = (nVolVec[i]&0x0f) | ( (nVolVec[i+1]&0x0f)<<4);
	}
	return nPos;
}


//////////////////////////////////////////////////////////////////////////
MY_INLINE void ISession::SetTargetSockAddr(ULONG net_addr, USHORT net_port)
{
	m_saTarget.sin_family = AF_INET;
	m_saTarget.sin_port = net_port;
	m_saTarget.sin_addr.S_un.S_addr = net_addr;
}


//////////////////////////////////////////////////////////////////////////
MY_INLINE int ISession::_EnterDownloadProgram()
{
	m_sendBuf[0] = CF_PROGRAM_DOWN_ENTRY;
	m_sendBuf[1] = 0;
	return ISendTo(m_sendBuf, 2);
}

MY_INLINE int ISession::_EndDownloadProgram()
{
	m_sendBuf[0] = CF_PROGRAM_END;
	m_sendBuf[1] = 0;
	return ISendTo(m_sendBuf, 2);
}

int TaskToCmd(S_DB_TASK* pTask, OUT BYTE* pBuf)
{
	int nPos = 0;
	if (_DEV_TYPE_POWER == pTask->devType)
	{
		nPos = __MakeCmd_Power(pBuf, pTask->bPowerOnVec);
		pBuf[nPos++] = 0x00;
	}
	else if (_DEV_TYPE_MP3 == pTask->devType)
	{
		if (ByteToCtrlMode(pTask->playMode) == PCC_FILEON)
			return __MakeCmd_MutliMP3(pBuf, pTask->nDev, ByteToCtrlMode(pTask->playMode), pTask->multisong);
		else
			return __MakeCmd_MP3(pBuf, pTask->nDev, ByteToCtrlMode(pTask->playMode), pTask->song);
	}
	else if (_DEV_TYPE_CD == pTask->devType)
	{
		return __MakeCmd_CD(pBuf, pTask->nDev, ByteToCtrlMode(pTask->playMode), pTask->song);
	}
	else if (_DEV_TYPE_RADIO == pTask->devType)
	{
		RADIO_CTRL_CODE code;
		BYTE param1 = 0, param2=0;               

		if (pTask->is_FM)
		{
			if (pTask->is_channel)
			{
				code = RCC_FM_CHANNEL;
				param1 = pTask->nChannel - 1;
			}
			else
			{
				code = RCC_FM_RATE;
				param1 = (BYTE)pTask->rate;
				float frate = pTask->rate*100;
				int tmp = (int)frate;
				param2 = (BYTE)tmp;
			}
		} 
		else
		{
			if (pTask->is_channel)
			{
				code = RCC_AM_CHANNEL;
				param1 = pTask->nChannel - 1;
			}
			else
			{
				code = RCC_AM_RATE;
				param1 = (BYTE)(pTask->rate/100);
				float frate = pTask->rate;
				int tmp = (int)frate;
				param2 = (BYTE)(tmp%100);
			}
		}
		return __MakeCmd_Radio(pBuf, pTask->nDev, code, param1, param2);
	}
	else if (_DEV_TYPE_PC_MEDIA == pTask->devType)
	{
		pBuf[nPos++] = CF_CONTROL_PC_MEDIA ;
	}
	else if (_TYPE_ZONE_LINE == pTask->devType)
	{
		pBuf[nPos++] = CF_CONTROL_ZONE_LINE_MATRIX_1;
		pBuf[nPos++] = pTask->nLine;
	}
	else if (_TYPE_ZONE_VOLUME == pTask->devType)
	{
		pBuf[nPos++] = CF_CONTROL_ZONE_VOLUME_MATRIX_1;
		pBuf[nPos++] = pTask->nVolume;
	}
	else
	{
	}

	// 分区
	if (_TYPE_ZONE_LINE == pTask->devType ||
		_TYPE_ZONE_VOLUME == pTask->devType)
	{
		for (int i_zone=0; i_zone<_MAX_ZONEA; )
		{
			pBuf[nPos] = 0;
			for (int j=0; j<8 && i_zone<_MAX_ZONEA; j++,i_zone++)
			{
				if (pTask->bSelZoneVec.at(i_zone))
				{
					pBuf[nPos] |= (0x80>>j);
				}
			}
			nPos++;
		}
		return nPos+8;
	}

	return nPos;
}

//星期一：0，....
MY_INLINE int ISession::_DownloadProgram(BYTE iDayOfWeek, _DbTaskVec& taskVec)
{
	ZeroMemory(m_sendBuf, 1024);
	if (iDayOfWeek > 7)
	{
		ASSERT(0);
		return -1;
	}
	std::stable_sort(taskVec.begin(), taskVec.end(), isEarlier);
	_DbTaskVec::iterator iter = taskVec.begin();
	BYTE nStep = 0;
	int iSendPages = 0;
	do
	{
		m_sendBuf[0] = CF_PROGRAM_DOWN;
		m_sendBuf[1] = iDayOfWeek;
		for (int iPage = 0, nPos; iPage < 2; iPage++)
		{
			m_sendBuf[2] = (BYTE) iPage;
			m_sendBuf[3] = nStep;
			m_sendBuf[4] = 0;
			nPos = 5;
			for (int i=0; i<8; i++)
			{
				if (iter != taskVec.end())
				{
					// 标志
					m_sendBuf[nPos++] = 0XAA;
					// 时间
					ParseTaskSecond(iter->nTimeSec, m_sendBuf[nPos], m_sendBuf[nPos+1], m_sendBuf[nPos+2]);
					nPos += 3;
					// 控制代码 + 控制数据
					TaskToCmd(&(*iter), &m_sendBuf[nPos]);
					nPos += 12; 
					
					// 
					iter++;
				}
				else
				{
					memset(&m_sendBuf[nPos], 0xFF, 16);
					nPos += 16;
				}
			}
			//
			iSendPages++;
			ISendTo(m_sendBuf, nPos);
			Sleep(10);
		}
		nStep++;
	} while (iter != taskVec.end());
	return iSendPages;
}

MY_INLINE int ISession::_DownRemoteTask(_DbTaskGroupVec& remoteVec)
{
	m_sendBuf[0] = CF_SET_REMOTE_CONTROL;
	int nPos = 1;
	for (int i=0; i<_MAX_REMOTE_FUNC; i++)
	{
		for (int iSub = 0; iSub<_MAX_REMOTE_SUB; iSub++)
		{
			if (iSub < (int)remoteVec.at(i).m_taskVec.size() && 
				remoteVec.at(i).m_taskVec.at(iSub).devType != _DEV_DEFAULT)
			{
				// 标志
				m_sendBuf[nPos++] = 0xAA;
				// 时间
 				ParseTaskSecond(remoteVec.at(i).m_taskVec.at(iSub).nTimeSec, m_sendBuf[nPos], m_sendBuf[nPos+1], m_sendBuf[nPos+2]);
 				nPos += 3;
				// 控制代码 + 控制数据
				TaskToCmd(& remoteVec.at(i).m_taskVec.at(iSub), &m_sendBuf[nPos]);
				nPos += 12;
			} 
			else
			{
				memset(&m_sendBuf[nPos], 0xFF, 16);
				nPos += 16;
			}
		}
	}
	//
	return ISendTo(m_sendBuf, nPos);
}


//////////////////////////////////////////////////////////////////////////
// 获取设备状态
MY_INLINE int ISession::_GetHostState(BOOL bAppend/* =FALSE */, CTime* pTime/*=NULL*/, E_ALARM_MODEL alarmMode/* = AM_CURRENT_ZONE */, BYTE broadVolumeAdd/* =0 */)
{
	ZeroMemory(m_sendBuf, 8);
	int iPos = 0;
	m_sendBuf[iPos++] = CF_GET_HOST_STATE;
	if (bAppend)
	{
		ASSERT(pTime);
		ASSERT(broadVolumeAdd <= 20);
		m_sendBuf[iPos++] = 0xAA;
		m_sendBuf[iPos++] = pTime->GetYear() - 2000; // 年
		m_sendBuf[iPos-1] = ((m_sendBuf[iPos-1] / 10) << 4) + (m_sendBuf[iPos-1] % 10);  
		m_sendBuf[iPos++] = pTime->GetMonth(); 
		m_sendBuf[iPos-1] = ((m_sendBuf[iPos-1] / 10) << 4) + (m_sendBuf[iPos-1] % 10);  
		m_sendBuf[iPos++] = pTime->GetDay();
		m_sendBuf[iPos-1] = ((m_sendBuf[iPos-1] / 10) << 4) + (m_sendBuf[iPos-1] % 10);  
		m_sendBuf[iPos++] = pTime->GetDayOfWeek(); // 星期, 1 = Sunday, 2 = Monday, to 7 = Saturday
		m_sendBuf[iPos-1] = ((m_sendBuf[iPos-1] / 10) << 4) + (m_sendBuf[iPos-1] % 10);  
		m_sendBuf[iPos++] = pTime->GetHour();
		m_sendBuf[iPos-1] = ((m_sendBuf[iPos-1] / 10) << 4) + (m_sendBuf[iPos-1] % 10);  
		m_sendBuf[iPos++] = pTime->GetMinute();
		m_sendBuf[iPos-1] = ((m_sendBuf[iPos-1] / 10) << 4) + (m_sendBuf[iPos-1] % 10);  
		m_sendBuf[iPos++] = pTime->GetSecond();
		m_sendBuf[iPos-1] = ((m_sendBuf[iPos-1] / 10) << 4) + (m_sendBuf[iPos-1] % 10);  
		m_sendBuf[iPos++] = alarmMode;
		m_sendBuf[iPos++] = broadVolumeAdd;
	}
	else
	{
		m_sendBuf[iPos++] = 0x01;
		m_sendBuf[iPos++] = 0x00;
		m_sendBuf[iPos++] = 0x00;
		m_sendBuf[iPos++] = 0x00;
	}

	return ISendTo(m_sendBuf, iPos);
}
// IP和物理地址
MY_INLINE int ISession::_GetIPAddr()
{
	ZeroMemory(m_sendBuf, 8);
	m_sendBuf[0] = CF_GET_IP_ADDR;
	return ISendTo(m_sendBuf, 8);
}

MY_INLINE int ISession::_SetIPAddr(ULONG net_addr)
{
	ASSERT(net_addr>0);
	ZeroMemory(m_sendBuf, 8);
	m_sendBuf[0] = CF_SET_IP_ADDR;
	memcpy_s(&m_sendBuf[1], 8, (BYTE*)&net_addr, 4);
	return ISendTo(m_sendBuf, 8);
}

// 序列号和物理地址
MY_INLINE int ISession::_GetSerialNumAndMac()
{
	ZeroMemory(m_sendBuf, 8);
	m_sendBuf[0] = CF_GET_SERIAL_NUM;
	return ISendTo(m_sendBuf, 1);
}

MY_INLINE int ISession::_SetSerialNumAndMac(BYTE physicalAddr[6], IN_ADDR ipAddr, USHORT serialNum[4])
{
	ZeroMemory(m_sendBuf, 32);
	m_sendBuf[0] = CF_SET_SERIAL_NUM;
	memcpy_s(&m_sendBuf[1], 32, physicalAddr, 6);
	memcpy_s(&m_sendBuf[7], 32, (BYTE*)&ipAddr, 4);
	int nPos = 11;
	for (int i=0; i<4; i++)
	{
		m_sendBuf[nPos++] = (BYTE) (serialNum[i] >> 8);
		m_sendBuf[nPos++] = (BYTE) serialNum[i];
	}
	ASSERT(19 == nPos);
	return ISendTo(m_sendBuf, nPos);
}

// 6800注册
MY_INLINE int ISession::_Register(USHORT registerNum[4])
{
	ZeroMemory(m_sendBuf, 16);
	int nPos=0;
	m_sendBuf[nPos++] = CF_SET_REGISTER_NUM;
	for (int i=0; i<4; i++)
	{
		m_sendBuf[nPos++] = (BYTE) (registerNum[i] >> 8);
		m_sendBuf[nPos++] = (BYTE) registerNum[i];
	}
	return ISendTo(m_sendBuf, nPos);
}

// 查询
MY_INLINE int ISession::_QueryMP3(int nDev)
{
	ASSERT(nDev < 4);
	ZeroMemory(m_sendBuf, 8);
	m_sendBuf[0] = CF_QUERY_MP3_1+nDev;
	return ISendTo(m_sendBuf, 6);
}

MY_INLINE int ISession::_QueryCD(int nDev)
{
	ASSERT(nDev < 4);
	ZeroMemory(m_sendBuf, 8);
	m_sendBuf[0] = CF_QUERY_CD_1+nDev;
	return ISendTo(m_sendBuf, 6);
}

MY_INLINE int ISession::_QueryRadio(int nDev)
{
	ASSERT(nDev < 4);
	ZeroMemory(m_sendBuf, 8);
	m_sendBuf[0] = CF_QUERY_RADIO_1+nDev;
	return ISendTo(m_sendBuf, 6);
}


// 控制MP3	1-循环播放,2-循环单曲,3-播放,4-播放单曲,5-停止,6-暂停
MY_INLINE int ISession::_ControlMP3(int nDev, PLAYER_CTRL_MODE code, int nSong/* = 1*/)
{
	if (nSong == 0)
	{
		ASSERT(FALSE);
		nSong = 1;
	}
	ZeroMemory(m_sendBuf, 6);
	__MakeCmd_MP3(m_sendBuf, nDev, code, nSong);
	return ISendTo(m_sendBuf, 6);
}
//新增选曲循环播放，协议先单独新增
MY_INLINE int ISession::_ControlSelMultiMP3(int nDev,PLAYER_CTRL_MODE code,int* nSong)
{
	ZeroMemory(m_sendBuf,8);
	ASSERT(nDev < 4);
	m_sendBuf[0] = CF_CONTROL_MP3_1+nDev;
	m_sendBuf[1] = code!=PCC_FILEON ? code : 7;//新增播放方式，更改
	for (int i = 0;i< 6;i++)
	{
		m_sendBuf[i+2] = (BYTE)nSong[i];
	}
	return ISendTo(m_sendBuf,8);
}

// 控制CD	0-循环播放,1-循环单曲,2-播放,3-播放单曲,4-停止
MY_INLINE int ISession::_ControlCD(int nDev, PLAYER_CTRL_MODE code, int nSong/* = 1*/)
{
	if (nSong == 0)
	{
		ASSERT(FALSE);
		nSong = 1;
	}
	ZeroMemory(m_sendBuf, 6);
	__MakeCmd_CD(m_sendBuf, nDev, code, nSong ? nSong : 1);
	return ISendTo(m_sendBuf, 6);
}


// 控制RADIO
MY_INLINE int ISession::_ControlRadio(int nDev, RADIO_CTRL_CODE code, BYTE param1/*=0*/, BYTE param2/*=0*/)
{
	ZeroMemory(m_sendBuf, 8);
	__MakeCmd_Radio(m_sendBuf, nDev, code, param1, param2);
	return ISendTo(m_sendBuf, 5);
}

// 控制电源
MY_INLINE int ISession::_ControlPower(int iIndex, bool bOn/* = TRUE*/)
{
	std::bitset<_MAX_POWER> bOnVec;
	for (int i=0; i<_MAX_POWER; i++)
	{
		bOnVec[i] = m_sys.m_power.bPowerOnVec[i];
	}
	bOnVec[iIndex] = bOn;
	return _ControlPower(bOnVec);
}


MY_INLINE int ISession::_ControlPower(	std::bitset<_MAX_POWER>& bPowerOnVec)
{
	ZeroMemory(m_sendBuf, 6);
	__MakeCmd_Power(m_sendBuf, bPowerOnVec);
	return ISendTo(m_sendBuf, 5);
}

// 分区控制： 线路设置, 音量控制
MY_INLINE int ISession::_SetZoneLine(bool bAllZone, int nZone, BYTE nLine)
{
	if ( ((nLine > _MAX_AUDIO_LINE) && nLine != _DEV_TYPE_MUTE_VOLUME) ||
		(nZone>=_MAX_ZONEA))
	{
		ASSERT(FALSE);
		return -1;
	}

	BYTE nLineVec[8];
	if (bAllZone)
	{
		for (int i=0; i<8; i++)
		{
			nLineVec[i] = nLine;
		}
		for (int iMatrix=0; iMatrix<_SINGLE_LINE_MATRIXS; iMatrix++)
		{
			_SetMatrixLine(iMatrix, nLineVec);
		}
		return 1;
	} 
	else
	{
		for (int i=0; i<8; i++)
		{
			nLineVec[i] = 0x0F;
		}
		nLineVec[nZone%8] = nLine;
		return _SetMatrixLine(nZone/8, nLineVec);
	}
}

MY_INLINE int ISession::_SetZoneLine(bool bZoneSel[_MAX_ZONEA], BYTE nLine)
{
	if (nLine > _MAX_AUDIO_LINE && nLine != _DEV_TYPE_MUTE_VOLUME) 
	{
		ASSERT(FALSE);
		return -1;
	}
	BYTE nLineVec[8];
	for (int iMatrix=0, iZone=0; iMatrix<_SINGLE_LINE_MATRIXS; iMatrix++)
	{
		bool bValid = false;
		for (int i=0; i<8 && iZone < _MAX_ZONEA; i++)
		{
			if (bZoneSel[iZone++])
			{
				nLineVec[i] =  nLine;
				bValid = true;
			} 
			else
			{
				nLineVec[i] =  0x0F;
			}

		}
		if (bValid)
		{
			_SetMatrixLine(iMatrix, nLineVec);
		}
	}
	return 1;
}


MY_INLINE int ISession::_SetMatrixLine(int nMatrix, BYTE nLineVec[8])
{
	ZeroMemory(m_sendBuf, 128);
	int nPos = __MakeCmd_MatrixLine(m_sendBuf, nMatrix, nLineVec);
	int iRet = ISendTo(m_sendBuf, nPos);
	Sleep(100);
	return iRet;
}

MY_INLINE int ISession::_SetZoneVolume(bool bAllZone, int nZone, BYTE nVol)
{
	if ( (nVol > _MAX_ZONE_VOLUME) ||
		(nZone>=_MAX_ZONEA))
	{
		ASSERT(FALSE);
		return -1;
	}

	BYTE nVolVec[8];
	if (bAllZone)
	{
		for (int i=0; i<8; i++)
		{
			nVolVec[i] = nVol;
		}
		for (int iMatrix=0; iMatrix<_SINGLE_LINE_MATRIXS; iMatrix++)
		{
			_SetMatrixVolume(iMatrix, nVolVec);
		}
		return 1;
	} 
	else
	{
		for (int i=0; i<8; i++)
		{
			nVolVec[i] = 0x0F;
		}
		nVolVec[nZone%8] = nVol;
		return _SetMatrixVolume(nZone/8, nVolVec);
	}
}

MY_INLINE int ISession::_SetZoneVolume(bool bZoneSel[_MAX_ZONEA], BYTE nVol)
{
	if (nVol > _MAX_ZONE_VOLUME) 
	{
		ASSERT(FALSE);
		return -1;
	}
	BYTE nLineVec[8];
	for (int iMatrix=0, iZone=0; iMatrix<_SINGLE_LINE_MATRIXS; iMatrix++)
	{
		bool bValid = false;
		for (int i=0; i<8; i++)
		{
			if (bZoneSel[iZone++])
			{
				nLineVec[i] =  nVol;
				bValid = true;
			} 
			else
			{
				nLineVec[i] =  0x0F;
			}

		}
		if (bValid)
		{
			_SetMatrixVolume(iMatrix, nLineVec);
		}
	}
	return 1;
}


MY_INLINE int ISession::_SetMatrixVolume(int nMatrix, BYTE nVolVec[8])
{
	ZeroMemory(m_sendBuf, 16);
	int nPos=__MakeCmd_MatrixVolume(m_sendBuf, nMatrix, nVolVec);
	int iRet = ISendTo(m_sendBuf, nPos);
	Sleep(100);
	return iRet;
}

//////////////////////////////////////////////////////////////////////////

MY_INLINE int ISession::ParseAll(BYTE* pBuf, int nLen)
{
	int nPos = 8;
	// 1.命令解析
	if ('B' == pBuf[3])
	{
		//设备状态处理
		if (nLen < 507)
		{
			return -1;
		}
		ParseHost(pBuf, nLen);
	}
	else
	{
		// TB6800应答
		switch (pBuf[nPos])
		{
			case CF_PROGRAM_DOWN_ENTRY:
				break;
			case CF_PROGRAM_END:
				break;
			case CF_PROGRAM_DOWN:
				break;
		case CF_GET_IP_ADDR: // 返回IP和物理地址
		case CF_SET_IP_ADDR:
			ParseIP(&pBuf[nPos+1]);
			break;
		case CF_SERIAL_NUM_RET: // 返回序列号
			ParseSerialNumAndMac(&pBuf[nPos+1]);
			break;
			// 		case CF_SET_REMOTE_CONTROL_RET: // 设置遥控数据
			// 			break;
		}
	}
	return 0;
}


MY_INLINE int ISession::ParseHost(BYTE* pBuf, int nLen)
{
	if ('B' == pBuf[3])
	{
		int nPos = 8;
		//设备状态处理
		if (nLen != 507)
		{
			return 0;
		}
		// MP3
		nPos += ParseMP3(&pBuf[nPos]); // 6*4 = 24字节
		//CD 
		nPos += ParseCD(&pBuf[nPos]); // 6*4 = 24字节
		// Radio
		nPos += ParseRadio(&pBuf[nPos]); // 6*4 = 24字节
		// 6823
		nPos += ParseAlarmCollect(&pBuf[nPos]); // 5*4 = 20字节
		nPos += 20; // 20字节 预留
		// 6807
		nPos += ParseSingleMartix(&pBuf[nPos]); // 9*8 = 72字节
		// 6812
		nPos += ParseCallingStation(&pBuf[nPos]); // 2*8 = 16字节  8个呼叫站状态(标志计数,状态)
		// 6830
		nPos += ParseAlarmActiver(&pBuf[nPos]); // 3字节  报警信号发生器状态(标志计数,播放状态,播放曲目)
		// 6816
		nPos += ParsePower(&pBuf[nPos]); // 3字节TB6816状态(标志计数,电源状态,系统状态)
		// 6811
		nPos += ParseStrongCutPowerController(&pBuf[nPos]); // 4*10 = 40字节TB6811状态(标志计数,状态1,状态2,状态3-20位)
		//新增功能：MP3选曲循环数据~~~~~~~~~~~~~~~~~~~  //6*20-4 = 116字节
		ParseMultiCycMP3(&pBuf[nPos]);
		// 6825你
		nPos = 370; // (TB6825)电话呼叫控制器,数据起始地址-370
		nPos += ParseTelephoneState(&pBuf[nPos]); // 4字节
		nPos += 5;  //life_time 4个字节，regber_count 1个字节
		//MP3_File_Name    8个字节
		ParseNameMP3(&pBuf[nPos]);
		return nPos;
	}
	return 0;
}

MY_INLINE int ISession::ParsePower(BYTE* pBuf)
{
	SPower power;
	power.bExist = pBuf[0] ? true:false;
	if (power.bExist)
	{
		for (int i=0; i<_MAX_POWER; i++)
		{
			power.bPowerOnVec[i] = (pBuf[1] & (0x01 << i)) ? false : true;
		}
		power.bAllPowerOn = (pBuf[2] & 0x10) ? true:false;
	}

	m_sys.m_power = power;

	return 3;
}

MY_INLINE int ISession::ParseMP3(BYTE* pBuf)
{
	BYTE* pUnitBuf=pBuf;

	for (int i=0; i<_MAX_MP3; i++, pUnitBuf+=6)
	{
		SMP3Unit & mp3 = m_sys.m_mp3.it[i];
		mp3.bExist = (pUnitBuf[0] & 0x03) ? true:false;
		if (m_sys.m_mp3.it[i].bExist)
		{
			mp3.nMapAudioLine = (pUnitBuf[1] & 0xE0) /32;

			m_sys.m_audioInLine.nAudioType[mp3.nMapAudioLine] = _DEV_TYPE_MP3;
			m_sys.m_audioInLine.iSubParam[mp3.nMapAudioLine] = i;

			mp3.nSongCount = pUnitBuf[2];

			switch ((pUnitBuf[1]&0x03))
			{
			case 0x00: // 停止
				mp3.bFault = false;
				mp3.iMode = PCC_STOP;
// 				if ((pUnitBuf[1] & 0x1C) == 0x18)
// 				{
// 					mp3.iMode = PCC_FILEON;
// 				}
				break;
			case 0x01: // 播放
				mp3.bFault = false;
				switch (pUnitBuf[1] & 0x1C)
				{
				case 0x04: // 循环播放·-
					mp3.iMode = PCC_LOOP;
					break;
				case 0x08: // 循环单曲
					mp3.iMode = PCC_SINGLE_CYCLE;
					break;
				case 0x0C: // （顺序）播放
					mp3.iMode = PCC_ORDER;
					break;
				case 0x10: // 播放单曲
					mp3.iMode = PCC_SINGLE;
					break;
				case 0x18://正在传送文件名
					mp3.iMode = PCC_FILEON;
				default:
					ASSERT(FALSE);
					break;
				}
				mp3.nCurSong = pUnitBuf[3];
				mp3.iProgressSecond = ((((pUnitBuf[4] & 0xF0) >> 4) * 10) + (pUnitBuf[4] & 0x0F)) * 60
					+ (((pUnitBuf[5] & 0xF0) >> 4) * 10) +  (pUnitBuf[5] & 0x0f);
				break;
			case 0x03: // 故障
				mp3.bFault = true;
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
		
	}

	return 6*_MAX_MP3;
}
//MP3新增协议功能，多曲循环
MY_INLINE int ISession::ParseMultiCycMP3(BYTE* pBuf)
{
	BYTE* pUnitBuf=pBuf;

	for (int i=0; i<_MAX_MP3; i++, pUnitBuf+= 8)
	{
		SMP3Unit& mp3 = m_sys.m_mp3.it[i];
		//是否存在
		if (mp3.bExist)
		{
			switch(pUnitBuf[0])
			{
				//多曲循环
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
				{
					int nIndex = pUnitBuf[0];
					mp3.iMode = PCC_FILEON;
					for (int j = 0;j<6;j++)
					{
						mp3.nSongCycle[j] = pUnitBuf[j+1];
					}
					if (mp3.nCurSong != mp3.nSongCycle[nIndex])
					{
						mp3.nCurSong = mp3.nSongCycle[nIndex];
					}
				}
				break;
			case 0xff:
				{

				}
				break;
			default:
				break;
			}
		}
	}
	return 370;
}
//MP3新增协议功能，文件名传送
MY_INLINE int ISession::ParseNameMP3(BYTE* pBuf)
{
	BYTE* pUnitBuf=pBuf;

	for (int i=0; i<_MAX_MP3; i++, pUnitBuf += 32)
	{
		SMP3Unit& mp3 = m_sys.m_mp3.it[i];
		//是否存在
		if (mp3.bExist)
		{
			memcpy(mp3.nCurSongName,pUnitBuf,32);
		}
	}
	return 507;
}
MY_INLINE int ISession::ParseCD(BYTE* pBuf)
{
	BYTE* pUnitBuf=pBuf;
	for (int i=0; i<_MAX_CD; i++, pUnitBuf+=6)
	{
		SCDUnit & cd = m_sys.m_cd.it[i];
		cd.bExist = (pUnitBuf[0] != 0) ? true:false;
		if (cd.bExist)
		{
			cd.nMapAudioLine = (pUnitBuf[1] & 0xE0) /32;

			m_sys.m_audioInLine.nAudioType[cd.nMapAudioLine] = _DEV_TYPE_CD;
			m_sys.m_audioInLine.iSubParam[cd.nMapAudioLine] = i;

			cd.nSongCount = pUnitBuf[2];

			switch (pUnitBuf[1] & 0x1C)
			{
			case 0x00: // 播放
				cd.iState = CDS_PLAYING;

				switch (pUnitBuf[1] & 0x03)
				{
				case 0: // （顺序）播放
					cd.iMode = PCC_ORDER;
					break;
				case 1: // 循环单曲
					cd.iMode = PCC_SINGLE_CYCLE;
					break;
				case 2: // 循环播放
					cd.iMode = PCC_LOOP;
					break;
				default:
					break;
				}

				cd.nCurSong = pUnitBuf[3];
				cd.iProgressSecond = ((((pUnitBuf[4] & 0xF0) >> 4) * 10) + (pUnitBuf[4] & 0x0F)) * 60
					+ (((pUnitBuf[5] & 0xF0) >> 4) * 10) +  (pUnitBuf[5] & 0x0f);
				break;
			case 0x04: // 停止
				cd.iState = CDS_STOP;
				break;
			case 0x10: // Open
				cd.iState = CDS_OPEN;
				break;
			case 0x0C: // Close
				cd.iState = CDS_CLOSE;
				break;
			case 0x14: // Load
				cd.iState = CDS_LOAD;
				break;
			case 0x18: // Nodisc
				cd.iState = CDS_NO_DISC;
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
	}

	return 6*_MAX_CD;
}

MY_INLINE int ISession::ParseRadio(BYTE* pBuf)
{
	BYTE* pUnitBuf=pBuf;
	for (int i=0; i<_MAX_RADIO; i++, pUnitBuf+=6)
	{
		SRadioUnit& radio = m_sys.m_radio.it[i];
		radio.bExist = (pUnitBuf[0] != 0) ? true:false;
		if (radio.bExist)
		{
			radio.nMapAudioLine = (pUnitBuf[1] & 0xE0) /32;

			m_sys.m_audioInLine.nAudioType[radio.nMapAudioLine] = _DEV_TYPE_RADIO;
			m_sys.m_audioInLine.iSubParam[radio.nMapAudioLine] = i;

			radio.bFM = ( (pUnitBuf[1] & 0x02) != 0) ? true : false;
			if ( (pUnitBuf[1] & 0x10) != 0 )
			{
				radio.bSerach = true;
				radio.iSearchMode = RSM_AUTO_SERARCH;
			} 
			else if ( (pUnitBuf[1] & 0x0C) == 0X0C )
			{
				radio.bSerach = true;
				radio.iSearchMode = RSM_UP_SEARCH;
			} 
			else if ( (pUnitBuf[1] & 0x0C) == 0X04 )
			{
				radio.bSerach = true;
				radio.iSearchMode = RSM_DOWN_SEARCH;
			}
			else
			{
				radio.bSerach = false;
			}

			if (radio.bFM)
			{
				radio.rate = pUnitBuf[2]*100 
					+ (((pUnitBuf[3] & 0xF0) >> 4) * 10) + (pUnitBuf[3] & 0x0F)
					+ float((((pUnitBuf[4] & 0xF0) >> 4) * 10) + (pUnitBuf[4] & 0x0F)) / 100;
			} 
			else
			{
				//(((Int(UDP_Data(58) / 16) * 10) + (UDP_Data(58) Mod 16)) * 100 + ((Int(UDP_Data(59) / 16) * 10) + (UDP_Data(59) Mod 16)))
				radio.rate = (float)((int)((pUnitBuf[2]/16)*10) + (int)((pUnitBuf[2] % 16) * 100) 
					+ (int)((pUnitBuf[3]/16)*10) + (int)(pUnitBuf[3] % 16));
			}
			radio.nChannel = pUnitBuf[5] + 1;
		}
	}
	return 6*_MAX_RADIO;
}

MY_INLINE int ISession::ParseCallingStation(BYTE* pBuf)
{
	BYTE* pUnitBuf=pBuf;
	for (int i=0; i<_MAX_CALLINGSTATION; i++, pUnitBuf+=2)
	{
		SCallingStationUnit& cs = m_sys.m_callingStation.it[i];
		cs.bExist = pUnitBuf[0] ? true:false;
		if (cs.bExist)
		{
			cs.iState = pUnitBuf[1];
		}
	}
	return 2*_MAX_CALLINGSTATION;
}

MY_INLINE int ISession::ParseTelephoneState(BYTE* pBuf)
{
	STelephone tel;
	tel.bExist = pBuf[0] ? true:false;
	if (tel.bExist)
	{
		tel.iState = pBuf[1]&0x0F;
		tel.bLinkOn = (pBuf[2] & 0x01) ? true : false;
		tel.bMute = (pBuf[2] & 0x08) ? true : false;
	}

	m_sys.m_telephone = tel;
	return 4;
}

MY_INLINE int ISession::ParseSingleMartix(BYTE* pBuf)
{
	int nPos = 0;
	for (int i=0; i<_SINGLE_LINE_MATRIXS; i++, nPos+=9)
	{
		if (pBuf[nPos] != 0)
		{
			m_sys.m_zone.SetState(i, true);
			for (int j=0; j<4; j++)
			{
				m_sys.m_zone.it[i*8+j*2].nLineType = pBuf[nPos+1+j] & 0x0F;
				m_sys.m_zone.it[i*8+j*2+1].nLineType = (pBuf[nPos+1+j] & 0XF0) >> 4;

				m_sys.m_zone.it[i*8+j*2].nVolume = pBuf[nPos+1+4+j] & 0x0F;
				m_sys.m_zone.it[i*8+j*2+1].nVolume = (pBuf[nPos+1+4+j] & 0XF0) >> 4;
			}
		} 
		else
		{
			m_sys.m_zone.SetState(i, false);
		}
	}
	return nPos; // = 9byte * 8
}

MY_INLINE int ISession::ParseStrongCutPowerController(BYTE* pBuf)
{
	// byte1:状态字， byte2、3、4：状态
	int nPos = 0;
	for (int i=0; i<10; i++, nPos+=4)
	{
		if (0 == pBuf[nPos])
			continue;

		for (int j=0; j<20; j++)
		{ 
			m_sys.m_zone.b6811Open[i*20+j] = (pBuf[nPos+1 + (j/8)] & (0X80 >> (j%8))) ? true : false;
		}
	}
	return nPos;
}

MY_INLINE int ISession::ParseSerialNumAndMac(BYTE* pBuf)
{
	int nPos=0;
	for (int i=0; i<6; i++)
	{
		m_sys.m_host.physicalAddr[i] = pBuf[nPos++];
	}
	m_sys.m_host.ipAddr.S_un.S_un_b.s_b1 = pBuf[nPos++];
	m_sys.m_host.ipAddr.S_un.S_un_b.s_b2 = pBuf[nPos++];
	m_sys.m_host.ipAddr.S_un.S_un_b.s_b3 = pBuf[nPos++];
	m_sys.m_host.ipAddr.S_un.S_un_b.s_b4 = pBuf[nPos++];

	for (int i=0; i<4; i++)
	{
		m_sys.m_host.serialNum[i] = pBuf[nPos]*256 + pBuf[nPos+1];
		nPos+=2;
	}
	nPos += 5;

	// 生命周期
// 	If (UDP_Data(35) And &H80) = 0 Then
// 		Life_Time = UDP_Data(34)
// 		Life_Time = Life_Time * 256 + UDP_Data(33)
// 		Life_Time = Life_Time * 256 + UDP_Data(32)
// 		Life_Time = Int(((Life_Time * 690) / 1000) / 60 / 60)
// 		Form14.Label2(1).Caption = CStr(Life_Time)
// 	Else
// 		Form14.Label2(1).Caption = "需要注册"
	if (pBuf[nPos+3] & 0x80)
	{
		m_sys.m_host.remainTime = 0;
	}
	else
	{
		m_sys.m_host.remainTime = pBuf[nPos] + pBuf[nPos+1]*256 + pBuf[nPos+2]*256*256;
		m_sys.m_host.remainTime = (((m_sys.m_host.remainTime * 690) / 1000) / 60 / 60);
	}
	nPos += 4;

	// 注册次数
// 	If UDP_Data(36) = 137 Then
// 		Form14.Label2(2).Caption = "永久注册"
// 		Form14.Text1(0).Enabled = False
// 		Form14.Text1(1).Enabled = False
// 		Form14.Text1(2).Enabled = False
// 		Form14.Text1(3).Enabled = False
// 		Form14.Command1(0).Enabled = False
// 	Else
// 		Form14.Label2(2).Caption = CStr(UDP_Data(36))
// 		Form14.Text1(0).Enabled = True
// 		Form14.Text1(1).Enabled = True
// 		Form14.Text1(2).Enabled = True
// 		Form14.Text1(3).Enabled = True
// 		Form14.Command1(0).Enabled = True
// 		End If                                                                                              '注册次数
	m_sys.m_host.regTimes = pBuf[nPos++];
	nPos+=1;
	return nPos;
}

MY_INLINE int ISession::ParseIP(BYTE* pBuf)
{
	int nPos = 0;
	for (int i=0; i<6; i++)
	{
		//m_sys.m_host.physicalAddr[i] = pBuf[nPos;
		nPos++;
	}
	m_sys.m_host.ipAddr.S_un.S_un_b.s_b1 = pBuf[nPos++];
	m_sys.m_host.ipAddr.S_un.S_un_b.s_b2 = pBuf[nPos++];
	m_sys.m_host.ipAddr.S_un.S_un_b.s_b3 = pBuf[nPos++];
	m_sys.m_host.ipAddr.S_un.S_un_b.s_b4 = pBuf[nPos++];
	return nPos;
}


MY_INLINE int ISession::ParseAlarmCollect(BYTE* pBuf)
{
	BYTE* pUnitBuf = pBuf;
	for (int i=0; i<_MAX_6823; i++, pUnitBuf+=5)
	{
		SAlarmCollectUint& alarmColl = m_sys.m_alarmCollect.it[i];
		
		alarmColl.bExist = (pUnitBuf[0] != 0) ? true : false;
		if (alarmColl.bExist)
		{

 			for (int iAct=0, iPos=1, iBit=0; iAct<_MAX_6823_ACTIVE; iAct++)
 			{
				//If ((UDP_Data(81 + Int(i / 8)) And (2 ^ (i Mod 8))) = 0) Then
				//alarmColl.bAlarm[iAct] = (pUnitBuf[iAct/8+1] & (0x01<<(iAct%8))) ? false : true;

				alarmColl.bAlarm[iAct] = (pUnitBuf[iPos] & (0x01 << iBit)) ? false : true;
				if (7 == iBit)
				{
					iBit = 0;
					iPos++;
				}
				else
				{
					iBit++;
				}
 			}
		}
	}
	return _MAX_6823*5;
}

MY_INLINE int ISession::ParseAlarmActiver(BYTE* pBuf)
{
	SAlarmActiver saa;
	saa.bExist = pBuf[0] ? true : false;
	if (saa.bExist)
	{
		saa.nSongCount = pBuf[1];
		saa.nCurSong = pBuf[2];
	}
	m_sys.m_alarmActiver = saa;
	return 3;
}






int RemoteTaskToByte(_DbTaskGroupVec& remoteVec, OUT BYTE* pBuf, int nBufLen)
{
	int nPos = 0;
	for (int i=0; i<_MAX_REMOTE_FUNC; i++)
	{
		nPos += StringToBuf(remoteVec[i].name, (BYTE*)&pBuf[nPos]);
		BYTE nSubCount = (remoteVec[i].m_taskVec.size() < _MAX_REMOTE_SUB) ? (BYTE)remoteVec[i].m_taskVec.size() : _MAX_REMOTE_SUB ;
		pBuf[nPos++] = nSubCount;
		for (int iSub=0; iSub < nSubCount; iSub++)
		{
			nPos += __TaskToBytes(&remoteVec[i].m_taskVec[iSub], &pBuf[nPos]);
		}
	}
	if (nPos > nBufLen)
	{
		ASSERT(FALSE);
	}
	return nPos;
}

int ByteToRemoteTask(_DbTaskGroupVec& remoteVec, BYTE* pBuf, int nBufLen)
{
	remoteVec.clear();
	int nPos = 0;
	for (int i=0; i<_MAX_REMOTE_FUNC; i++)
	{
		S_DB_TASK_GROUP group;
		int nNameLen = strlen((char*)&pBuf[nPos]);
		if (nPos > nBufLen)
			break;

		group.SetName((char*)&pBuf[nPos]);
		nPos += nNameLen+1;

		BYTE nSubCount = pBuf[nPos++];
		if (nSubCount >= _MAX_REMOTE_FUNC)
		{
			continue;
		}
		//nSubCount = (nSubCount < _MAX_REMOTE_SUB) ? nSubCount : 1;
		for (int iSub=0; iSub<nSubCount; iSub++)
		{
			S_DB_TASK task;
			int nTaskLen = __BytesToTask(&task, &pBuf[nPos]);
			nPos += nTaskLen;
			if (nPos > nBufLen)
				break;
			group.m_taskVec.push_back(task);
		}
		remoteVec.push_back(group);
	}
	return (int) remoteVec.size();
}