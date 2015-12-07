#pragma once


#include "6800_data.h"
#include <string>
#include "global.h"

/*
<System>
	<Host>
		<IP addr="192.168.16.88" port="6699"></IP>
		<OTHER alarmMode="0" boradVolumeAdd="0" />
	</Host>

	<UserVec>
		<User name="user1" password="123456" type="65535">
			<Zone all="0">1+2+3+...</Zone>
			<Terminal>1+2+3+...</Terminal>
		</User>
	</UserVec>

	<TerminalVec>
		<Terminal ID="1" config="1" zone="1" name="终端名"></Terminal>
	</TerminalVec>
	
	<ZoneVec>
		<Zone ID="1" name="分区名"></Zone>
	</ZoneVec>

	<AudioInLines>
		<!--audiotype:E_DEV_TYPE-->
		<Line ID="1" name="" config="1" type="1" addr="0"></Single>
	</AudioInLines>
</System>
*/
#define _NODE_System			"System"
#define _NODE_Host				"Host"
#define _NODE_IP				"IP"
#define _NODE_OTHER				"OTHER"
#define _NODE_UserVec			"UserVec"	
#define _NODE_User				"User"	
#define _NODE_ZoneVec			"ZoneVec"
#define _NODE_Zone				"Zone"
#define _NODE_TerminalVec		"TerminalVec"
#define _NODE_Terminal			"Terminal"
#define _NODE_AudioInLines		"AudioInLines"
#define _NODE_Line				"Line"

#define _NODE_GroupVec			"GroupVec"
#define _NODE_Group				"Group"
#define _NODE_RemoteGroup		"RemoteGroup"
#define _NODE_Task				"Task"
#define _NODE_Config			"Config"


#define _ATTRIBUTE_Mon		"Mon"
#define _ATTRIBUTE_Tues		"Tues"
#define _ATTRIBUTE_Wed		"Wed"
#define _ATTRIBUTE_Thur		"Thur"
#define _ATTRIBUTE_Fri		"Fri"
#define _ATTRIBUTE_Sat		"Sat"
#define _ATTRIBUTE_sun		"Sun"

#define _ATTRIBUTE_ID		"ID"
#define _ATTRIBUTE_config	"config"
#define _ATTRIBUTE_zone		"zone"
#define _ATTRIBUTE_name		"name"
#define _ATTRIBUTE_type		"type"
#define _ATTRIBUTE_addr		"addr"
#define _ATTRIBUTE_password "password"
#define _ATTRIBUTE_port		"port"
#define _ATTRIBUTE_alarmMode "alarmMode"
#define _ATTRIBUTE_boradVolumeAdd "boradVolumeAdd"

#define _ATTRIBUTE_second	"second"
#define _ATTRIBUTE_ctrlType	"ctrlType"
// power
#define _ATTRIBUTE_powerOn "powerOn"
// mp3 , cd
#define _ATTRIBUTE_devNum "devNum"
#define _ATTRIBUTE_playmode "playmode"
#define _ATTRIBUTE_song "song"
#define _ATTRIBUTE_songfi "songfi"
#define _ATTRIBUTE_songse "songse"
#define _ATTRIBUTE_songth "songth"
#define _ATTRIBUTE_songfo "songfo"
#define _ATTRIBUTE_songfiv "songfiv"
#define _ATTRIBUTE_songsi "songsi"

// radio
#define _ATTRIBUTE_isFM "isFM"
#define _ATTRIBUTE_isChannel "isChannel"
#define _ATTRIBUTE_rate "rate"
#define _ATTRIBUTE_channel "channel"
// line , volume
#define _ATTRIBUTE_line "line"
#define _ATTRIBUTE_volume "volume"
#define _ATTRIBUTE_zoneOn "zoneOn"


class CDataBase
{
public:
	CDataBase()
	{
	}

	~CDataBase()
	{
		if (m_pDoc)
		{
			delete m_pDoc;
		}
	}

private:
	std::string m_XmlFile;
public:
	TiXmlDocument* m_pDoc;
	bool SetFilePath(std::string XmlFile);
	bool CreateXml(std::string XmlFile);
	
	S_DB_USER_VEC m_user;
	SSystem* m_pSys;
	void SetParamter(SSystem* pSys){
		ASSERT(pSys);
		m_pSys = pSys;
	}

	// 服务端口
	UINT GetServerPort();

	// 设备IP地址
	bool GetNetParameter(ULONG& net_addr, USHORT& net_port); // 即使读取失败，也返回默认值
	bool SetNetParameter(ULONG net_addr, USHORT net_port);
	
	// 报警方式 和 广播音量增加值
	bool GetAlarmMode(E_ALARM_MODEL& alarmMode, BYTE& boradVolumeAdd);
	bool SetAlarmMode(E_ALARM_MODEL alarmMode, BYTE boradVolumeAdd);

	bool LoadZone();
	bool LoadAudioInLine();
	bool LoadUsers();
	void Load()
	{
		GetAlarmMode(m_pSys->alarmMode, m_pSys->broadVolumeAdd);
		LoadZone();
		LoadAudioInLine();
		LoadUsers();

		_DbUserVec::iterator iterRet;
		if (!m_user.FindUser(DEF_SUPER_USER, iterRet))
		{
			S_DB_USER userAdmin;
			userAdmin.SetName(DEF_SUPER_USER);
			userAdmin.SetPassword(DEF_SUPER_USER);
			userAdmin.type = _UT_ADMIN;
			m_user.m_userVec.push_back(userAdmin);
			WriteUsers();
		}
	}

	bool WriteZone();
	bool WriteAudioInLine();
	bool WriteUsers();

protected:
	TiXmlElement* GetSystemElement();
};


//////////////////////////////////////////////////////////////////////////
class CTaskDataBase
{
public:
	CTaskDataBase()
	{
		m_bProgConfig = false;
		for (int i=0; i<7; i++)
		{
			m_szProgConfig[i][0] = NULL;
		}
		for (int i=0; i<_MAX_REMOTE_FUNC; i++)
		{
			S_DB_TASK_GROUP group;
			m_remoteVec.push_back(group);
		}
	}

	~CTaskDataBase()
	{
		if (m_pDoc)
		{
			delete m_pDoc;
		}
	}

private:
	std::string m_XmlFile;
public:
	TiXmlDocument* m_pDoc;
	bool SetFilePath(std::string XmlFile);
	bool CreateXml(std::string XmlFile);

	_DbTaskGroupVec m_groupVec;
	bool m_bProgConfig; // 是否已经下载
	char m_szProgConfig[7][_MAX_NAME_LEN+1]; // 程序每天配置

	// 遥控器功能
	_DbTaskGroupVec m_remoteVec;

	bool Load(){
		LoadRemote();
		LoadConfig();
		return LoadTasks();
	}

	bool LoadConfig();
	bool WriteConfig();
	bool LoadTasks();
	bool WriteTasks();
	bool LoadRemote();
	bool WriteRemote();
	void SortTask();
};

__declspec(selectany) CTaskDataBase theTaskDB;
__declspec(selectany) CDataBase theDataBase;