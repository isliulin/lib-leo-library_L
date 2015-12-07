#include "stdafx.h"
#include "6800_data.h"

CString PlayerCtrlModeToString(BYTE mode)
{
	switch (mode)
	{
	case PCC_STOP:
		return _T("ֹͣ");
	case PCC_LOOP:
		return _T("ѭ������");
	case PCC_SINGLE_CYCLE:
		return _T("ѭ������");
	case PCC_ORDER:
		return _T("˳�򲥷�");
	case PCC_SINGLE:
		return _T("���ŵ���");
	case  PCC_FILEON:
	case 0x07:
		return _T("ѡ��ѭ��");
	case PCC_FILEOPEN:
		return _T("�����ļ���");
	default:
		ASSERT(FALSE);
		return _T("");
	}
}
CString CDPlayerCtrlModeToString(BYTE mode)
{
	switch (mode)
	{
	case PCC_STOP:
		return _T("ֹͣ");
	case PCC_LOOP:
		return _T("ѭ������");
	case PCC_SINGLE_CYCLE:
		return _T("ѭ������");
	case PCC_ORDER:
		return _T("˳��/����");
	case PCC_SINGLE:
		return _T("˳��/����");    // ��ΪCD��û�е�������״̬���أ��������ڰ�����˳�򲥷�һ��ϲ�
	default:
		ASSERT(FALSE);
		return _T("");
	}
}
CString CDStateToString(BYTE state)
{
	switch (state)
	{
	case CDS_PLAYING:
		ASSERT(0);
		return _T("����");
	case CDS_STOP:
		return _T("ֹͣ");
	case CDS_OPEN:
		return _T("Open");
	case CDS_CLOSE:
		return _T("Close");
	case CDS_LOAD:
		return _T("Load");
	case CDS_NO_DISC:
		return _T("No DISC");
	default:
		ASSERT(0);
		return _T("");
	}
}

CString RadioSerchModeToString(BYTE mode)
{
	switch (mode)
	{
	case RSM_AUTO_SERARCH:
		return _T("Auto Search");
	case RSM_UP_SEARCH:
		return _T("UP Search");
	case RSM_DOWN_SEARCH:
		return _T("Down Search");
	default:
		return _T("");
	}
}


CString TelephoneStateToString(BYTE iState)
{
	switch (iState)
	{
	case 0:
		return _T("����");
	case 1:
		return _T("��������");
	case 2:
		return _T("����ѡ��");
	case 3:
		return _T("�㲥��");
	case 4:
		return _T("���������");
	case 5:
		return _T("�����ն˺�");
	case 6:
		return _T("�޸�����");
	case 7:
		return _T("�ٴ���������");
	case 8:
		return _T("�޸��������");
	default:
		ASSERT(0);
		return _T("");
	}
}

CString CallingStationStateToString(BYTE state)
{
// 	if (state & 0x03)
// 	{
// 		// �������б�־
// 		return _T("��������");
// 	} 
// 	else 
	if (state & 0x04)
	{
		return _T("�㲥");
	}
	else if (state & 0x08)
	{
		// �㲥��·æ��־
		return _T("��·æ");
	} 
	else
	{
		return _T("����");
	}
}

CString DevToString(UINT uDevType)
{
	CString szTmp;
	switch (uDevType)
	{
	case _DEV_TYPE_MP3:
		szTmp = _T("MP3");
		break;
	case _DEV_TYPE_CD:
		szTmp = _T("CD");
		break;
	case _DEV_TYPE_RADIO:
		szTmp = _T("Radio");
		break;
	case _DEV_TYPE_PC_MEDIA:
		szTmp = _T("PC������");
		break;
	case _DEV_TYPE_EXT_INPUT:
		szTmp = _T("�ⲿ����");
		break;
	case _DEV_TYPE_POWER:
		szTmp = _T("��Դ");
		break;
	case _TYPE_ZONE_LINE:
		szTmp = _T("��·");
		break;
	case _TYPE_ZONE_VOLUME:
		szTmp = _T("����");
		break;
	default:
		//ASSERT(FALSE);
		break;
	}
	return szTmp;
}

CString DevToString(UINT uDevType, UINT nID/*=0*/)
{
	CString stTemp;
	switch (uDevType)
	{
	case _DEV_TYPE_MP3:
		stTemp.Format(_T("%d��MP3"), nID+1);
		break;
	case _DEV_TYPE_CD:
		stTemp.Format(_T("%d��CD"), nID+1);
		break;
	case _DEV_TYPE_RADIO:
		stTemp.Format(_T("%d��Radio"), nID+1);
		break;
	case _DEV_TYPE_PC_MEDIA:
		stTemp = _T("PC������");
		break;
	case _DEV_TYPE_EXT_INPUT:
		stTemp = _T("�ⲿ����");
		break;
	case _DEV_TYPE_POWER:
		stTemp = _T("��Դ");
		break;
	case _TYPE_ZONE_LINE:
		stTemp.Format(_T("��·%d"), nID+1);
		break;
	case _TYPE_ZONE_VOLUME:
		stTemp.Format(_T("����%d"), nID);
		break;
	default:
		//ASSERT(FALSE);
		break;
	}
	return stTemp;
}


CString UserTypeToString(UINT ut)
{
	CString str;
	if (ut == _UT_ADMIN)
	{
		return _T("��������Ա");
	}
	else
	{
		if (ut & _UT_RIGHT_CONTROL)
		{
			str += _T("��������");
		}
		if (ut & _UT_RIGHT_PROGRAM)
		{
			if (str.GetLength())
			{
				str += _T(" | ");
			}
			str += _T("���");
		}
		if (ut & _UT_RIGHT_REMOTE)
		{
			if (str.GetLength())
			{
				str += _T(" | ");
			}
			str += _T("ң������");
		}
		if (ut & _UT_RIGHT_CONFIG)
		{
			if (str.GetLength())
			{
				str += _T(" | ");
			}
			str += _T("ϵͳ����");
		}
		if (ut & _UT_RIGHT_USER)
		{
			if (str.GetLength())
			{
				str += _T(" | ");
			}
			str += _T("�û�����");
		}
// 		if (ut & _UT_RIGHT_LOG)
// 		{
// 			if (str.GetLength())
// 			{
// 				str += _T(" | ");
// 			}
// 			str += _T("��־");
// 		}
	}
	return str;
}

CString ProgramTaskToString(S_DB_TASK& task, bool bAppendName/* = true*/)
{
	CString str;
	CString szTmp;
	if (bAppendName)
	{
		BYTE nHour, nMinute, nSec;
		ParseTaskSecond(task.nTimeSec, nHour, nMinute, nSec);
		szTmp.Format(_T("%02d:%02d:%02d"), nHour, nMinute, nSec);
		str += szTmp;
		//
		str += _T("(");
		str += CA2T(task.name);
		str += _T("):");
	}
	//
	switch (task.devType)
	{
	case _DEV_DEFAULT:
		str += _T("��");
		return str;
		break;
	case _TYPE_ZONE_LINE:
		szTmp = DevToString(task.devType, task.nLine);
		break;
	case _TYPE_ZONE_VOLUME:
		szTmp = DevToString(task.devType, task.nVolume);
		break;
	default:
		szTmp = DevToString(task.devType, task.nDev);
		break;
	}
	str += _T("����");
	str += szTmp;
	//
	//
	str += _T("��");
	szTmp = _T("");
	switch (task.devType)
	{
	case _DEV_TYPE_POWER:
		for (int i=0; i<_MAX_POWER; i++)
		{
			szTmp.Format(_T("%d"), i+1);
			str += szTmp + (task.bPowerOnVec[i] ? _T("(��) ") : _T("(��) "));
		}
		break;
	case _DEV_TYPE_MP3:
		str += PlayerCtrlModeToString(task.playMode);
		if (task.playMode != PCC_STOP)
		{
			if (task.playMode == PCC_FILEON ||task.playMode == 0x07)
			{
				szTmp.Format(_T(" ��Ŀ%d,%d,%d,%d,%d,%d"),task.multisong[0],task.multisong[1],task.multisong[2],task.multisong[3],task.multisong[4],task.multisong[5]);
			}
			else
			szTmp.Format(_T(" ��Ŀ%d"), task.song);
			str += szTmp;
		}
		break;
	case _DEV_TYPE_CD:
		str += CDPlayerCtrlModeToString(task.playMode);
		if (task.playMode != PCC_STOP)
		{
			szTmp.Format(_T(" ��Ŀ%d"), task.song);
			str += szTmp;
		}
		break;
	case _DEV_TYPE_RADIO:
		//str += _T("����");
		str += task.is_FM ? _T("FM") : _T("AM");
		str += task.is_channel ? _T("Ƶ��") : _T("Ƶ��");
		if (task.is_channel)
		{
			szTmp.Format(_T("%d"), task.nChannel);
		} 
		else
		{
			if (task.is_FM)
			{
				szTmp.Format(_T("%0.2f MHz"), task.rate);
			} 
			else
			{
				szTmp.Format(_T("%d KHz"), (int)task.rate);
			}
		}
		str += szTmp;
		break;
	case _TYPE_ZONE_LINE:
	case _TYPE_ZONE_VOLUME:
		str += _T("����");
		for (int i=0, j=0; i<_MAX_ZONEA; i++)
		{
			if (task.bSelZoneVec[i])
			{
				szTmp.Format(_T("%d"), i+1);
				if (j == 0)
				{
					j++;
				} 
				else
				{
					str += _T(",");
				}
				str += szTmp;
			}
		}
		break;
	case _DEV_TYPE_PC_MEDIA:
		break;
	default:
		//ASSERT(FALSE);
		break;
	}
	return str;
}



CString SAudioInLine::LineTypeToString( int nLineType )
{
	if (nLineType <0 || nLineType > 15)
	{
		ASSERT(0);
		return _T("");
	}

	if (nLineType>=LT_Line1 && nLineType <= LT_Line8)
	{
		CString strLine = StringPool::Get(GIDS_LINE_START + nLineType);
		CString strDev = DevToString(nAudioType[nLineType], iSubParam[nLineType]);
		CString strConfig = DevToString(config.nAudioType[nLineType], config.iSubParam[nLineType]);
		if (strDev.GetLength() > 0 || strConfig.GetLength() > 0)
		{
			strLine += _T("��");
			if (strDev.GetLength() > 0)
			{
				strLine += strDev;
			} 
			else
			{
				strLine += _T("(") + strConfig + _T(")");
			}
		}

// 		CString strLine = CA2T(config.name[nLineType], CP_ACP/*CP_UTF8*/);
// 		if (strLine.GetLength())
// 		{
// 			strLine += _T(" ");
// 		}
// 		CString str = DevToString(config.nAudioType[nLineType], config.iSubParam[nLineType]);
// 		if (str.GetLength())
// 		{
// 			if (strLine.GetLength())
// 			{
// 				strLine += _T(" - ");
// 			}
// 			strLine += str;
// 		}
// 		strLine += _T("  (");
// 		str = DevToString(nAudioType[nLineType], iSubParam[nLineType]);
// 		if (str.GetLength() == 0)
// 		{
// 			str += StringPool::Get(GIDS_LINE_START + nLineType);
// 		}
// 		strLine += str;
// 		strLine += _T(")");
		return strLine;
	} 
	else if (nLineType == 0x0c)
	{
		return _T("�ӵ�--����");
	}
	else if (nLineType>=LT_Alarm_BD && nLineType<=LT_Error15)
	{
		return StringPool::Get(GIDS_LINE_START + nLineType);
	}
	else
	{
		ASSERT(0);
		return _T("");
	}
}
