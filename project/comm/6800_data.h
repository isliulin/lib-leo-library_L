#include <vector>
#include <map>
#include <algorithm>
#include <inaddr.h>
#include <bitset>

#pragma once


enum E_DEFINE{                  //_MAX_ZONEA = 128,Ϊ128 ������֧��16������
	_MAX_ZONEA = 64, 
	_SINGLE_LINE_MATRIXS = 8, //ע:MAX_ZONEA = 64, Ϊ64������֧��8������ 
	_MAX_POWER=8,
	_MAX_MP3=4,
	_MAX_CD=4,
	_MAX_RADIO=4,
	_MAX_CALLINGSTATION=8, // ����վ����
	_MAX_6823=4, // �����ɼ���
	_MAX_6823_ACTIVE = 30, // 6830�Ǻ�30�������ı����ɼ���

	_MAX_ZONE_VOLUME = 14, // �忨��������

	_MAX_AUDIO_LINE = 8,   //����Դ����

	_MAX_REMOTE_FUNC = 12, // ң����
	_MAX_REMOTE_SUB = 1, //ң���������
	_MAX_TASKS = 254, // ����̲���
	_TASK_TIME_AUTO_ADD = 5, // ����ʱÿ�����
	//
	_MAX_TASK_GROUP = 64, // �ݶ�100��̫��ע�����ʱsocket�ڴ滺��
	_MAX_NAME_LEN = 32,
	_MAX_PASSWORD_LEN = 16,

	_MAX_USER_ACCOUNTS = 20, //  �û����20����������ע�⴫�͵Ļ��� 
};

enum E_DEV_TYPE{
	_DEV_DEFAULT=0X00, // ��ָ��,Ĭ�� 
	_DEV_TYPE_MP3,
	_DEV_TYPE_CD,
	_DEV_TYPE_RADIO,
	_DEV_TYPE_PC_MEDIA,
	_DEV_TYPE_EXT_INPUT, //�ⲿ����

	_TYPE_ZONE_LINE,
	_TYPE_ZONE_VOLUME,

	_DEV_TYPE_MUTE_VOLUME = 0X0c,
	_DEV_TYPE_POWER = 0X20,
	_DEV_TYPE_ALARM_ACTIVER = 0X40, //

	_DEV_TYPE_ALL_ZONE = 0X100,
	_DEV_TYPE_ZONE = 0X200,
	_DEV_TYPE_TERMINAL = 0X400,

};

inline E_DEV_TYPE ByteToDevType(BYTE dt)
{
	switch (dt)
	{
	case _DEV_DEFAULT:
		return _DEV_DEFAULT;
	case _DEV_TYPE_MP3:
		return _DEV_TYPE_MP3;
	case _DEV_TYPE_CD:
		return _DEV_TYPE_CD;
	case _DEV_TYPE_RADIO:
		return _DEV_TYPE_RADIO;
	case _DEV_TYPE_PC_MEDIA:
		return _DEV_TYPE_PC_MEDIA;
	case _DEV_TYPE_EXT_INPUT:
		return _DEV_TYPE_EXT_INPUT;
	case _DEV_TYPE_POWER:
		return _DEV_TYPE_POWER;
	case _DEV_TYPE_ALARM_ACTIVER:
		return _DEV_TYPE_ALARM_ACTIVER;
	case _DEV_TYPE_MUTE_VOLUME:
		return _DEV_TYPE_MUTE_VOLUME;
	default:
//		ASSERT(FALSE);
		return _DEV_DEFAULT;
	}
}

//////////////////////////////////////////////////////////////////////////
inline int Int2Byte(UINT nInt, BYTE* pBuf)
{
	pBuf[0] = BYTE((nInt>>24) & 0xFF);
	pBuf[1] = BYTE((nInt>>16) & 0xFF);
	pBuf[2] = BYTE((nInt>>8) & 0xFF);
	pBuf[3] = BYTE((nInt) & 0xFF);
	return 4;
}

inline UINT Byte2Int(BYTE* pBuf)
{
	UINT nInt;
	nInt = pBuf[0];
	nInt = nInt << 8;
	nInt += pBuf[1];
	nInt = nInt << 8;
	nInt += pBuf[2];
	nInt = nInt << 8;
	nInt += pBuf[3];
	return nInt;
}


inline std::string CharToHexString( int nChLen, char * passwordEnc ) 
{
	std::string str;
	for (int iCh = 0; iCh < nChLen; iCh++)
	{
		char nVisualChar[3];
		if (sprintf_s(nVisualChar, 3, "%02X", (BYTE)passwordEnc[iCh]) != 2)
		{
			ASSERT(FALSE);
		}
		nVisualChar[2] = NULL;
		str += nVisualChar;
	}
	return str;
}

inline int HexStringToChar(char* pHexIn, int nLenIn, char* pOut, int nLenOut)
{
	if (nLenIn % 2 )
		return 0;
	if ((nLenOut*2) < (nLenIn))
		return 0;

	int nCount = nLenIn / 2;
	char* pSrc = pHexIn;
	int i=0;
	for (; i<nCount; i++)
	{
		int uTemp;
		if (sscanf_s(pSrc, "%02X", &uTemp) != 1)
		{
			return i;
		}
		pOut[i] = (char)uTemp;
		pSrc += 2;
	}
	return i+1;
}


// ��Դ
struct SPower //TB6816
{
	bool bExist;
	bool bAllPowerOn; // ���������״̬��־���棬��ʶ��Դȫ���򿪣�����״̬��ʾ
	std::bitset<_MAX_POWER> bPowerOnVec;

	SPower() : bExist(false) {}
};

enum PLAYER_CTRL_MODE{
	PCC_FILEON = 0x00,	//ѡ��ѭ��
	PCC_LOOP,		// ѭ������
	PCC_SINGLE_CYCLE,	// ѭ������
	PCC_ORDER,		// (˳��)����
	PCC_SINGLE,		// ���ŵ���
	PCC_STOP,		// ֹͣ
	PCC_FILEOPEN,//������----�����ڴ����ļ�
};

inline PLAYER_CTRL_MODE ByteToCtrlMode(BYTE mode)
{
	switch (mode)
	{
	case PCC_STOP:
		return PCC_STOP;
	case PCC_LOOP:
		return PCC_LOOP;
	case PCC_SINGLE_CYCLE:
		return PCC_SINGLE_CYCLE;
	case PCC_ORDER:
		return PCC_ORDER;
	case PCC_SINGLE:
		return PCC_SINGLE;
	case PCC_FILEON:
	case 0X07:
		return PCC_FILEON;
	default:
		ASSERT(FALSE);
		return PCC_STOP;
	}
}


CString PlayerCtrlModeToString(BYTE mode);
CString CDPlayerCtrlModeToString(BYTE mode);
// MP3
struct SMP3Unit //TB6829
{
	bool bExist;
	bool bFault;
	PLAYER_CTRL_MODE iMode; // ����ģʽ 
	int nSongCount;
	int nCurSong;
	int iProgressSecond; // ���Ž���
	BYTE nCurSongName[32];//���ŵ�������
	int nCurSongIndex;//����ѭ��ʱ������
	int nSongCycle[6];//����ѭ����Ŀ������1~27��
	int nMapAudioLine; // ӳ����·

	
	SMP3Unit() : bExist(false) {}
};

struct SMP3
{
	SMP3Unit it[_MAX_MP3];
};

enum CD_STATE{
	CDS_PLAYING,	// ���ţ����岥��״̬���ղ���ģʽ
	CDS_STOP,		// ֹͣ
	CDS_OPEN,		// Open
	CDS_CLOSE,		// Close
	CDS_LOAD,		// Load
	CDS_NO_DISC,	// No DISC
};

CString CDStateToString(BYTE state);

// CD
struct SCDUnit //TB6827
{
	bool bExist;
	CD_STATE iState;
	PLAYER_CTRL_MODE iMode;
	int nSongCount;
	int nCurSong;
	int iProgressSecond; // ���Ž���

	int nMapAudioLine; // ӳ����·

	SCDUnit() : bExist(false) {}
};

struct SCD
{
	SCDUnit it[_MAX_CD];
};

enum RADIO_SEARCH_MODE{
	RSM_AUTO_SERARCH,
	RSM_UP_SEARCH,
	RSM_DOWN_SEARCH,
};

CString RadioSerchModeToString(BYTE mode);

// ������
struct SRadioUnit //TB6828
{
	bool bExist;
	bool bFM; // true: FM;  false: AM
	bool bSerach; // �Ƿ�������̨
	RADIO_SEARCH_MODE iSearchMode; // ������ʽ
	float rate;
	int	nChannel;

	int nMapAudioLine; // ӳ����·

	SRadioUnit() : bExist(false) {}
};

struct SRadio //TB6828
{
	SRadioUnit it[_MAX_RADIO];
};

/*
	�绰�ӿ�״̬: STelephone::iState
	0	"����"
	1	"��������"
		"����ѡ��"
		"�㲥��"
		"���������"
		"�����ն˺�"
		"�޸�����"
		"�ٴ���������"
	8	"�޸��������"
*/

// �绰�ӿ�״̬
struct STelephone //TB6825
{
	bool bExist;
	BYTE iState;
	bool bLinkOn;
	bool bMute;

	STelephone() : bExist(false) {}
};

extern CString TelephoneStateToString(BYTE iState);

// �����źŲɼ��� 
struct SAlarmCollectUint //TB6823
{
	bool bExist;
	bool bAlarm[_MAX_6823_ACTIVE]; //

	SAlarmCollectUint() : bExist(false) {}
};

struct SAlarmCollect
{
	SAlarmCollectUint it[_MAX_6823];
};

// �����źŷ�����
struct SAlarmActiver//TB6830
{
	bool bExist;
	int nSongCount;
	int nCurSong;

	SAlarmActiver() : bExist(false) {}
};

// ����վ
struct SCallingStationUnit //TB6812
{
	bool bExist;
	BYTE iState;// bit - 0: �������б�־0�� true ���ں��� false ֹͣ
				//		 1: �������б�־1 ...
				//		 2: �㲥��־ ...
				//		 3: �㲥��·æ��־ ...

	SCallingStationUnit() : bExist(false) {}
};

CString CallingStationStateToString(BYTE state);

struct  SCallingStation
{
	SCallingStationUnit it[_MAX_CALLINGSTATION];
};

enum E_LINE_TYPE{
	LT_Line1=0, // "Line1"
	LT_Line8=7, // "Line8"
	LT_Alarm_BD, // "Alarm BD"
	LT_Broad1, // "Broad1"
	LT_Fire, // "Fire"
	LT_Empty, // "Empty"
	LT_Error12, // "Error12"
	LT_Error13, // "Error13"
	LT_Error14,
	LT_Error15,
};

struct SZoneUnitA 
{
	char name[_MAX_NAME_LEN+1];
	bool bOnline;
	BYTE nLineType; // E_LINE_TYPE
	BYTE nVolume; // ����

	SZoneUnitA() : bOnline(false) {
		name[0] = NULL;
	}
};

struct SZoneA 
{
	// 6807 
	SZoneUnitA it[_MAX_ZONEA]; // ��������
	// 6811
	bool b6811Open[200 ]; // 10*20 ǿ�е�Դ������

	void SetState(int iCard, bool bExit)
	{
		ASSERT(iCard>=0 && iCard<_SINGLE_LINE_MATRIXS);
		for (int i=0, j=iCard*8; i<8; i++, j++)
		{
			it[j].bOnline = bExit;
		}
	}
};

CString DevToString(UINT uDevType);
CString DevToString(UINT uDevType, UINT nID=0);

struct  SAudioInLineConfig
{
	char name[_MAX_AUDIO_LINE][_MAX_NAME_LEN+1];
	E_DEV_TYPE nAudioType[_MAX_AUDIO_LINE];
	int iSubParam[_MAX_AUDIO_LINE];

	SAudioInLineConfig() {
		for (int i=0; i<_MAX_AUDIO_LINE; i++)
		{
			name[i][0] = NULL;
			nAudioType[i] = _DEV_DEFAULT;
			iSubParam[i] = 0;
		}
	}
};

// ��Ƶ�����ź�
struct  SAudioInLine
{
	E_DEV_TYPE nAudioType[15];
	int iSubParam[15];
	SAudioInLineConfig config;

	SAudioInLine() {
		for (int i=0; i<15; i++)
		{
			nAudioType[i] = _DEV_DEFAULT;
			iSubParam[i] = 0;
		}
	}

	CString LineTypeToString(int nLineType);
};

inline CString VolumeToString(int iVol)
{
	CString str;
	str.Format(_T("%d"), iVol);
	return str;
}




// ע�����Ϊ REG_PERMANET ʱ��Ϊ����ע��
#define _REG_PERMANET 137
struct SHost //TB6800 
{
	BYTE physicalAddr[6]; // MAC��ַ
	IN_ADDR ipAddr;	// IP��ַ
	USHORT serialNum[4]; // ���к�
	UINT remainTime; // ��������, ʣ��ʱ��
	BYTE regTimes; // ע�����
	USHORT registerData[4]; // ע������

	SHost() {
		memset(this, 0, sizeof(SHost));
	}
};

enum E_ALARM_MODEL{
	AM_CURRENT_ZONE,// "��������"
	AM_NEIGHBOR_1,	// "����һ��"
	AM_NEIGHBOR_2,	// "���ڶ���"
	AM_NEIGHBOR_3,	// "��������"
	AM_NEIGHBOR_4,	// "��������"
	AM_ALL_ZONE,	// "ȫ������"
};

inline E_ALARM_MODEL ByteToAlarmModel(BYTE am)
{
	switch (am)
	{
	case AM_CURRENT_ZONE:
		return AM_CURRENT_ZONE;
	case AM_NEIGHBOR_1:
		return AM_NEIGHBOR_1;
	case AM_NEIGHBOR_2:
		return AM_NEIGHBOR_2;
	case AM_NEIGHBOR_3:
		return AM_NEIGHBOR_3;
	case AM_NEIGHBOR_4:
		return AM_NEIGHBOR_4;
	case AM_ALL_ZONE:
		return AM_ALL_ZONE;
	default:
		ASSERT(FALSE);
		return AM_CURRENT_ZONE;
	}
}

struct SSystem
{
	SHost	m_host;
	E_ALARM_MODEL alarmMode;
	BYTE broadVolumeAdd;	// �㲥ʱ��������ֵ(Record-90)
 	
	SZoneA m_zone;

	SPower m_power;
	SMP3	m_mp3;
	SCD		m_cd;
	SRadio m_radio;
	// �����źŲɼ��� 
	SAlarmCollect m_alarmCollect;
	// �����źŷ�����
	SAlarmActiver m_alarmActiver;
	// ����վ������
	SCallingStation m_callingStation;
	// �绰�ӿ�״̬
	STelephone	m_telephone;
	// ������Ƶ����
	SAudioInLine m_audioInLine;


	int GetAudioInLine(int nAudioDev, int nAudioDevAddr)
	{
		if (nAudioDevAddr <0 || nAudioDevAddr >= 4)
		{
			ASSERT(0);
			return -1;
		}
		switch (nAudioDev)
		{
		case _DEV_TYPE_MP3:
			return m_mp3.it[nAudioDevAddr].nMapAudioLine;
			break;
		case _DEV_TYPE_CD:
			return m_cd.it[nAudioDevAddr].nMapAudioLine;
			break;
		case _DEV_TYPE_RADIO:
			return m_radio.it[nAudioDevAddr].nMapAudioLine;
			break;
		case _DEV_TYPE_PC_MEDIA:
			ASSERT(FALSE);
			return -1;
		default:
			ASSERT(FALSE);
			break;
		}
		return -1;
	}
};




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// �ַ���ת���� CW2T, CA2T, CT2A, CT2W
// CT2A(strPath, CP_ACP)
// CString str = CA2T(pXmlChild->GetText(), CP_ACP �� CP_UTF8); 
inline const char* UTF8_to_ANSI( const char * pszUTF8) 
{
	static std::string str;
	str = CW2A( CA2W(pszUTF8, CP_UTF8) );
	return str.c_str();
}

inline const char* ANSI_to_UTF8(const char * pszANSI)
{
	static std::string str;
	str = CW2A(CA2W(pszANSI), CP_UTF8);
	return 	str.c_str();
}

enum E_USER_TYPE{
	_UT_NULL = 0,
	_UT_RIGHT_CONTROL = 0X01, // ��������Ȩ��
	_UT_RIGHT_PROGRAM = 0X02, // ����༭Ȩ��
	_UT_RIGHT_REMOTE = 0X04, // ң��Ȩ��
	_UT_RIGHT_CONFIG = 0X08, // ����
	_UT_RIGHT_USER = 0X10, // �û�����
	_UT_RIGHT_LOG = 0X20, // ��־
	_UT_ADMIN = 0xFFFFFF, // �����û�
	_UT_DEFAUT = _UT_RIGHT_CONTROL | _UT_RIGHT_PROGRAM | _UT_RIGHT_REMOTE,
};

CString UserTypeToString(UINT ut);

struct S_DB_USER
{
	char name[_MAX_NAME_LEN+1];
	char password[_MAX_PASSWORD_LEN+1];
	UINT type;
	std::bitset<_MAX_ZONEA> bZoneVec; // ����Ȩ��
	//
	S_DB_USER (){
		LoadDefault();
	}

	void LoadDefault(){
		ZeroMemory(name, _MAX_NAME_LEN+1);
		ZeroMemory(password, _MAX_PASSWORD_LEN+1);
		type = _UT_NULL;
		bZoneVec.reset();
	}

	void SetName(const CString strName){
		CT2A pszName(strName);
		SetName(pszName);
	}

	void SetName(const char* pszName){
		int nLen = strlen(pszName);
		if (0 == nLen)
		{
			ASSERT(FALSE);
			return;
		} 
		else if (nLen > _MAX_NAME_LEN)
		{
			ASSERT(FALSE);
			nLen = _MAX_NAME_LEN;
		}
		strcpy_s(this->name, _MAX_NAME_LEN+1, pszName);
	}

	void SetPassword(CString strPwd){
		CT2A pszPassword(strPwd);
		SetPassword(pszPassword);
	}

	void SetPassword(char* pszPwd){
		int nLen = strlen(pszPwd);
		if (0 == nLen)
		{
// 			ASSERT(FALSE);
// 			return;
		} 
		else if (nLen > _MAX_PASSWORD_LEN)
		{
// 			ASSERT(FALSE);
			nLen = _MAX_PASSWORD_LEN;
		}
		memcpy_s(this->password, _MAX_PASSWORD_LEN+1, pszPwd, nLen);
		this->password[nLen] = NULL;
	}

};


inline int StringToBuf(IN char* pSz, OUT BYTE* pBuf, int nMaxLen=_MAX_NAME_LEN)
{
	int nLen = strlen(pSz);
	if (nLen > nMaxLen)
	{
		ASSERT(FALSE);
		nLen = _MAX_NAME_LEN;
	}
	strcpy_s((char*)pBuf, nMaxLen, pSz);
	pBuf[nLen+1] = NULL;
	return nLen+1;
}

inline int BufToString(OUT char* pSz, IN BYTE* pBuf, int nMaxLen=_MAX_NAME_LEN)
{
	if (strcpy_s(pSz, nMaxLen, (char*)pBuf) != 0)
	{
		pSz[0] = NULL;
		return 1;
	}
	else
	{
		return strlen(pSz)+1;
	}
}

inline int StringToBuf(CString& str, BYTE* pBuf, int nMaxLen=_MAX_NAME_LEN)
{
	CT2A pszName(str);
	return StringToBuf(pszName, pBuf, nMaxLen);
}

inline int __CompressSelectZoneToBytes(const std::bitset<_MAX_ZONEA>& zoneSelect, BYTE* pStr/*=NULL*/)
{
	ASSERT(pStr);
	int byte_count = _MAX_ZONEA / 8 + ((_MAX_ZONEA % 8) ? 1 : 0);
	memcpy_s(pStr, byte_count, &zoneSelect, byte_count);
	return byte_count;
}

inline int __DecompressBytesToSelectZone(std::bitset<_MAX_ZONEA>& zoneSelect, BYTE* pStr/*=NULL*/)
{
	ASSERT(pStr);
	int byte_count = _MAX_ZONEA / 8 + ((_MAX_ZONEA % 8) ? 1 : 0);
	memcpy_s(&zoneSelect, byte_count, pStr, byte_count);
	//memcpy_s(&zoneSelect, 8, pStr, 8);
	return byte_count;
	//return 8;
}

#include "MyEncrypt.h"

inline int UserToByte(S_DB_USER& user, BYTE* pBuf, bool bPassword = true)
{
	int nPos = 0;
	nPos += StringToBuf(user.name, pBuf);
	
	if (bPassword)
	{
		CMyEncrypt myEnc;
		myEnc._Encrypt((char*)&pBuf[nPos], user.password, _MAX_PASSWORD_LEN);
		nPos += _MAX_PASSWORD_LEN;

//		nPos += StringToBuf(user.password, &pBuf[nPos]);
	}

	nPos += Int2Byte(user.type, &pBuf[nPos]);

	nPos += __CompressSelectZoneToBytes(user.bZoneVec, &pBuf[nPos]);
	return nPos;
}

inline int ParseByteToUser(S_DB_USER& user, BYTE* pBuf, bool bPassword = true)
{
	int nPos=0;
	nPos += BufToString(user.name, &pBuf[nPos]);

	if (bPassword)
	{
		CMyEncrypt myEnc;
		myEnc._Decrypt(user.password, (char*)&pBuf[nPos], _MAX_PASSWORD_LEN);
		user.password[_MAX_PASSWORD_LEN] = NULL;
		nPos += _MAX_PASSWORD_LEN;

//		nPos += BufToString(user.password, &pBuf[nPos], _MAX_PASSWORD_LEN+1);
	}

	user.type = Byte2Int(&pBuf[nPos]);
	nPos+=4;

	nPos += __DecompressBytesToSelectZone(user.bZoneVec, &pBuf[nPos]);
	return nPos;
}

typedef std::vector<S_DB_USER> _DbUserVec;

struct S_DB_USER_VEC 
{
	_DbUserVec m_userVec;

	bool FindUser(char name[_MAX_NAME_LEN+1], _DbUserVec::iterator& iter);
	bool SetUser(S_DB_USER& user);
	bool AddUser(S_DB_USER& user);
	bool DelUser(char name[_MAX_NAME_LEN+1]);
};

inline bool S_DB_USER_VEC::FindUser(char name[_MAX_NAME_LEN+1], _DbUserVec::iterator& iter)
{

	for (iter = m_userVec.begin(); iter!=m_userVec.end(); ++iter)
	{
		if (strlen(name) == strlen(iter->name))
		{
			if (memcmp(name, iter->name, strlen(name)) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

inline bool S_DB_USER_VEC::SetUser(S_DB_USER& user)
{
	_DbUserVec::iterator iter;
	if (FindUser(user.name, iter))
	{
		*iter = user;
		return true;
	}
	else
	{
		return false;
	}
}

inline bool S_DB_USER_VEC::AddUser(S_DB_USER& user)
{
	_DbUserVec::iterator iter;
	if (! FindUser(user.name, iter))
	{
		m_userVec.push_back(user);
		return true;
	}
	else
	{
		return false;
	}
}

inline bool S_DB_USER_VEC::DelUser(char name[_MAX_NAME_LEN+1])
{
	_DbUserVec::iterator iter;
	if (FindUser(name, iter))
	{
		m_userVec.erase(iter);
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////


inline void Byte2BitSet(BYTE contenxt, OUT std::bitset<8>& bitSet)
{
	for (int i=0; i<_MAX_POWER; i++)
	{
		bitSet.set(i, (contenxt & (0X01<<i)) ? true:false);
	}
}

inline void ParseTaskSecond(UINT nTimeSec, BYTE& nHour, BYTE& nMin, BYTE& nSec)
{
	nHour = (BYTE) (nTimeSec / 3600);
	nMin = (BYTE) ((nTimeSec - nHour*3600) / 60);
	nSec = (BYTE) (nTimeSec % 60);
}

inline CTime ParseTaskSecond(UINT nTimeSec)
{
	CTime time = CTime::GetCurrentTime();
	int nHour = nTimeSec / 3600;
	int nMin = (nTimeSec - nHour*3600) / 60;
	int nSec = nTimeSec % 60;
	return CTime(time.GetYear(), time.GetMonth(), time.GetDay(), nHour, nMin, nSec);
}

inline UINT MakeTaskSecond(int nHour, int nMin, int nSec)
{
	return nHour*3600 + nMin*60 + nSec;
}

inline UINT MakeTaskSecond(CTime& time)
{
	return MakeTaskSecond(time.GetHour(), time.GetMinute(), time.GetSecond());
}

//////////////////////////////////////////////////////////////////////////
struct S_DB_TASK
{
	//BOOL bEnable;
	UINT nTimeSec; // ʱ�䣬���� ��24*3600
	char name[_MAX_NAME_LEN+1];
	BYTE devType; // E_DEV_TYPE

	// power
	std::bitset<_MAX_POWER> bPowerOnVec;
	// mp3 , cd
	BYTE nDev;
	BYTE playMode; // PLAYER_CTRL_MODE
	BYTE song;
	BYTE multisong[6]; //ѡ��ѭ������
	// radio
	BYTE is_FM; // true: FM;  false: AM
	BYTE is_channel; // true: channel false: rate
	float rate;
	BYTE nChannel;
	// line , volume
	BYTE nLine; // ��·
	BYTE nVolume; // ����ֵ
	std::bitset<_MAX_ZONEA> bSelZoneVec;

	S_DB_TASK() : nTimeSec(0), devType(_DEV_TYPE_POWER),
		nDev(0), playMode(PCC_ORDER), song(1), // mp3 , cd
		is_FM(TRUE), is_channel(TRUE), rate(600), nChannel(1),// radio
		nLine(0), nVolume(0) // line , volume
	{
		name[0] = NULL;
	}

	S_DB_TASK& operator= (const S_DB_TASK& it2)
	{
		memcpy(this, &it2, sizeof(S_DB_TASK));
		return *this;
	}

	void SetName(const CString strName){
		CT2A pszName(strName);
		SetName(pszName);
	}

	void SetName(const char* pszName){
		int nLen = strlen(pszName);
		if (0 == nLen)
		{
			//ASSERT(FALSE);
			return;
		} 
		else if (nLen > _MAX_NAME_LEN)
		{
			//ASSERT(FALSE);
			nLen = _MAX_NAME_LEN;
		}
		strcpy_s(this->name, _MAX_NAME_LEN+1, pszName);
	}

	bool IsInvalid()
	{
		if (nTimeSec >= (24*3600))
		{
			return false;
		}

		if (_DEV_TYPE_POWER == devType)
		{
		}
		else if (_DEV_TYPE_MP3 == devType)
		{
		}
		else if (_DEV_TYPE_CD == devType)
		{
		}
		else if (_DEV_TYPE_RADIO == devType)
		{
		}
		else if (_DEV_TYPE_PC_MEDIA == devType)
		{
		}
		else if (_TYPE_ZONE_LINE == devType)
		{
		}
		else if (_TYPE_ZONE_VOLUME == devType)
		{
		}
		else
		{
			return false;
		}
		return true;
	}
};

typedef std::vector<S_DB_TASK> _DbTaskVec;

inline bool isEarlier(const S_DB_TASK& s1, const S_DB_TASK& s2)
{
	return s1.nTimeSec < s2.nTimeSec;
}
//std::stable_sort(x.begin(), x.end(), isEarlier);

struct S_DB_TASK_GROUP
{
	char name[_MAX_NAME_LEN+1];
	_DbTaskVec m_taskVec;

	S_DB_TASK_GROUP(){
		name[0] = NULL;
	}

	void SortTask(){
		std::stable_sort(m_taskVec.begin(), m_taskVec.end(), isEarlier);
	}

	void SetName(const CString strName){
		CT2A pszName(strName);
		SetName(pszName);
	}

	void SetName(const char* pszName){
		int nLen = strlen(pszName);
// 		if (0 == nLen)
// 		{
// 			ASSERT(FALSE);
// 			return;
//		} 
		/*else */if (nLen > _MAX_NAME_LEN)
		{
			ASSERT(FALSE);
			nLen = _MAX_NAME_LEN;
		}
		strcpy_s(this->name, _MAX_NAME_LEN+1, pszName);
	}

	S_DB_TASK_GROUP& operator= (const S_DB_TASK_GROUP& it2)
	{
		memcpy(this->name, it2.name, _MAX_NAME_LEN+1);

		m_taskVec.clear();
		for (_DbTaskVec::const_iterator iter = it2.m_taskVec.begin(); iter != it2.m_taskVec.end(); iter++)
		{
			m_taskVec.push_back(*iter);
		}

		return *this;
	}
};

typedef std::vector<S_DB_TASK_GROUP> _DbTaskGroupVec;

inline _DbTaskVec::iterator FindTask(_DbTaskVec& taskVec, UINT nTimeSec)
{
	for (_DbTaskVec::iterator iter = taskVec.begin(); iter != taskVec.end(); iter++)
	{
		if (iter->nTimeSec == nTimeSec)
		{
			return iter;
		}
	}
	return taskVec.end();
}


inline _DbTaskGroupVec::iterator FindGroup(_DbTaskGroupVec& gv, char* pName)
{
	for (_DbTaskGroupVec::iterator iter = gv.begin(); iter != gv.end();	iter++)
	{
		if (strcmp(pName, iter->name)==0)
		{
			return iter;
		}
	}
	return gv.end();
}

inline _DbTaskGroupVec::iterator FindGroup(_DbTaskGroupVec& gv, CString& name)
{
	return FindGroup(gv, CT2A(name));
}


CString ProgramTaskToString(S_DB_TASK& task, bool bAppendName = true);
//////////////////////////////////////////////////////////////////////////
// 
// enum USER_LOG_TYPE{
// 	ULT_USER_LOGIN = 100,
// 	ULT_USER_LOGOUT,
// 	ULT_USER_NEW,
// 	ULT_USER_DEL,
// 	ULT_CLEAR_HOST_LOG,
// 	ULT_MODIFY_DEV_CONFIG,
// 	ULT_SPEAK, // PC��˷�Ѱ��
// 	ULT_CLEAR_USER_RECORD,
// };
// 
// struct S_DB_USER_EVENT{
// 	LONG ID;
// 	COleDateTime eventTime;
// 	CString user;
// 	long userType;
// 	LONG logFlag;
// 	CString szMsg;
// 
// 	S_DB_USER_EVENT& operator = (const S_DB_USER_EVENT& item2){
// 		ID = item2.ID;
// 		eventTime = item2.eventTime;
// 		user = item2.user;
// 		userType = item2.userType;
// 		logFlag = item2.logFlag;
// 		szMsg = item2.szMsg;
// 	}
// 
// 	CString DateTimeToString(COleDateTime& odt)
// 	{
// 		return odt.Format(_T("%Y/%m/%d %H:%M:%S"));
// 	}
// 
// 	S_DB_USER_EVENT():ID(0){;}
// };
