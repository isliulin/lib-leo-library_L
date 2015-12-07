#include "6800_data.h"
#pragma once

#define WM_USER_START WM_USER+200
// ����ָ����ȴ�0.5S ʧ���ظ�3��
enum CMD_FLAG{
	CF_GET_HOST_STATE = 0x04, // ��ȡ�豸״̬	��ȡʱ�ɴ������֣�����ʱ�䣬������ʽ���㲥ʱ��������ֵ

	CF_GET_IP_ADDR = 0x05, // ��ȡIP�������ַ
	CF_SET_IP_ADDR = 0x06, // ����IP��ַ

	CF_GET_SERIAL_NUM = 0x07, // ��ȡ���кź������ַ
	CF_SET_SERIAL_NUM = 0x08, // �������кź������ַ
	CF_SET_REGISTER_NUM = 0x09, // 6800ע��	
	CF_SERIAL_NUM_RET = 0x07,

	CF_QUERY_MP3_1 = 0x10, // ��ѯMP3, 0x10~13
	CF_QUERY_CD_1 = 0x14, // ��ѯCD 0x14~17 
	CF_QUERY_RADIO_1 = 0x48, // ��ѯRADIO 0x48~4b  
	CF_CONTROL_MP3_1 = 0X80, // MP3������	0x80~83	
	CF_CONTROL_CD_1 = 0x84, // CD������	0x84~87
	CF_CONTROL_RADIO_1 = 0x98, // RADIO������	0x98~9B		
	CF_CONTROL_POWER = 0x9C, // ��Դ������
	CF_CONTROL_CALLSTATION = 0xA0, // ����վ������
	CF_CONTROL_BORADCAST = 0xA1, // �㲥������	
	CF_CONTROL_PC_MEDIA = 0XFE, // PC�����������룬**

	CF_PROGRAM_DOWN_ENTRY = 0X01,	// ����������� 0x01 0x00
	CF_PROGRAM_DOWN = 0X02,	//���س���
	CF_PROGRAM_END = 0x03, //�˳����س���	0x03 0x00
	// ��ȡ����

	CF_SET_REMOTE_CONTROL = 0X0A, // ����ң������

	CF_CONTROL_ZONE_LINE_MATRIX_1 = 0X88, // ������·���� ��һ�����ܹ�8��
	CF_CONTROL_ZONE_VOLUME_MATRIX_1 = 0X90, // ������������ ��һ�����ܹ�8��


	CF_WRITE_TER_ID = 0xB0, // д���ն�ID������ʱֻ�������һ̨�ն�
	CF_WRITE_TER_ID_RET = 0x72,
	CF_GET_TER_ID = 0x71, // �����ն�ID, ���� ID=0xffff Ϊ��ָ���ն�
	CF_GET_TER_ID_RET = 0x73,
	CF_SET_TER_PARAMETER = 0xB1, // �����ն� ���� ���ն˺�
	CF_SET_TER_PARAMETER_RET = CF_WRITE_TER_ID_RET,
	CF_GET_PAGE_TER_STATE = 0X10, // ��ȡ�ն�״̬����ҳ����
	CF_GET_PAGE_TER_STATE_RET = 0X10, 
	CF_CONTROL_TER = 0xB2, // �����ն�
	CF_CONTROL_TER_RET = 0x70, 

	CF_SET_6850_LINE = 0x9D, //	6850��·����
	CF_SET_6850_VOLUME = 0x9E, // 6850��������			


	//////////////////////////////////////////////////////////////////////////
	CF_USER = 0xF0, // �û��������
	CF_SUB_USER_CMD_LOGIN = 0x01,
	CF_SUB_USER_CMD_LOGOUT,
	CF_SUB_USER_CMD_ADD,
	CF_SUB_USER_CMD_DEL,
	CF_SUB_USER_CMD_EDIT, 
	CF_SUB_USER_GET, // ��������������ϸ
	CF_SUB_USER_GET_COUNT,
	CF_SUB_USER_GET_USERS,

	CF_LINK_IP = 0XF1, // �����豸IP���� �������
	CF_SUB_LINK_IP_GET = 0X01,
	CF_SUB_LINK_IP_SET,

	CF_ALARM_MODE = 0XF2, // ����ģʽ 
	CF_SUB_ALARM_MODE_GET = 0X01,
	CF_SUB_ALARM_MODE_SET,

	CF_LINE_INFO = 0XF3, // ��·
	CF_SUB_LINE_INFO_GET = 0X01,
	CF_SUB_LINE_INFO_SET,

	CF_ZONE_INFO = 0XF4, // ����
	CF_SUB_ZONE_NAME_GET=0X01,
	CF_SUB_ZONE_NAME_SET,

	CF_TASK = 0XF5, // ��ʱ��Ŀ
	CF_SUB_GROUP_GET = 0x01, // ��ȡ��Ŀ(��������ϸ����)
	CF_SUB_GROUP_ADD, // �����Ŀ
	CF_SUB_GROUP_DEL, // ɾ��ĳ��Ŀ
	CF_SUB_GROUP_EDIT_NAME, // �޸�����
	CF_SUB_TASK_GET, // ��ȡĳ��Ŀ������
	CF_SUB_TASK_SET, // �޸�ĳ��Ŀ����
	CF_SUB_PROGRAM_CONFIG_GET, // ��ȡ��ʱ����������Ϣ
	CF_SUB_PROGRAM_CONFIG_SET, // ���ö�ʱ����������Ϣ
	CF_SUB_TASK_DOWN, // ��������
	CF_SUB_REMOTE_GET, // ң�����û�ȡ
	CF_SUB_REMOTE_SET, // ң����������

	//////////////////////////////////////////////////////////////////////////
	CF_ERR_CONNECT = 0XFE, // ͨ�Ŵ���
	CF_ERR_CMD = 0xFF, // ����������󣬷���
};

enum TASK_SET_TYPE{
	TST_NEW = 1, // �½�����
	TST_EDIT, // �޸�����
	TST_DEL, // ɾ������
	TST_DEL_ALL, //ɾ����������
};

enum MY_ERROR{
	_ERR_SUCCESS = 0, // ��ȷ
	_ERR_USER_NAME, // �û�������
	_ERR_USER_PASSWORD, // �������
	_ERR_DB_OPERATE, // ���ݿ���� 
	
	_ERR_NOT_SPECIFY, // δָ������
	
	_ERR_NOT_ACT, //
	_ERR_TIME_OUT,  // ��ʱ����Ĵ���
	_ERR_DATA_UNDISPOSE, // ָ��δ�����ָ���ʽ����ȷ
};

enum {
	_AM_MIN_RATE = 0,
	_AM_MAX_RATE = 9999/*1602*/,
	_FM_MIN_RATE = 0,
	_FM_MAX_RATE = 255,
};
// AM : �ҹ��ǲ���9KHz����Χ��531KHz-1602KHz��
enum RADIO_CTRL_CODE{
	RCC_FM_RATE=0,	// ����FMƵ��(����1=Ƶ������,����2=Ƶ��С��)
	RCC_FM_CHANNEL,	// ����FMƵ��	(����1=Ƶ��������2=0)
	RCC_AM_RATE,	// ����AMƵ��(����1=Ƶ�ʸ�2λ������2=Ƶ�ʵ�2λ)
	RCC_AM_CHANNEL,	// ����AMƵ��(����1=Ƶ��������2=0)
	RCC_SEARCH_UP=8,	// ��������(����1=0������2=0)
	RCC_SEARCH_DOWN,	// ��������(����1=0������2=0)
	RCC_SEARCH_AUTO,	// �Զ�����(����1=0������2=0)
	RCC_SAVE_CHANNEL,	// �洢��̨(����1=�洢Ƶ��������2=0)
};

//////////////////////////////////////////////////////////////////////////
BYTE __GetCmdRetByte(BYTE cmd);
UINT __GetElapseTimer(BYTE cmd);
BOOL __IsRetCmd(BYTE cmd);

//////////////////////////////////////////////////////////////////////////

// ISession command target
class ISession
{
public:
	ISession() {}
	~ISession() {}
public:
	sockaddr_in m_saTarget; // Ŀ�ĵ�ַ
	void SetTargetSockAddr(ULONG net_addr, USHORT net_port);
	BYTE			m_sendBuf[1024]; // the send buffer 
	BYTE			m_recvBuf[1024]; // the receive buffer 
	int				m_iSendLen; // the length of last send buffer 
	int				m_iRecvLen; // the length of last receive buffer

	virtual int IReceive(BYTE* pBuf, int nLen) {return ParseAll(pBuf, nLen); }
	virtual int ISendTo(BYTE* pBuf, int nLen) {m_iSendLen=nLen; return -1; }
public:
	SSystem m_sys;
	// Send Command
	//////////////////////////////////////////////////////////////////////////
	// ��������
	virtual int _EnterDownloadProgram();
	virtual int _EndDownloadProgram();
	virtual int _DownloadProgram(BYTE iDayOfWeek, _DbTaskVec& taskVec); //����һ��0��....

	virtual int _DownRemoteTask(_DbTaskGroupVec& remoteVec);
	// ��ȡ�豸״̬
	virtual int _GetHostState(BOOL bAppend=FALSE, CTime* pTime=NULL, E_ALARM_MODEL alarmMode=AM_CURRENT_ZONE, BYTE broadVolumeAdd=0);
	// IP�������ַ
	virtual int _GetIPAddr();
	virtual int _SetIPAddr(ULONG net_addr);
	// ���кź������ַ
	virtual int _GetSerialNumAndMac();
	virtual int _SetSerialNumAndMac(BYTE physicalAddr[6], IN_ADDR ipAddr, USHORT serialNum[4]);
	// 6800ע��
	virtual int _Register(USHORT registerNum[4]);

	// ��·����, ��������
	int _SetZoneLine(bool bAllZone, int nZone, BYTE nLine);
	int _SetZoneLine(bool bZoneSel[_MAX_ZONEA], BYTE nLine);
	virtual int _SetMatrixLine(int nMatrix, BYTE nLineVec[8]); // nLineVec[8]: 0~7��������·��0x0f����·����
	int _SetZoneVolume(bool bAllZone, int nZone, BYTE nVol);
	int _SetZoneVolume(bool bZoneSel[_MAX_ZONEA], BYTE nVol);
	virtual int _SetMatrixVolume(int nMatrix, BYTE nVolVec[8]); // nVolVec[]: 0~_MAX_ZONE_VOLUME; 0x0f����������

	// ��ѯ
	virtual int _QueryMP3(int nDev);
	virtual int _QueryCD(int nDev);
	virtual int _QueryRadio(int nDev);
	// ����MP3 (nDev: 0~3)
	virtual int _ControlMP3(int nDev, PLAYER_CTRL_MODE code,int nSong = 1);
	//����ѡ��ѭ�����ţ�Э���ȵ�������
	virtual int _ControlSelMultiMP3(int nDev,PLAYER_CTRL_MODE code,int* nSong);
	// ����CD (nDev: 0~3)
	virtual int _ControlCD(int nDev, PLAYER_CTRL_MODE code, int nSong = 1);
	// ����RADIO (nDev: 0~3)
	virtual int _ControlRadio(int nDev, RADIO_CTRL_CODE code, BYTE param1=0, BYTE param2=0);
	// ���Ƶ�Դ
	int _ControlPower(int iIndex, bool bOn = TRUE);
	virtual int _ControlPower(std::bitset<_MAX_POWER>& bPowerOnVec);
	// ���ƺ���վ	0xA0
	// ���ƹ㲥	0xA1

	// Parse Command
	//////////////////////////////////////////////////////////////////////////
	virtual int ParseAll(BYTE* pBuf, int nLen);
	// HOST
	virtual int ParseHost(BYTE* pBuf, int nLen);

	// IP
	virtual int ParseIP(BYTE* pBuf);
	// ���кţ�����Ϣ
	virtual int ParseSerialNumAndMac(BYTE* pBuf);

	// MP3
	virtual int ParseMP3(BYTE* pBuf);
	//MP3����Э�鹦�ܣ�����ѭ��
	virtual int ParseMultiCycMP3(BYTE* pBuf);
	//MP3����Э�鹦�ܣ��ļ�������
	virtual int ParseNameMP3(BYTE* pBuf);
	// CD
	virtual int ParseCD(BYTE* pBuf);
	// RADIO
	virtual int ParseRadio(BYTE* pBuf);
	// �����źŲɼ��� 6823
	virtual int ParseAlarmCollect(BYTE* pBuf);
	// �źž��� 6807
	virtual int ParseSingleMartix(BYTE* pBuf); //////
	// ����վ 6812
	virtual int ParseCallingStation(BYTE* pBuf);
	// �����źŷ����� 6830
	virtual int ParseAlarmActiver(BYTE* pBuf);
	// POWER
	virtual int ParsePower(BYTE* pBuf);
	// ǿ�е�Դ������ 6811
	virtual int ParseStrongCutPowerController(BYTE* pBuf);
	// �绰�ӿ�״̬ 6825
	virtual int ParseTelephoneState(BYTE* pBuf);

public:

};

// ���س���
extern int __hton_ZoneName(OUT BYTE* pBuf, int nBufLen, SZoneUnitA* pZoneUnit, int nZoneStart, int nZoneEnd);
//
extern int __ParseZoneName(BYTE* pBuf, int nLen, SZoneA& zone);

extern int __TaskToBytes(S_DB_TASK* pTask, BYTE* pStr);
extern int __BytesToTask(S_DB_TASK* pTask, BYTE* pStr);

extern int RemoteTaskToByte(_DbTaskGroupVec& remoteVec, OUT BYTE* pBuf, int nBufLen = 1000);
extern int ByteToRemoteTask(_DbTaskGroupVec& remoteVec, BYTE* pBuf, int nBufLen);
