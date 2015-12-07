#pragma once
#include <atlcoll.h>

class StringPool
{
public:
	StringPool() {}

	virtual ~StringPool()
	{
		m_mapImgPath.RemoveAll();
	}

	typedef CAtlMap<UINT, CString> _TypeImagePath;
	static LPCTSTR const Get(UINT uResID)
	{
		_TypeImagePath::CPair* pPairRet = _Instance()->m_mapImgPath.Lookup(uResID);

		if (NULL != pPairRet)
		{
			return pPairRet->m_value.GetBuffer();
		}
		else
		{
			ASSERT(FALSE);
			return _T("nil");
		}
	}

	static void _ExitInstance()
	{
		if (ms_pInstance)
		{
			delete ms_pInstance;
			ms_pInstance = NULL;
		}
	}

	// pStr 为常量，不能为临时变量
	static void AddString(UINT uResID, LPCTSTR pStr)
	{
		_Instance()->m_mapImgPath[uResID] =  pStr;
	}

	static BOOL Load(const char* pStrXml=NULL)
	{
		ASSERT(pStrXml!=NULL);
		TiXmlDocument xmlDoc;
		xmlDoc.LoadFile(pStrXml);

		if (xmlDoc.Error())
			return FALSE;

		LPCSTR lpszStringID = NULL;
		UINT uStringID = 0;

		TiXmlElement *pXmlStringRootElem = xmlDoc.RootElement();

		if (!pXmlStringRootElem)
			return FALSE;

		if (strcmp(pXmlStringRootElem->Value(), "string") != 0)
			return FALSE;

		for (TiXmlElement* pXmlChild = pXmlStringRootElem->FirstChildElement("s"); NULL != pXmlChild; pXmlChild = pXmlChild->NextSiblingElement("s"))
		{
			lpszStringID = pXmlChild->Attribute("id");
			if (!lpszStringID)
				continue;

			uStringID = (UINT)(ULONG)::StrToIntA(lpszStringID);
			_Instance()->m_mapImgPath[uStringID] = CA2T(pXmlChild->GetText(), CP_UTF8);
		}

		return TRUE;
	}



protected:
	_TypeImagePath m_mapImgPath;
	static StringPool* ms_pInstance;

	static StringPool* _Instance()
	{
		if (!ms_pInstance)
			ms_pInstance = new StringPool;
		return ms_pInstance;
	}
};

__declspec(selectany) StringPool* StringPool::ms_pInstance = NULL;