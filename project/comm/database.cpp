#include "stdafx.h"
#include "database.h"
#include "MyEncrypt.h"

#define MY_INLINE 

/*!
*  /brief 创建xml文件。
*
*  /param XmlFile xml文件全路径。
*  /return 是否成功。true为成功，false表示失败。
*/
MY_INLINE bool CDataBase::CreateXml(std::string XmlFile)
{
	// 定义一个TiXmlDocument类指针
	if (m_pDoc)
	{
		delete m_pDoc;
	}
	m_pDoc = new TiXmlDocument;

	if (NULL==m_pDoc)
	{
		return false;
	}
	m_pDoc->UseMicrosoftBOM(true);
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration(_VERSION_DATA_A, "UTF-8", "");
	if (NULL==pDeclaration)
	{
		return false;
	}
	m_pDoc->LinkEndChild(pDeclaration);


	// 生成一个根节点：System
	TiXmlElement *pRootEle = new TiXmlElement(_NODE_System);
	if (NULL==pRootEle)
	{
		return false;
	}
	m_pDoc->LinkEndChild(pRootEle);
	// 生成子节点：Terminals
	TiXmlElement *pTerminalsEle = new TiXmlElement(_NODE_TerminalVec);
	if (NULL==pTerminalsEle)
	{
		return false;
	}
	pRootEle->LinkEndChild(pTerminalsEle);
	// 生成子节点：Zones
	TiXmlElement *pZonesEle = new TiXmlElement(_NODE_ZoneVec);
	if (NULL==pZonesEle)
	{
		return false;
	}
	pRootEle->LinkEndChild(pZonesEle);
	// 生成子节点：InputAudioLines
	TiXmlElement *InputAudioLinesEle = new TiXmlElement(_NODE_AudioInLines);
	if (NULL==InputAudioLinesEle)
	{
		return false;
	}
	pRootEle->LinkEndChild(InputAudioLinesEle);

	return m_pDoc->SaveFile(XmlFile);
}

MY_INLINE bool CDataBase::SetFilePath(std::string XmlFile)
{
	m_XmlFile = XmlFile;
	// 检测是否文件
	// 已经存在一个XML的文档对象，则删除原有对象
	if (m_pDoc)
	{
		// 
		delete m_pDoc;
	}
	m_pDoc = new TiXmlDocument(m_XmlFile.c_str());
	m_pDoc->UseMicrosoftBOM(true);
	if (!m_pDoc->LoadFile())
	{
		return CreateXml(m_XmlFile);
	}

	TiXmlDeclaration* decl = m_pDoc->FirstChild()->ToDeclaration();
	if (decl != NULL 
		&& 0 == strcmp(decl->Version(), _VERSION_DATA_A))
	{
		return true;
	}
	else
	{
//  		m_pDoc->SaveFile(m_XmlFile + ".old.xml");
//  		return CreateXml(m_XmlFile);
	}

	return true;
}

MY_INLINE bool CDataBase::LoadZone()
{
	ASSERT(m_pDoc);
	ASSERT(m_pSys);
	//
	TiXmlElement *pSysElement = m_pDoc->FirstChildElement(_NODE_System);
	if (!pSysElement)
		return true;

	TiXmlElement* pTerminals = pSysElement->FirstChildElement(_NODE_ZoneVec); 
	if (!pTerminals)
		return true;

	for (TiXmlElement* pElement = pTerminals->FirstChildElement(_NODE_Zone);
		pElement != NULL;
		pElement = pElement->NextSiblingElement(_NODE_Zone))
	{
		const char * pszName = NULL;
		int nID = -1;

		for (TiXmlAttribute* attribute = pElement->FirstAttribute();
			attribute != NULL;
			attribute = attribute->Next())
		{
			if (strcmp(attribute->Name(), _ATTRIBUTE_ID) == 0)
			{
				nID = attribute->IntValue();
			} 
			else if (strcmp(attribute->Name(), _ATTRIBUTE_name) == 0)
			{
				pszName = attribute->Value();
			}
		}

		if (nID>=0 && nID <_MAX_ZONEA && pszName)
		{
			strcpy_s(m_pSys->m_zone.it[nID].name, _MAX_NAME_LEN+1, UTF8_to_ANSI(pszName));
		}
	}
	return true;
}

MY_INLINE bool CDataBase::LoadAudioInLine()
{
	ASSERT(m_pDoc);
	ASSERT(m_pSys);
	//
	TiXmlElement *pSysElement = m_pDoc->FirstChildElement(_NODE_System);
	if (!pSysElement)
		return true;

	TiXmlElement* pTerminals = pSysElement->FirstChildElement(_NODE_AudioInLines); 
	if (!pTerminals)
		return true;

	for (TiXmlElement* pElement = pTerminals->FirstChildElement(_NODE_Line);
		pElement != NULL;
		pElement = pElement->NextSiblingElement(_NODE_Line))
	{
		int nID = -1;
		const char* pszName = NULL;
		BYTE nAudioType = 0;
		int nAddr = 0;

		for (TiXmlAttribute* attribute = pElement->FirstAttribute();
			attribute != NULL;
			attribute = attribute->Next())
		{
			if (strcmp(attribute->Name(), _ATTRIBUTE_ID) == 0)
			{
				nID = attribute->IntValue();
			} 
			else if (strcmp(attribute->Name(), _ATTRIBUTE_name) == 0)
			{
				pszName = attribute->Value();
			}
			else if (strcmp(attribute->Name(), _ATTRIBUTE_type) == 0)
			{
				nAudioType = (BYTE)attribute->IntValue();
			}
			else if (strcmp(attribute->Name(), _ATTRIBUTE_addr) == 0)
			{
				nAddr = attribute->IntValue();
				if (!(nAddr>=0 && nAddr<8))
				{
					nAddr = 0;
				}
			}
		}

		if (nID>=0 && nID <_MAX_AUDIO_LINE)
		{
			strcpy_s(m_pSys->m_audioInLine.config.name[nID], _MAX_NAME_LEN+1, UTF8_to_ANSI(pszName));
			m_pSys->m_audioInLine.config.nAudioType[nID] = ByteToDevType(nAudioType);
			m_pSys->m_audioInLine.config.iSubParam[nID] = nAddr;
		}
	}
	return true;
}

MY_INLINE bool CDataBase::LoadUsers()
{
	ASSERT(m_pDoc);
	m_user.m_userVec.clear();
	//m_userVec.erase(m_userVec.begin(), m_userVec.end());
	//
	TiXmlElement *pSysElement = m_pDoc->FirstChildElement(_NODE_System);
	if (!pSysElement)
		return true;

	TiXmlElement* pUserVec = pSysElement->FirstChildElement(_NODE_UserVec); 
	if (!pUserVec)
		return true;

	CMyEncrypt myEnc;
	for (TiXmlElement* pElement = pUserVec->FirstChildElement(_NODE_User);
		pElement != NULL;
		pElement = pElement->NextSiblingElement(_NODE_User))
	{
		S_DB_USER user;

		for (TiXmlAttribute* attribute = pElement->FirstAttribute();
			attribute != NULL;
			attribute = attribute->Next())
		{
			if (strcmp(attribute->Name(), _ATTRIBUTE_name) == 0)
			{
				strcpy_s(user.name, _MAX_NAME_LEN+1, UTF8_to_ANSI(attribute->Value()));
			}
			else if (strcmp(attribute->Name(), _ATTRIBUTE_password) == 0)
			{
				char password_org[_MAX_PASSWORD_LEN*2+1];
				strcpy_s(password_org, sizeof(password_org), attribute->Value());
				char password_enc[_MAX_PASSWORD_LEN+1];
				HexStringToChar(password_org, strlen(password_org), password_enc, sizeof(password_enc));
				myEnc._Decrypt(user.password, password_enc, _MAX_PASSWORD_LEN);
				user.password[_MAX_PASSWORD_LEN] = NULL;

				//strcpy_s(user.password, _MAX_PASSWORD_LEN+1, attribute->Value());
			}
			else if (strcmp(attribute->Name(), _ATTRIBUTE_type) == 0)
			{
				user.type = (UINT)attribute->IntValue();
			}
			else if (strcmp(attribute->Name(), _ATTRIBUTE_zone) == 0)
			{
				std::string str(attribute->Value());
				user.bZoneVec = std::bitset<_MAX_ZONEA>(str);
			}
		}
		m_user.m_userVec.push_back(user);
	}
	return true;
}

MY_INLINE TiXmlElement* CDataBase::GetSystemElement()
{
	TiXmlElement *pSysElement = m_pDoc->FirstChildElement(_NODE_System);
	if (!pSysElement)
	{
		// 生成一个根节点：System
		pSysElement = new TiXmlElement(_NODE_System);
		if (!pSysElement)
		{
			ASSERT(FALSE);
			return NULL;
		}
		m_pDoc->LinkEndChild(pSysElement);
	}
	return pSysElement;
}

// MY_INLINE bool CDataBase::WriteTerminal()
// {
// 	ASSERT(m_pDoc);
// 	//
// 	TiXmlElement *pSysElement = GetSystemElement();
// 	if (!pSysElement)
// 	{
// 		ASSERT(FALSE);
// 		return false;
// 	}
// 
// 	TiXmlElement* pTerminalVec = pSysElement->FirstChildElement(_NODE_TerminalVec); 
// 	if (pTerminalVec)
// 	{
// 		pSysElement->RemoveChild(pTerminalVec);
// 	}
// 
// 	pTerminalVec = new TiXmlElement(_NODE_TerminalVec);
// 	if (!pTerminalVec)
// 	{
// 		ASSERT(FALSE);
// 		return false;
// 	}
// 	pSysElement->LinkEndChild(pTerminalVec);
// 
// 	//
// 	for (int i=0; i<_MAX_TERMINAL; i++)
// 	{
// 		TiXmlElement* pTer = new TiXmlElement(_NODE_Terminal);
// 		if (!pTer)
// 		{
// 			ASSERT(FALSE);
// 			return false;
// 		}
// 		pTer->SetAttribute(_ATTRIBUTE_ID, i);
// 		pTer->SetAttribute(_ATTRIBUTE_config, m_terVec[i].bConfig ? 1:0);
// 		pTer->SetAttribute(_ATTRIBUTE_zone, m_terVec[i].iZone);
// 		pTer->SetAttribute(_ATTRIBUTE_name, m_terVec[i].name);
// 		pTerminalVec->LinkEndChild(pTer);
// 	}
// 
// 	return m_pDoc->SaveFile(m_XmlFile);
// }
// 
MY_INLINE bool CDataBase::WriteZone()
{
	ASSERT(m_pDoc);
	ASSERT(m_pSys);
	//
	TiXmlElement *pSysElement = GetSystemElement();
	if (!pSysElement)
	{
		ASSERT(FALSE);
		return false;
	}

	TiXmlElement* pZoneVec = pSysElement->FirstChildElement(_NODE_ZoneVec); 
	if (pZoneVec)
	{
		pSysElement->RemoveChild(pZoneVec);
	}

	pZoneVec = new TiXmlElement(_NODE_ZoneVec);
	if (!pZoneVec)
	{
		ASSERT(FALSE);
		return false;
	}
	pSysElement->LinkEndChild(pZoneVec);

	//
	for (int i=0; i<_MAX_ZONEA; i++)
	{
		TiXmlElement* pZone = new TiXmlElement(_NODE_Zone);
		if (!pZone)
		{
			ASSERT(FALSE);
			return false;
		}
		
		pZone->SetAttribute(_ATTRIBUTE_ID, i);
		pZone->SetAttribute(_ATTRIBUTE_name,  ANSI_to_UTF8(m_pSys->m_zone.it[i].name));
		pZoneVec->LinkEndChild(pZone);
	}

	return m_pDoc->SaveFile(m_XmlFile);
}

MY_INLINE bool CDataBase::WriteAudioInLine()
{
	ASSERT(m_pDoc);
	ASSERT(m_pSys);
	//
	TiXmlElement *pSysElement = GetSystemElement();
	if (!pSysElement)
	{
		ASSERT(FALSE);
		return false;
	}

	TiXmlElement* pAudioLines = pSysElement->FirstChildElement(_NODE_AudioInLines); 
	if (pAudioLines)
	{
		pSysElement->RemoveChild(pAudioLines);
	}

	pAudioLines = new TiXmlElement(_NODE_AudioInLines);
	if (!pAudioLines)
	{
		ASSERT(FALSE);
		return false;
	}
	pSysElement->LinkEndChild(pAudioLines);

	//
	for (int i=0; i<_MAX_AUDIO_LINE; i++)
	{
		TiXmlElement* pLine = new TiXmlElement(_NODE_Line);
		if (!pLine)
		{
			ASSERT(FALSE);
			return false;
		}
		pLine->SetAttribute(_ATTRIBUTE_ID, i);
		pLine->SetAttribute(_ATTRIBUTE_name, ANSI_to_UTF8(m_pSys->m_audioInLine.config.name[i]));
		pLine->SetAttribute(_ATTRIBUTE_type, m_pSys->m_audioInLine.config.nAudioType[i]);
		pLine->SetAttribute(_ATTRIBUTE_addr, m_pSys->m_audioInLine.config.iSubParam[i]);
		pAudioLines->LinkEndChild(pLine);
	}

	return m_pDoc->SaveFile(m_XmlFile);
}

MY_INLINE bool CDataBase::WriteUsers()
{
	ASSERT(m_pDoc);
	//
	TiXmlElement *pSysElement = GetSystemElement();
	if (!pSysElement)
	{
		ASSERT(FALSE);
		return false;
	}

	TiXmlElement* pUserVec = pSysElement->FirstChildElement(_NODE_UserVec); 
	if (pUserVec)
	{
		pSysElement->RemoveChild(pUserVec);
	}

	pUserVec = new TiXmlElement(_NODE_UserVec);
	if (!pUserVec)
	{
		ASSERT(FALSE);
		return false;
	}
	pSysElement->LinkEndChild(pUserVec);

	CMyEncrypt myEnc;
	//
	for (_DbUserVec::iterator iter = m_user.m_userVec.begin();
							  iter != m_user.m_userVec.end(); 
							  ++iter)
	{
		TiXmlElement* pUser = new TiXmlElement(_NODE_User);
		if (!pUser)
		{
			ASSERT(FALSE);
			return false;
		}
		pUser->SetAttribute(_ATTRIBUTE_name, ANSI_to_UTF8((*iter).name));

		char passwordEnc[_MAX_PASSWORD_LEN+1];
		myEnc._Encrypt(passwordEnc, iter->password, _MAX_PASSWORD_LEN);
		passwordEnc[_MAX_PASSWORD_LEN] = NULL;
		std::string strPwd = CharToHexString(_MAX_PASSWORD_LEN, passwordEnc);
		pUser->SetAttribute(_ATTRIBUTE_password, strPwd);
		//pUser->SetAttribute(_ATTRIBUTE_password, iter->password);

		pUser->SetAttribute(_ATTRIBUTE_type, (int)iter->type);
		pUser->SetAttribute(_ATTRIBUTE_zone, iter->bZoneVec.to_string());
		pUserVec->LinkEndChild(pUser);
	}

	return m_pDoc->SaveFile(m_XmlFile);
}

UINT CDataBase::GetServerPort()
{
	UINT net_port = SERVER_PORT;

	ASSERT(m_pDoc);
	//
	TiXmlElement *pSysElement = m_pDoc->FirstChildElement(_NODE_System);
	if (!pSysElement)
		return false;
	int iPort = 0;

	pSysElement->Attribute("server_port", &iPort);
	if (iPort != 0)
	{
		net_port = (UINT) iPort;
	}
	return net_port;
}


// 即使读取失败，也返回默认值
bool CDataBase::GetNetParameter(ULONG& net_addr, USHORT& net_port)
{
	net_addr = inet_addr(DEV_IP_DEF);
	net_port = htons(DEV_NET_PORT);

	ASSERT(m_pDoc);
	//
	TiXmlElement *pSysElement = m_pDoc->FirstChildElement(_NODE_System);
	if (!pSysElement)
		return false;

	TiXmlElement* pHostVec = pSysElement->FirstChildElement(_NODE_Host); 
	if (!pHostVec)
		return false;

	TiXmlElement* pElement = pHostVec->FirstChildElement(_NODE_IP);
	if (pElement != NULL)
	{
		bool bIp = false;
		bool bPort = false;
		for (TiXmlAttribute* attribute = pElement->FirstAttribute();
			attribute != NULL;
			attribute = attribute->Next())
		{
			if (strcmp(attribute->Name(), _ATTRIBUTE_addr) == 0)
			{
				std::string addr = attribute->Value();
				ULONG ina = inet_addr(addr.c_str());
				if (ina != INADDR_NONE)
				{
					net_addr = ina;
					bIp = true;
				}
			}
			else if (strcmp(attribute->Name(), _ATTRIBUTE_port) == 0)
			{
				net_port = htons(attribute->IntValue());
				bPort = true;
			}
		}
		if (bIp && bPort)
		{
			return true;
		}
	}
 	return false;
}

bool CDataBase::SetNetParameter( ULONG net_addr, USHORT net_port )
{
	ASSERT(m_pDoc);
 	ASSERT(net_addr != INADDR_NONE);
 	ASSERT(net_port > 100);
 	//
 	TiXmlElement *pSysElement = GetSystemElement();
 	if (!pSysElement)
 	{
 		ASSERT(FALSE);
 		return false;
 	}
 
 	TiXmlElement* pHost = pSysElement->FirstChildElement(_NODE_Host); 
 	TiXmlElement* pIP;
 	if (!pHost)
	{
		pHost = new TiXmlElement(_NODE_Host);
		if (!pHost)
		{
			ASSERT(FALSE);
			return false;
		}
		pSysElement->LinkEndChild(pHost);
	}
	else
	{
		pIP = pHost->FirstChildElement(_NODE_IP);
		if (pIP)
		{
			pHost->RemoveChild(pIP);
		}
	}

	pIP = new TiXmlElement(_NODE_IP);
	if (!pIP)
	{
		ASSERT(FALSE);
		return false;
	}

 	//net_addr
	IN_ADDR trIa;
	trIa.S_un.S_addr = net_addr;
	pIP->SetAttribute(_ATTRIBUTE_addr, inet_ntoa(trIa));
 	pIP->SetAttribute(_ATTRIBUTE_port, ntohs(net_port));
 	pHost->LinkEndChild(pIP);

	return m_pDoc->SaveFile(m_XmlFile);
}

bool CDataBase::GetAlarmMode(E_ALARM_MODEL& alarmMode, BYTE& boradVolumeAdd)
{
	alarmMode = AM_CURRENT_ZONE;
	boradVolumeAdd = 0;

	ASSERT(m_pDoc);
	//
	TiXmlElement *pSysElement = m_pDoc->FirstChildElement(_NODE_System);
	if (!pSysElement)
		return false;

	TiXmlElement* pHostVec = pSysElement->FirstChildElement(_NODE_Host); 
	if (!pHostVec)
		return false;

	TiXmlElement* pElement = pHostVec->FirstChildElement(_NODE_OTHER);
	if (pElement != NULL)
	{
		bool bAM = false;
		bool bAddVol = false;
		for (TiXmlAttribute* attribute = pElement->FirstAttribute();
			attribute != NULL;
			attribute = attribute->Next())
		{
			if (strcmp(attribute->Name(), _ATTRIBUTE_alarmMode) == 0)
			{
				alarmMode =  ByteToAlarmModel( (BYTE)attribute->IntValue() );
				bAM = true;
			}
			else if (strcmp(attribute->Name(), _ATTRIBUTE_boradVolumeAdd) == 0)
			{
				boradVolumeAdd = (BYTE)attribute->IntValue();
				bAddVol = true;
			}
		}
		if (bAM && bAddVol)
		{
			return true;
		}
	}
	return false;
}

bool CDataBase::SetAlarmMode(E_ALARM_MODEL alarmMode, BYTE boradVolumeAdd)
{
	ASSERT(m_pDoc);
	ASSERT(boradVolumeAdd < 20 );
	//
	TiXmlElement *pSysElement = GetSystemElement();
	if (!pSysElement)
	{
		ASSERT(FALSE);
		return false;
	}

	TiXmlElement* pHost = pSysElement->FirstChildElement(_NODE_Host); 
	TiXmlElement* pOther;
	if (!pHost)
	{
		pHost = new TiXmlElement(_NODE_Host);
		if (!pHost)
		{
			ASSERT(FALSE);
			return false;
		}
		pSysElement->LinkEndChild(pHost);
	}
	else
	{
		pOther = pHost->FirstChildElement(_NODE_OTHER);
		if (pOther)
		{
			pHost->RemoveChild(pOther);
		}
	}

	pOther = new TiXmlElement(_NODE_OTHER);
	if (!pOther)
	{
		ASSERT(FALSE);
		return false;
	}

	pOther->SetAttribute(_ATTRIBUTE_alarmMode, alarmMode);
	pOther->SetAttribute(_ATTRIBUTE_boradVolumeAdd, boradVolumeAdd);

	pHost->LinkEndChild(pOther);
	return m_pDoc->SaveFile(m_XmlFile);
}

//////////////////////////////////////////////////////////////////////////
/*!
*  /brief 创建xml文件。
*
*  /param XmlFile xml文件全路径。
*  /return 是否成功。true为成功，false表示失败。
*/
MY_INLINE bool CTaskDataBase::CreateXml(std::string XmlFile)
{
	// 定义一个TiXmlDocument类指针
	if (m_pDoc)
	{
		delete m_pDoc;
	}
	m_pDoc = new TiXmlDocument;

	if (NULL==m_pDoc)
	{
		return false;
	}
	m_pDoc->UseMicrosoftBOM(true);
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration(_VERSION_DATA_A, "UTF-8", "");
	if (NULL==pDeclaration)
	{
		return false;
	}
	m_pDoc->LinkEndChild(pDeclaration);

	// 生成一个根节点：
	TiXmlElement *pRootEle = new TiXmlElement(_NODE_GroupVec);
	if (NULL==pRootEle)
	{
		return false;
	}
	m_pDoc->LinkEndChild(pRootEle);

	return m_pDoc->SaveFile(XmlFile);
}

MY_INLINE bool CTaskDataBase::SetFilePath(std::string XmlFile)
{
	m_XmlFile = XmlFile;
	// 检测是否文件
	// 已经存在一个XML的文档对象，则删除原有对象
	if (m_pDoc)
	{
		// 
		delete m_pDoc;
	}
	m_pDoc = new TiXmlDocument(m_XmlFile.c_str());
	m_pDoc->UseMicrosoftBOM(true);
	if (!m_pDoc->LoadFile())
	{
		return CreateXml(m_XmlFile);
	}

	TiXmlDeclaration* decl = m_pDoc->FirstChild()->ToDeclaration();
	if (decl != NULL
		&& 0 == strcmp(decl->Version(), _VERSION_DATA_A))
	{
		return true;
	}
	else
	{
//  		m_pDoc->SaveFile(m_XmlFile + ".old.xml");
//  		return CreateXml(m_XmlFile);
	}

	return true;
}

// _NODE_GroupVec			
// _NODE_Group
// _NODE_Task
BOOL SelectZoneToString(const std::bitset<_MAX_ZONEA>& zoneSelect, char* pStr=NULL)
{
	ASSERT(pStr);
	ZeroMemory(pStr, _MAX_ZONEA / 4 + 1);
	for (int iZone=0, iPos=0; iZone<_MAX_ZONEA; iPos++)
	{
		pStr[iPos] |= zoneSelect[iZone++] ? 0x01 : 0x00;
		pStr[iPos] |= zoneSelect[iZone++] ? 0x02 : 0x00;
		pStr[iPos] |= zoneSelect[iZone++] ? 0x04 : 0x00;
		pStr[iPos] |= zoneSelect[iZone++] ? 0x08 : 0x00;

		if (!pStr[iPos]) 
			pStr[iPos] |= 0x10;
	}
	return TRUE;
}

BOOL StringToSelectZone(std::bitset<_MAX_ZONEA>& zoneSelect, const char* pStr=NULL)
{
	ASSERT(pStr);

	int nLen = strlen(pStr);

	for (int iZone=0, iPos=0; iZone<_MAX_ZONEA ; iPos++)
	{
		if (iPos < nLen)
		{
			zoneSelect[iZone++] = (pStr[iPos] & 0x01) ? 0x01 : 0x00;
			zoneSelect[iZone++] = (pStr[iPos] & 0x02) ? 0x01 : 0x00;
			zoneSelect[iZone++] = (pStr[iPos] & 0x04) ? 0x01 : 0x00;
			zoneSelect[iZone++] = (pStr[iPos] & 0x08) ? 0x01 : 0x00;
		}
		else
		{
			zoneSelect[iZone++] = FALSE;
			zoneSelect[iZone++] = FALSE;
			zoneSelect[iZone++] = FALSE;
			zoneSelect[iZone++] = FALSE;
		}
	}
	return TRUE;
}

char* pszDayName[7] = {
	_ATTRIBUTE_Mon,
		_ATTRIBUTE_Tues,
		_ATTRIBUTE_Wed,
		_ATTRIBUTE_Thur,
		_ATTRIBUTE_Fri,
		_ATTRIBUTE_Sat,
		_ATTRIBUTE_sun};

MY_INLINE bool CTaskDataBase::LoadConfig()
{
	m_bProgConfig = false;
	for (int i=0; i<7; i++)
	{
		m_szProgConfig[i][0] = NULL;
	}

	ASSERT(m_pDoc);
	TiXmlElement * pConfigElement = m_pDoc->FirstChildElement(_NODE_Config);
	if (!pConfigElement)
	{
		return true;
	}

	// 获取项目属性
	for (TiXmlAttribute* attribute = pConfigElement->FirstAttribute();
		attribute != NULL; 
		attribute = attribute->Next())
	{
		if (strcmp(attribute->Name(), _ATTRIBUTE_config) == 0)
		{
			m_bProgConfig = attribute->IntValue() ? true:false;
		}
	}


	for (int i=0; i<7; i++)
	{
		TiXmlElement* pEle = pConfigElement->FirstChildElement(pszDayName[i]);
		if (pEle && pEle->GetText())
		{
			strcpy_s(&m_szProgConfig[i][0], _MAX_NAME_LEN+1, UTF8_to_ANSI(pEle->GetText()));
		}
	}
	return true;
}

MY_INLINE bool CTaskDataBase::WriteConfig()
{

	ASSERT(m_pDoc);
	TiXmlElement * pConfigElement = m_pDoc->FirstChildElement(_NODE_Config);
	if (pConfigElement)
	{
		m_pDoc->RemoveChild(pConfigElement);
	}
	pConfigElement = new TiXmlElement(_NODE_Config);
	if (!pConfigElement)
	{
		ASSERT(FALSE);
		return false;
	}
	pConfigElement->SetAttribute(_ATTRIBUTE_config, m_bProgConfig ? 1 : 0);
	m_pDoc->LinkEndChild(pConfigElement);

	for (int i=0; i<7; i++)
	{
		TiXmlElement* pEle = new TiXmlElement(pszDayName[i]);
		if (!pEle)
		{
			ASSERT(FALSE);
			return false;
		}
		pConfigElement->LinkEndChild(pEle);

 		TiXmlText* pNode = new TiXmlText(ANSI_to_UTF8(m_szProgConfig[i]));
 		pEle->LinkEndChild(pNode);
	}

	return m_pDoc->SaveFile(m_XmlFile);
}

inline BOOL GetTaskAttribute(S_DB_TASK& task, TiXmlAttribute* attribute)
{
	BYTE nFlag = 0;
	//
	for ( ; attribute != NULL; attribute = attribute->Next())
	{
		if (strcmp(attribute->Name(), _ATTRIBUTE_second) == 0)
		{
			task.nTimeSec = (UINT)attribute->IntValue();
			nFlag |= 0x01;
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_name) == 0)
		{
			strcpy_s(task.name, _MAX_NAME_LEN+1, UTF8_to_ANSI(attribute->Value()));
		} 
		else if (strcmp(attribute->Name(), _ATTRIBUTE_ctrlType) == 0)
		{
			task.devType = (BYTE)attribute->IntValue();
			nFlag |= 0x02;
		} 
		// power
		else if (strcmp(attribute->Name(), _ATTRIBUTE_powerOn) == 0)
		{
			Byte2BitSet((BYTE)attribute->IntValue(), task.bPowerOnVec);
		} 
		// mp3 , cd
		else if (strcmp(attribute->Name(), _ATTRIBUTE_devNum) == 0)
		{
			task.nDev = (BYTE)attribute->IntValue();
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_playmode) == 0)
		{
			task.playMode = (BYTE)attribute->IntValue();
			if (task.playMode == 0x07)
			{
				task.playMode = 0;
			}
		} 
		else if (strcmp(attribute->Name(), _ATTRIBUTE_song) == 0)
		{
			task.song = (BYTE)attribute->IntValue();
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_songfi) == 0)
		{
			task.multisong[0] = (BYTE)attribute->IntValue();
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_songse) == 0)
		{
			task.multisong[1] = (BYTE)attribute->IntValue();
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_songth) == 0)
		{
			task.multisong[2] = (BYTE)attribute->IntValue();
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_songfo) == 0)
		{
			task.multisong[3] = (BYTE)attribute->IntValue();
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_songfiv) == 0)
		{
			task.multisong[4] = (BYTE)attribute->IntValue();
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_songsi) == 0)
		{
			task.multisong[5] = (BYTE)attribute->IntValue();
		}
		// radio, （nDev 同MP3）
		else if (strcmp(attribute->Name(), _ATTRIBUTE_isFM) == 0)
		{
			task.is_FM = attribute->IntValue() ? true : false;
		} 
		else if (strcmp(attribute->Name(), _ATTRIBUTE_isChannel) == 0)
		{
			task.is_channel = attribute->IntValue() ? true : false;
		} 
		else if (strcmp(attribute->Name(), _ATTRIBUTE_rate) == 0)
		{
			int tmp = attribute->IntValue();
			float frate = (float)tmp;
			task.rate = frate / 100;
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_channel) == 0)
		{
			task.nChannel = (BYTE)attribute->IntValue();
		} 
		// line , volume
		else if (strcmp(attribute->Name(), _ATTRIBUTE_line) == 0)
		{
			task.nLine = (BYTE)attribute->IntValue();
		}
		else if (strcmp(attribute->Name(), _ATTRIBUTE_volume) == 0)
		{
			task.nVolume = (BYTE)attribute->IntValue();
		} 
		else if (strcmp(attribute->Name(), _ATTRIBUTE_zoneOn) == 0)
		{
			std::string str(attribute->Value());
			task.bSelZoneVec = std::bitset<_MAX_ZONEA>(str);
		}
	}


	if (nFlag == 0x03 && task.IsInvalid())
	{
		return TRUE;
	}
	return FALSE;
}

inline BOOL SetTaskAttribute(TiXmlElement* pTaskElement, const S_DB_TASK& task)
{
	pTaskElement->SetAttribute(_ATTRIBUTE_second, (int)task.nTimeSec);
	pTaskElement->SetAttribute(_ATTRIBUTE_name, ANSI_to_UTF8(task.name));
	pTaskElement->SetAttribute(_ATTRIBUTE_ctrlType, task.devType);

	if (_DEV_DEFAULT == task.devType)
	{
	} 
	else if (_DEV_TYPE_POWER == task.devType)
	{
		pTaskElement->SetAttribute(_ATTRIBUTE_powerOn, task.bPowerOnVec.to_ulong());
	} 
	else if (_DEV_TYPE_MP3 == task.devType || 
		_DEV_TYPE_CD == task.devType)
	{
		pTaskElement->SetAttribute(_ATTRIBUTE_devNum, task.nDev);
		pTaskElement->SetAttribute(_ATTRIBUTE_playmode, task.playMode);
		pTaskElement->SetAttribute(_ATTRIBUTE_song,task.song);
		
		pTaskElement->SetAttribute(_ATTRIBUTE_songfi, task.multisong[0]);
 		pTaskElement->SetAttribute(_ATTRIBUTE_songse, task.multisong[1]);
 		pTaskElement->SetAttribute(_ATTRIBUTE_songth, task.multisong[2]);
 		pTaskElement->SetAttribute(_ATTRIBUTE_songfo, task.multisong[3]);
 		pTaskElement->SetAttribute(_ATTRIBUTE_songfiv, task.multisong[4]);
 		pTaskElement->SetAttribute(_ATTRIBUTE_songsi, task.multisong[5]);
	}
	else if (_DEV_TYPE_RADIO == task.devType)
	{
		pTaskElement->SetAttribute(_ATTRIBUTE_devNum, task.nDev);
		pTaskElement->SetAttribute(_ATTRIBUTE_isFM, task.is_FM);
		pTaskElement->SetAttribute(_ATTRIBUTE_isChannel, task.is_channel);
		if (task.is_channel)
		{
			pTaskElement->SetAttribute(_ATTRIBUTE_channel, task.nChannel);
		} 
		else
		{
			float f = task.rate*100;
			int ret = (int)f;
			pTaskElement->SetAttribute(_ATTRIBUTE_rate, ret);
		}
	}
	else if (_TYPE_ZONE_LINE == task.devType)
	{
		// 				char pStr[_MAX_ZONEA/2 + 2] = {0};
		// 				SelectZoneToString(task.bSelZoneVec, pStr);
		pTaskElement->SetAttribute(_ATTRIBUTE_line, task.nLine);
		pTaskElement->SetAttribute(_ATTRIBUTE_zoneOn, task.bSelZoneVec.to_string());
	}
	else if (_TYPE_ZONE_VOLUME == task.devType)
	{
		// 				char pStr[_MAX_ZONEA/2 + 2] = {0};
		// 				SelectZoneToString(task.bSelZoneVec, pStr);
		pTaskElement->SetAttribute(_ATTRIBUTE_volume, task.nVolume);
		pTaskElement->SetAttribute(_ATTRIBUTE_zoneOn, task.bSelZoneVec.to_string());
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}

//Remote任务获取
MY_INLINE bool CTaskDataBase::LoadRemote()
{
	ASSERT(m_pDoc);

	TiXmlElement* pRemoteElement = m_pDoc->FirstChildElement(_NODE_RemoteGroup); 
	if (pRemoteElement != NULL)
	{
		for (TiXmlElement* pGroupEle = pRemoteElement->FirstChildElement(_NODE_Group);
			pGroupEle != NULL;
			pGroupEle = pGroupEle->NextSiblingElement(_NODE_Group))
		{
			// 
			LPCSTR lpszStringID = pGroupEle->Attribute(_ATTRIBUTE_ID);
			if (!lpszStringID)
				continue;

			UINT uStringID = (UINT)(ULONG)::StrToIntA(lpszStringID);
			if (!(uStringID >= 0 || uStringID < _MAX_REMOTE_FUNC))
				continue;

			LPCSTR lpszStringName = pGroupEle->Attribute(_ATTRIBUTE_name);
			m_remoteVec[uStringID].SetName(UTF8_to_ANSI(lpszStringName));
			m_remoteVec[uStringID].m_taskVec.clear();

			// 获取任务
			for (TiXmlElement* pTaskElement = pGroupEle->FirstChildElement(_NODE_Task); 
				pTaskElement != NULL; 
				pTaskElement = pTaskElement->NextSiblingElement(_NODE_Task))
			{
				S_DB_TASK task;
				//
				if (GetTaskAttribute(task, pTaskElement->FirstAttribute()))
				{
					m_remoteVec[uStringID].m_taskVec.push_back(task);
				}
				else
				{
					if (_DEV_DEFAULT == task.devType)
					{
						m_remoteVec[uStringID].m_taskVec.push_back(task);
					}
				}
			}
		}
	}
	return true;
}

MY_INLINE bool CTaskDataBase::WriteRemote()
{
	ASSERT(m_pDoc);

	TiXmlElement *pRemoteGroup = m_pDoc->FirstChildElement(_NODE_RemoteGroup);
	if (pRemoteGroup)
	{
		m_pDoc->RemoveChild(pRemoteGroup);
	}

	pRemoteGroup = new TiXmlElement(_NODE_RemoteGroup);
	if (!pRemoteGroup)
	{
		ASSERT(FALSE);
		return false;
	}
	m_pDoc->LinkEndChild(pRemoteGroup);

	int nGroupID = 0;
	for (_DbTaskGroupVec::const_iterator group_iter = m_remoteVec.begin();
		group_iter != m_remoteVec.end();
		group_iter++, nGroupID++)
	{
		TiXmlElement* pGroup = new TiXmlElement(_NODE_Group);
		if (!pGroup)
		{
			ASSERT(FALSE);
			return false;
		}
		pGroup->SetAttribute(_ATTRIBUTE_ID, nGroupID);
		pGroup->SetAttribute(_ATTRIBUTE_name, ANSI_to_UTF8(group_iter->name));
		pRemoteGroup->LinkEndChild(pGroup);

		for (_DbTaskVec::const_iterator task_iter = group_iter->m_taskVec.begin(); 
			task_iter != group_iter->m_taskVec.end();
			task_iter++)
		{
			TiXmlElement* pTask = new TiXmlElement(_NODE_Task);
			if (!pTask)
			{
				ASSERT(FALSE);
				return false;
			}
			SetTaskAttribute(pTask, *task_iter);
			pGroup->LinkEndChild(pTask);
		}
	}

	return m_pDoc->SaveFile(m_XmlFile);
}

MY_INLINE bool CTaskDataBase::LoadTasks()
{
	ASSERT(m_pDoc);
	m_groupVec.clear();

	TiXmlElement *pGroupVecElement = m_pDoc->FirstChildElement(_NODE_GroupVec);
	if (!pGroupVecElement)
		return true;

	for (TiXmlElement* pGroupElement = pGroupVecElement->FirstChildElement(_NODE_Group); 
		pGroupElement != NULL; 
		pGroupElement = pGroupElement->NextSiblingElement(_NODE_Group))
	{
		S_DB_TASK_GROUP group;

		// 获取项目属性
		for (TiXmlAttribute* attribute = pGroupElement->FirstAttribute();
			attribute != NULL; 
			attribute = attribute->Next())
		{
			if (strcmp(attribute->Name(), _ATTRIBUTE_name) == 0)
			{
				strcpy_s(group.name, _MAX_NAME_LEN+1, UTF8_to_ANSI(attribute->Value()));
			}
		}
		// 获取任务
		for (TiXmlElement* pTaskElement = pGroupElement->FirstChildElement(_NODE_Task); 
			pTaskElement != NULL; 
			pTaskElement = pTaskElement->NextSiblingElement(_NODE_Task))
		{
			S_DB_TASK task;
			//
			if (GetTaskAttribute(task, pTaskElement->FirstAttribute()))
			{
				group.m_taskVec.push_back(task);
			}
		}

		m_groupVec.push_back(group);
	}
	return true;
}

MY_INLINE bool CTaskDataBase::WriteTasks()
{
	ASSERT(m_pDoc);

	TiXmlElement *pGroupVec = m_pDoc->FirstChildElement(_NODE_GroupVec);
	if (pGroupVec)
	{
		m_pDoc->RemoveChild(pGroupVec);
	}

	pGroupVec = new TiXmlElement(_NODE_GroupVec);
	if (!pGroupVec)
	{
		ASSERT(FALSE);
		return false;
	}
	m_pDoc->LinkEndChild(pGroupVec);

	for (_DbTaskGroupVec::const_iterator group_iter = m_groupVec.begin();
		group_iter != m_groupVec.end();
		group_iter++)
	{
		TiXmlElement* pGroup = new TiXmlElement(_NODE_Group);
		if (!pGroup)
		{
			ASSERT(FALSE);
			return false;
		}
		pGroup->SetAttribute(_ATTRIBUTE_name, ANSI_to_UTF8(group_iter->name));
		pGroupVec->LinkEndChild(pGroup);

		for (_DbTaskVec::const_iterator task_iter = group_iter->m_taskVec.begin(); 
			task_iter != group_iter->m_taskVec.end();
			task_iter++)
		{
			TiXmlElement* pTask = new TiXmlElement(_NODE_Task);
			if (!pTask)
			{
				ASSERT(FALSE);
				return false;
			}
			SetTaskAttribute(pTask, *task_iter);

			pGroup->LinkEndChild(pTask);
		}
	}

	return m_pDoc->SaveFile(m_XmlFile);
}

void CTaskDataBase::SortTask()
{
	_DbTaskGroupVec::iterator iter = m_groupVec.begin();
	for (;iter != m_groupVec.end(); iter++)
	{
		iter->SortTask();
	}
}

















//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*!
*  /brief 获取xml文件的声明。
*
*  /param XmlFile xml文件全路径。
*  /param strVersion  Version属性值
*  /param strStandalone Standalone属性值
*  /param strEncoding Encoding属性值
*  /return 是否成功。true为成功，false表示失败。
*/
// MY_INLINE bool GetXmlDeclare(std::string XmlFile,
// 				   std::string &strVersion,
// 				   std::string &strStandalone,
// 				   std::string &strEncoding)
// {
// 	// 定义一个TiXmlDocument类指针
// 	TiXmlDocument *pDoc = new TiXmlDocument();
// 	if (NULL==pDoc)
// 	{
// 		return false;
// 	}
// 	pDoc->LoadFile(XmlFile);
// 	  TiXmlNode* pXmlFirst = pDoc->FirstChild();   
// 	  if (NULL != pXmlFirst)  
//      {  
//           TiXmlDeclaration* pXmlDec = pXmlFirst->ToDeclaration();  
//           if (NULL != pXmlDec)  
//           {  
//               strVersion = pXmlDec->Version();
//               strStandalone = pXmlDec->Standalone();
//               strEncoding = pXmlDec->Encoding();
// 	      }
// 	  }
// 	  return true;
// }


/*!
*  /brief 通过根节点和节点名获取节点指针。
*
*  /param pRootEle   xml文件的根节点。
*  /param strNodeName  要查询的节点名
*  /param Node      需要查询的节点指针
*  /return 是否找到。true为找到相应节点指针，false表示没有找到相应节点指针。
*/
// MY_INLINE bool GetNodePointerByName(TiXmlElement* pRootEle,std::string &strNodeName,TiXmlElement* &Node)
// {
// 	 // 假如等于根节点名，就退出
//      if (strNodeName==pRootEle->Value())
//      {
//          Node = pRootEle;
// 		 return true;
//      }
// 	  TiXmlElement* pEle = pRootEle;  
//       for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())  
//     {  
//           //递归处理子节点，获取节点指针
//           if(GetNodePointerByName(pEle,strNodeName,Node))
// 			  return true;
//      }  
// 	 return false;
// }
// 
  /*!
*  /brief 通过节点查询。
*
*  /param XmlFile   xml文件全路径。
*  /param strNodeName  要查询的节点名
*  /param strText      要查询的节点文本
*  /return 是否成功。true为成功，false表示失败。
*/
// MY_INLINE bool QueryNode_Text(std::string XmlFile,std::string strNodeName,std::string &strText)
// {
// 	// 定义一个TiXmlDocument类指针
// 	TiXmlDocument *pDoc = new TiXmlDocument();
// 	if (NULL==pDoc)
// 	{
// 		return false;
// 	}
// 	pDoc->LoadFile(XmlFile);
// 	TiXmlElement *pRootEle = pDoc->RootElement();
// 	if (NULL==pRootEle)
// 	{
// 		return false;
// 	}
//    TiXmlElement *pNode = NULL;
//    GetNodePointerByName(pRootEle,strNodeName,pNode);
//    if (NULL!=pNode)
//    {
//         strText = pNode->GetText(); 
// 		return true;
//    }
//    else
//    {
// 	    return false;
//    }
// 	
// }
// 
/*!
*  /brief 通过节点查询。
*
*  /param XmlFile   xml文件全路径。
*  /param strNodeName  要查询的节点名
*  /param AttMap      要查询的属性值，这是一个map，前一个为属性名，后一个为属性值
*  /return 是否成功。true为成功，false表示失败。
*/
// MY_INLINE bool QueryNode_Attribute(std::string XmlFile,std::string strNodeName,std::map<std::string,std::string> &AttMap)
// {
// 	// 定义一个TiXmlDocument类指针
//     typedef std::pair <std::string,std::string> String_Pair;
// 	TiXmlDocument *pDoc = new TiXmlDocument();
// 	if (NULL==pDoc)
// 	{
// 		return false;
// 	}
// 	pDoc->LoadFile(XmlFile);
// 	TiXmlElement *pRootEle = pDoc->RootElement();
// 	if (NULL==pRootEle)
// 	{
// 		return false;
// 	}
// 	TiXmlElement *pNode = NULL;
// 	GetNodePointerByName(pRootEle,strNodeName,pNode);
// 	if (NULL!=pNode)
// 	{
// 		TiXmlAttribute* pAttr = NULL; 
// 		for (pAttr = pNode->FirstAttribute(); pAttr; pAttr = pAttr->Next())  
// 		{  
// 			std::string strAttName = pAttr->Name();
// 			std::string strAttValue = pAttr->Value();
// 			AttMap.insert(String_Pair(strAttName,strAttValue));
// 		}  
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// 	return true;
// }
// 
/*!
*  /brief 删除指定节点的值。
*
*  /param XmlFile xml文件全路径。
*  /param strNodeName 指定的节点名。
*  /return 是否成功。true为成功，false表示失败。
*/
// MY_INLINE bool DelNode(std::string XmlFile,std::string strNodeName)
// {
// 	// 定义一个TiXmlDocument类指针
// 	TiXmlDocument *pDoc = new TiXmlDocument();
// 	if (NULL==pDoc)
// 	{
// 		return false;
// 	}
// 	pDoc->LoadFile(XmlFile);
// 	TiXmlElement *pRootEle = pDoc->RootElement();
// 	if (NULL==pRootEle)
// 	{
// 		return false;
// 	}
// 	TiXmlElement *pNode = NULL;
// 	GetNodePointerByName(pRootEle,strNodeName,pNode);
// 	// 假如是根节点
// 	if (pRootEle==pNode)
// 	{
//           if(pDoc->RemoveChild(pRootEle))
// 		  {
//                pDoc->SaveFile(XmlFile);
// 			   return true;
// 		  }
// 		  else 
// 			  return false;
// 	}
// 	// 假如是其它节点
// 	if (NULL!=pNode)
// 	{
// 		TiXmlNode *pParNode =  pNode->Parent();
// 		if (NULL==pParNode)
// 		{
//             return false;
// 		}
// 			
// 		TiXmlElement* pParentEle = pParNode->ToElement();
// 		if (NULL!=pParentEle)
// 		{
//             if(pParentEle->RemoveChild(pNode))
//                  pDoc->SaveFile(XmlFile);
// 			else
// 				return false;
// 		}
// 	}
// 	else
// 	{
//           return false;
// 	}
// 	 return false;
// }
// 
//////////////////////////////////////////////////////////////////////////