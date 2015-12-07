//////////////////////////////////////////////////////////////////////////
//  Class Name: BKBmpPool
// Description: Bitmap Pool
//     Creator: ZhangXiaoxuan
//     Version: 2009.4.24 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlcoll.h>

class ResBmpPool
{
public:
	ResBmpPool()
	{
	}
	virtual ~ResBmpPool()
	{
		HBITMAP hBmp = NULL;
		POSITION pos = m_mapBitmap.GetStartPosition();
		while (pos != NULL) 
		{
			if (hBmp = m_mapBitmap.GetNextValue(pos))
			{
				::DeleteObject(hBmp);
			}
		}

		m_mapBitmap.RemoveAll();
	}

	typedef CAtlMap<UINT, HBITMAP> _TypeBitmapPool;

	static HBITMAP GetBitmap(UINT uResID)
	{
		_TypeBitmapPool::CPair* pPairRet = _Instance()->m_mapBitmap.Lookup(uResID);
		if (NULL != pPairRet)
		{
			return pPairRet->m_value;
		}
		else
		{
			HBITMAP hbmp = NULL;
			if (LoadBmpFromResource(uResID, hbmp))
			{
				_Instance()->m_mapBitmap[uResID] = hbmp;
				return _Instance()->m_mapBitmap[uResID];
			} 
			else
			{
				ASSERT(FALSE);
				return NULL;
			}
		}
	}

	static size_t GetCount()
	{
		return _Instance()->m_mapBitmap.GetCount();
	}

	static void _ExitInstance()
	{
		if (ms_pInstance)
		{
			delete ms_pInstance;
			ms_pInstance = NULL;
		}
	}
protected:
	_TypeBitmapPool m_mapBitmap;
	static ResBmpPool* ms_pInstance;

	static ResBmpPool* _Instance()
	{
		if (!ms_pInstance)
			ms_pInstance = new ResBmpPool;
		return ms_pInstance;
	}
	static BOOL LoadBmpFromResource(UINT uResID, HBITMAP& hbmp){
#ifdef _IMAGEPOOL_LOADFROMFILE
		hbmp = (HBITMAP)::LoadImage(NULL,StringPool::Get(uResID),IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);
		if (hbmp)
		{
			return TRUE;
		}
#else
		HBITMAP hbmp;
		HINSTANCE hInst = AfxGetResourceHandle();
		::LoadBitmap(hInst, hbmp);
		if (hbmp)
		{
			return TRUE;
		}
#endif
		return FALSE;
	}
};

__declspec(selectany) ResBmpPool* ResBmpPool::ms_pInstance = NULL;