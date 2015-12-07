//////////////////////////////////////////////////////////////////////////
//  Class Name: ResImagePool
// Description: Image Pool
//     Creator: YuJC
//     Version: 2011.12.21 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlcoll.h>
#include <Gdiplus.h>
#include "StringPool.h"

class ResImagePool
{
public:
	ResImagePool()
	{
	}

	virtual ~ResImagePool()
	{
		Gdiplus::Image* pImg;
		POSITION pos = m_mapPng.GetStartPosition();
		while (pos != NULL) 
		{
			if (pImg = m_mapPng.GetNextValue(pos))
				delete pImg;
		}

		m_mapPng.RemoveAll();
	}

	typedef CAtlMap<UINT, Gdiplus::Image*> _TypeImagePool;

	static Gdiplus::Image* Get(UINT uResID)
	{
		_TypeImagePool::CPair* pPairRet = _Instance()->m_mapPng.Lookup(uResID);

		if (NULL != pPairRet)
		{
			return pPairRet->m_value;
		}
		else
		{
			Gdiplus::Image* pImage = NULL;
			if (LoadPngFromResource(uResID, pImage))
			{
				_Instance()->m_mapPng[uResID] = pImage;
				return _Instance()->m_mapPng[uResID];
			} 
			else
			{
				return NULL;
			}
		}
	}

	static BOOL Draw(CDC* pDC, UINT uResID, CRect* pRectDest, CRect* pRectSrc = NULL);

	static size_t GetCount()
	{
		return _Instance()->m_mapPng.GetCount();
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
	_TypeImagePool m_mapPng;
	static ResImagePool* ms_pInstance;

	static ResImagePool* _Instance()
	{
		if (!ms_pInstance)
		{
			ms_pInstance = new ResImagePool;
		}
		return ms_pInstance;
	}

	static BOOL LoadPngFromResource(UINT uResID, Gdiplus::Image* &pImage);
};

__declspec(selectany) ResImagePool* ResImagePool::ms_pInstance = NULL;


inline BOOL _DrawImage(CDC* pDC, Gdiplus::Image* pImg, CRect rcDest, CRect rcSource)
{
	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	Gdiplus::Rect dest_rect(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height());
	Gdiplus::Rect source_rect(rcSource.left, rcSource.top, rcSource.Width(), rcSource.Height());
	if (Gdiplus::Ok == 
		graphics.DrawImage(pImg, dest_rect, source_rect.GetLeft(), source_rect.GetTop(), source_rect.Width, source_rect.Height, Gdiplus::UnitPixel)
		)
		return TRUE;
	else
		return FALSE;
}

inline BOOL _DrawImage(CDC* pDC, Gdiplus::Image* pImg, CRect rcDest)
{
	CRect rcSource(0, 0, pImg->GetWidth(), pImg->GetHeight());
	if (rcSource.right > rcDest.Width())
	{
		rcSource.right = rcSource.left + rcDest.Width();
	}
	if (rcSource.bottom > rcDest.Height())
	{
		rcSource.bottom = rcSource.top + rcDest.Height();
	}
	return _DrawImage(pDC,pImg, rcDest, rcSource);
}

inline BOOL _DrawImage(CDC* pDC, Gdiplus::Image* pImg, CRect rcDest, CPoint ptSourceStart)
{
	CRect rcSource(ptSourceStart.x, ptSourceStart.y, pImg->GetWidth(), pImg->GetHeight());
	if (rcSource.Width() > rcDest.Width())
	{
		rcSource.right = rcSource.left + rcDest.Width();
	}
	if (rcSource.Height() > rcDest.Height())
	{
		rcSource.bottom = rcSource.top + rcDest.Height();
	}
	return _DrawImage(pDC,pImg, rcDest, rcSource);
}

inline BOOL ResImagePool::Draw(CDC* pDC, UINT uResID, CRect* pRectDest, CRect* pRectSrc/* = NULL*/)
{
	ASSERT(pRectDest);
	Gdiplus::Image* pImg = ResImagePool::Get(uResID);
	if (!pImg)
	{
		return FALSE;
	}

	if (pRectDest && !pRectDest->IsRectEmpty())
	{
		if (NULL == pRectSrc)
		{
			return  _DrawImage(pDC, pImg, *pRectDest);
		} 
		else
		{
			if (!pRectSrc->IsRectEmpty())
			{
				return _DrawImage(pDC, pImg, *pRectDest, *pRectSrc);
			}
		}
	}
	return FALSE;
}

#ifdef _IMAGEPOOL_LOADFROMFILE

inline BOOL ResImagePool::LoadPngFromResource(UINT uResID, Gdiplus::Image* &pImage)
{
	pImage = Gdiplus::Image::FromFile(StringPool::Get(uResID));
	if (pImage->GetLastStatus() == Gdiplus::Ok)
	{
		return TRUE;
	} 
	else
	{
		pImage = NULL;
		return FALSE;
	}
}

#else

inline BOOL ResImagePool::LoadPngFromResource(UINT uResID, Gdiplus::Image* &pImage)
{
	TRACE1("LoadPngFromResource: %d\n", uResID);

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hResource = ::FindResource (hInst,MAKEINTRESOURCE(uResID), _T("png")); // type
	if (!hResource)
		return FALSE;

	// load resource into memory
	DWORD imageSize = ::SizeofResource(hInst, hResource);
	void* pResourceData = ::LoadResource(hInst, hResource);
	if (!pResourceData)
		return FALSE;

	// Allocate global memory on which to create stream
	HGLOBAL hBuffer = ::GlobalAlloc(GMEM_FIXED, imageSize);
	if (NULL == hBuffer)
		return FALSE;

	void* pBuffer = ::GlobalLock(hBuffer);
	if (pBuffer)
	{
		CopyMemory(pBuffer,pResourceData,imageSize);
		IStream* pStream = NULL;
		if (::CreateStreamOnHGlobal(hBuffer,FALSE,&pStream) == S_OK)
		{
			pImage = Gdiplus::Image::FromStream(pStream);
			pStream->Release();
			if (pImage)
			{
				if (pImage->GetLastStatus() == Gdiplus::Ok
					&& pImage->GetWidth() > 0)
				{
					return TRUE;
				}
				else
				{
					pImage = NULL;
				}
			}
		}
		::GlobalUnlock(hBuffer);
	}

	::GlobalFree(hBuffer);
	return FALSE;
}

#endif
