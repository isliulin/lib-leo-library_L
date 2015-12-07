//////////////////////////////////////////////////////////////////////////
//  Class Name: CImageListEx
// Description: Image draw manager
//     Creator: YuJC
//     Version: 2011.12.21 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <Gdiplus.h>

enum E_AllotMode{
	_AM_Width, // 按指定长度
	_AM_Sub, // 按指定子图片个数
};

class CImageListEx
{
protected:
	Gdiplus::Image* m_pImg;
	E_AllotMode m_allotMode;
	int m_nSubImgCount;
	int m_lSubImageWidth;
public:
	~CImageListEx() {}
	CImageListEx(Gdiplus::Image* pImg = NULL, int iParamter = 3, E_AllotMode allotMode = _AM_Sub) {
		ASSERT(pImg);

		m_pImg = pImg;
		SetParamter(iParamter, allotMode);
	}

	void SetParamter(int iParamter, E_AllotMode allotMode) {
		ASSERT(iParamter>0);
		m_allotMode = allotMode;
		switch (m_allotMode)
		{
		case _AM_Sub:
			m_nSubImgCount = iParamter;
			Parse();
			break;
		case _AM_Width:
			m_lSubImageWidth = iParamter;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	BOOL Draw(HDC hDestDC, int iSubImg, const RECT& rectDest) ;

	bool IsNull(){
		if (m_pImg)
		{
// 			return m_pImg->IsNull();
			return false;
		}
		else
		{
			return true;
		}
	}


protected:
	void Parse() {
		switch (m_allotMode)
		{
		case _AM_Sub:
			m_lSubImageWidth = int(m_pImg->GetWidth() / m_nSubImgCount);
			ASSERT(m_lSubImageWidth>0);
			break;
		case _AM_Width:
			break;
		}
	}
};

inline BOOL CImageListEx::Draw(HDC hDestDC, int iSubImg, const RECT& rectDest) {
	ASSERT(!IsNull());
	ASSERT(iSubImg>=0);
	ASSERT(iSubImg<m_nSubImgCount);

	Gdiplus::Graphics graphics(hDestDC);
	Gdiplus::Rect dest_rect(rectDest.left, rectDest.top, rectDest.right-rectDest.left, rectDest.bottom-rectDest.top);
	Gdiplus::Rect src_rect(m_lSubImageWidth * iSubImg, 0, m_lSubImageWidth, m_pImg->GetHeight());
	graphics.DrawImage(m_pImg, dest_rect, src_rect.X, src_rect.Y, src_rect.Width, src_rect.Height, Gdiplus::UnitPixel);

	return TRUE;
}