#include "6800_data.h"
#pragma once

#define WM_USER_START WM_USER+200
// 返回指令，最多等待0.5S 失败重复3次
enum CMD_FLAG{
	CF_GET_HOST_STATE = 0x04, // 获取设备状态	获取时可带控制字，设置时间，报警方式，广播时音量增加值

	CF_GET_IP_ADDR = 0x05, // 获取IP和物理地址
	CF_SET_IP_ADDR = 0x06, // 设置IP地址

	CF_GET_SERIAL_NUM = 0x07, // 获取序列号和物理地址
	CF_SET_SERIAL_NUM = 0x08, // 设置序列号和物理地址
	CF_SET_REGISTER_NUM = 0x09, // 6800注册	
	CF_SERIAL_NUM_RET = 0x07,

	CF_QUERY_MP3_1 = 0x10, // 查询MP3, 0x10~13
	CF_QUERY_CD_1 = 0x14, // 查询CD 0x14~17 
	CF_QUERY_RADIO_1 = 0x48, // 查询RADIO 0x48~4b  
	CF_CONTROL_MP3_1 = 0X80, // MP3控制码	0x80~83	
	CF_CONTROL_CD_1 = 0x84, // CD控制码	0x84~87
	CF_CONTROL_RADIO_1 = 0x98, // RADIO控制码	0x98~9B		
	CF_CONTROL_POWER = 0x9C, // 电源控制码
	CF_CONTROL_CALLSTATION = 0xA0, // 呼叫站控制码
	CF_CONTROL_BORADCAST = 0xA1, // 广播控制码	
	CF_CONTROL_PC_MEDIA = 0XFE, // PC播放器控制码，**

	CF_PROGRAM_DOWN_ENTRY = 0X01,	// 进入程序下载 0x01 0x00
	CF_PROGRAM_DOWN = 0X02,	//下载程序
	CF_PROGRAM_END = 0x03, //退出下载程序	0x03 0x00
	// 读取程序

	CF_SET_REMOTE_CONTROL = 0X0A, // 设置遥控数据

	CF_CONTROL_ZONE_LINE_MATRIX_1 = 0X88, // 分区线路矩阵 第一个，总共8个
	CF_CONTROL_ZONE_VOLUME_MATRIX_1 = 0X90, // 分区音量矩阵 第一个，总共8个


	CF_WRITE_TER_ID = 0xB0, // 写入终端ID，操作时只允许接入一台终端
	CF_WRITE_TER_ID_RET = 0x72,
	CF_GET_TER_ID = 0x71, // 返回终端ID, 参数 ID=0xffff 为不指定终端
	CF_GET_TER_ID_RET = 0x73,
	CF_SET_TER_PARAMETER = 0xB1, // 设置终端 分区 和终端号
	CF_SET_TER_PARAMETER_RET = CF_WRITE_TER_ID_RET,
	CF_GET_PAGE_TER_STATE = 0X10, // 获取终端状态，以页计算
	CF_GET_PAGE_TER_STATE_RET = 0X10, 
	CF_CONTROL_TER = 0xB2, // 控制终端
	CF_CONTROL_TER_RET = 0x70, 

	CF_SET_6850_LINE = 0x9D, //	6850线路设置
	CF_SET_6850_VOLUME = 0x9E, // 6850音量控制			


	//////////////////////////////////////////////////////////////////////////
	CF_USER = 0xF0, // 用户相关命令
	CF_SUB_USER_CMD_LOGIN = 0x01,
	CF_SUB_USER_CMD_LOGOUT,
	CF_SUB_USER_CMD_ADD,
	CF_SUB_USER_CMD_DEL,
	CF_SUB_USER_CMD_EDIT, 
	CF_SUB_USER_GET, // 返回总数，和详细
	CF_SUB_USER_GET_COUNT,
	CF_SUB_USER_GET_USERS,

	CF_LINK_IP = 0XF1, // 连接设备IP参数 相关命令
	CF_SUB_LINK_IP_GET = 0X01,
	CF_SUB_LINK_IP_SET,

	CF_ALARM_MODE = 0XF2, // 报警模式 
	CF_SUB_ALARM_MODE_GET = 0X01,
	CF_SUB_ALARM_MODE_SET,

	CF_LINE_INFO = 0XF3, // 线路
	CF_SUB_LINE_INFO_GET = 0X01,
	CF_SUB_LINE_INFO_SET,

	CF_ZONE_INFO = 0XF4, // 分区
	CF_SUB_ZONE_NAME_GET=0X01,
	CF_SUB_ZONE_NAME_SET,

	CF_TASK = 0XF5, // 定时节目
	CF_SUB_GROUP_GET = 0x01, // 获取项目(不包含详细任务)
	CF_SUB_GROUP_ADD, // 添加项目
	CF_SUB_GROUP_DEL, // 删除某项目
	CF_SUB_GROUP_EDIT_NAME, // 修改名称
	CF_SUB_TASK_GET, // 获取某项目下任务
	CF_SUB_TASK_SET, // 修改某项目任务
	CF_SUB_PROGRAM_CONFIG_GET, // 获取定时程序配置信息
	CF_SUB_PROGRAM_CONFIG_SET, // 设置定时程序配置信息
	CF_SUB_TASK_DOWN, // 程序下载
	CF_SUB_REMOTE_GET, // 遥控设置获取
	CF_SUB_REMOTE_SET, // 遥控设置设置

	//////////////////////////////////////////////////////////////////////////
	CF_ERR_CONNECT = 0XFE, // 通信错误
	CF_ERR_CMD = 0xFF, // 命令解析错误，返回
};

enum TASK_SET_TYPE{
	TST_NEW = 1, // 新建任务
	TST_EDIT, // 修改任务
	TST_DEL, // 删除任务
	TST_DEL_ALL, //删除所有任务
};

enum MY_ERROR{
	_ERR_SUCCESS = 0, // 正确
	_ERR_USER_NAME, // 用户名错误
	_ERR_USER_PASSWORD, // 密码错误
	_ERR_DB_OPERATE, // 数据库错误 
	
	_ERR_NOT_SPECIFY, // 未指定错误
	
	_ERR_NOT_ACT, //
	_ERR_TIME_OUT,  // 超时引起的错误
	_ERR_DATA_UNDISPOSE, // 指令未处理或指令格式不正确
};

enum {
	_AM_MIN_RATE = 0,
	_AM_MAX_RATE = 9999/*1602*/,
	_FM_MIN_RATE = 0,
	_FM_MAX_RATE = 255,
};
// AM : 我国是步进9KHz，范围是531KHz-1602KHz，
enum RADIO_CTRL_CODE{
	RCC_FM_RATE=0,	// 设置FM频率(参数1=频率整数,参数2=频率小数)
	RCC_FM_CHANNEL,	// 设置FM频道	(参数1=频道，参数2=0)
	RCC_AM_RATE,	// 设置AM频率(参数1=频率高2位，参数2=频率低2位)
	RCC_AM_CHANNEL,	// 设置AM频道(参数1=频道，参数2=0)
	RCC_SEARCH_UP=8,	// 向上搜索(参数1=0，参数2=0)
	RCC_SEARCH_DOWN,	// 向下搜索(参数1=0，参数2=0)
	RCC_SEARCH_AUTO,	// 自动搜索(参数1=0，参数2=0)
	RCC_SAVE_CHANNEL,	// 存储电台(参数1=存储频道，参数2=0)
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
	sockaddr_in m_saTarget; // 目的地址
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
	// 程序下载
	virtual int _EnterDownloadProgram();
	virtual int _EndDownloadProgram();
	virtual int _DownloadProgram(BYTE iDayOfWeek, _DbTaskVec& taskVec); //星期一：0，....

	virtual int _DownRemoteTask(_DbTaskGroupVec& remoteVec);
	// 获取设备状态
	virtual int _GetHostState(BOOL bAppend=FALSE, CTime* pTime=NULL, E_ALARM_MODEL alarmMode=AM_CURRENT_ZONE, BYTE broadVolumeAdd=0);
	// IP和物理地址
	virtual int _GetIPAddr();
	virtual int _SetIPAddr(ULONG net_addr);
	// 序列号和物理地址
	virtual int _GetSerialNumAndMac();
	virtual int _SetSerialNumAndMac(BYTE physicalAddr[6], IN_ADDR ipAddr, USHORT serialNum[4]);
	// 6800注册
	virtual int _Register(USHORT registerNum[4]);

	// 线路设置, 音量控制
	int _SetZoneLine(bool bAllZone, int nZone, BYTE nLine);
	int _SetZoneLine(bool bZoneSel[_MAX_ZONEA], BYTE nLine);
	virtual int _SetMatrixLine(int nMatrix, BYTE nLineVec[8]); // nLineVec[8]: 0~7：正常线路；0x0f：线路不变
	int _SetZoneVolume(bool bAllZone, int nZone, BYTE nVol);
	int _SetZoneVolume(bool bZoneSel[_MAX_ZONEA], BYTE nVol);
	virtual int _SetMatrixVolume(int nMatrix, BYTE nVolVec[8]); // nVolVec[]: 0~_MAX_ZONE_VOLUME; 0x0f：音量不变

	// 查询
	virtual int _QueryMP3(int nDev);
	virtual int _QueryCD(int nDev);
	virtual int _QueryRadio(int nDev);
	// 控制MP3 (nDev: 0~3)
	virtual int _ControlMP3(int nDev, PLAYER_CTRL_MODE code,int nSong = 1);
	//新增选曲循环播放，协议先单独新增
	virtual int _ControlSelMultiMP3(int nDev,PLAYER_CTRL_MODE code,int* nSong);
	// 控制CD (nDev: 0~3)
	virtual int _ControlCD(int nDev, PLAYER_CTRL_MODE code, int nSong = 1);
	// 控制RADIO (nDev: 0~3)
	virtual int _ControlRadio(int nDev, RADIO_CTRL_CODE code, BYTE param1=0, BYTE param2=0);
	// 控制电源
	int _ControlPower(int iIndex, bool bOn = TRUE);
	virtual int _ControlPower(std::bitset<_MAX_POWER>& bPowerOnVec);
	// 控制呼叫站	0xA0
	// 控制广播	0xA1

	// Parse Command
	//////////////////////////////////////////////////////////////////////////
	virtual int ParseAll(BYTE* pBuf, int nLen);
	// HOST
	virtual int ParseHost(BYTE* pBuf, int nLen);

	// IP
	virtual int ParseIP(BYTE* pBuf);
	// 序列号，等信息
	virtual int ParseSerialNumAndMac(BYTE* pBuf);

	// MP3
	virtual int ParseMP3(BYTE* pBuf);
	//MP3新增协议功能，多曲循环
	virtual int ParseMultiCycMP3(BYTE* pBuf);
	//MP3新增协议功能，文件名传送
	virtual int ParseNameMP3(BYTE* pBuf);
	// CD
	virtual int ParseCD(BYTE* pBuf);
	// RADIO
	virtual int ParseRadio(BYTE* pBuf);
	// 报警信号采集器 6823
	virtual int ParseAlarmCollect(BYTE* pBuf);
	// 信号矩阵 6807
	virtual int ParseSingleMartix(BYTE* pBuf); //////
	// 呼叫站 6812
	virtual int ParseCallingStation(BYTE* pBuf);
	// 报警信号发生器 6830
	virtual int ParseAlarmActiver(BYTE* pBuf);
	// POWER
	virtual int ParsePower(BYTE* pBuf);
	// 强切电源控制器 6811
	virtual int ParseStrongCutPowerController(BYTE* pBuf);
	// 电话接口状态 6825
	virtual int ParseTelephoneState(BYTE* pBuf);

public:

};

// 返回长度
extern int __hton_ZoneName(OUT BYTE* pBuf, int nBufLen, SZoneUnitA* pZoneUnit, int nZoneStart, int nZoneEnd);
//
extern int __ParseZoneName(BYTE* pBuf, int nLen, SZoneA& zone);

extern int __TaskToBytes(S_DB_TASK* pTask, BYTE* pStr);
extern int __BytesToTask(S_DB_TASK* pTask, BYTE* pStr);

extern int RemoteTaskToByte(_DbTaskGroupVec& remoteVec, OUT BYTE* pBuf, int nBufLen = 1000);
extern int ByteToRemoteTask(_DbTaskGroupVec& remoteVec, BYTE* pBuf, int nBufLen);
