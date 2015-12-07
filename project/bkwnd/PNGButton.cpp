#include "StdAfx.h"
#include "PNGButton.h"
#include "..\pool\ImagePool.h"


IMPLEMENT_DYNCREATE(CBkImgWnd, CWnd)

BEGIN_MESSAGE_MAP(CBkImgWnd, CWnd)  
	ON_WM_PAINT()
END_MESSAGE_MAP() 

BOOL CBkImgWnd::Create(CWnd* pParentWnd, CRect& rect, UINT nID, UINT PngID, CCreateContext* pContext)
{
	m_imgID = PngID;
// 	LPCTSTR lpszClassName=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW , AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(TRANSPARENT), NULL);   
	DWORD dwStyle = WS_CHILDWINDOW|WS_VISIBLE;
	BOOL OK=CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID, pContext);
	ModifyStyleEx(0, WS_EX_TRANSPARENT); 
	return OK;
}

void CBkImgWnd::SetImg(UINT PngID)
{
	if (ResImagePool::Get(PngID))
	{
		m_imgID = PngID;
	}
	else
	{
		ASSERT(0);
		TRACE("PNGButton::Create 请重新设个一个正确的图片！");
	}
}


void CBkImgWnd::OnPaint()
{  
	CPaintDC dc(this); 
	CRect client_rect;
	GetClientRect(client_rect);

	Gdiplus::Image * pImg = ResImagePool::Get(m_imgID);

	if (pImg)
	{
		_DrawImage(&dc, pImg, client_rect);
	}
	else
	{
		ASSERT(FALSE);
		TRACE("PNGButton 请重新设个一个正确的图片！");
	}
}




IMPLEMENT_DYNCREATE(CPngButton, CWnd)

CPngButton::CPngButton(void) 
{
	m_bHover = FALSE;
	m_bCaptured = FALSE;
	m_bPushed = FALSE;
	m_bChecked = FALSE;
	m_bCheckButton = FALSE;
	m_bFillBackground = TRUE;
} 
CPngButton::~CPngButton(void) { }

BEGIN_MESSAGE_MAP(CPngButton, CWnd)  
	ON_WM_PAINT()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()  
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()  


BOOL CPngButton::Create(CWnd* pParentWnd, CRect& rect, UINT nID, UINT PngID, BOOL bCheckButton, CCreateContext* pContext)
{
	m_bCheckButton = bCheckButton;
	m_imgID = PngID;
// 	if (!ResImagePool::Get(m_imgID))
// 	{
// 		//ASSERT(0);
// 		//TRACE("PNGButton::Create 请重新设个一个正确的图片！");
// 	}
	LPCTSTR lpszClassName=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS , AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(TRANSPARENT), NULL);   
	DWORD dwStyle = WS_CHILDWINDOW|WS_VISIBLE;
	BOOL OK=CWnd::Create(lpszClassName, NULL, dwStyle, rect, pParentWnd, nID, pContext);
	ModifyStyleEx(0, WS_EX_TRANSPARENT); 
	return OK;
}

void CPngButton::SetImg(UINT PngID)
{
	if (ResImagePool::Get(PngID))
	{
		m_imgID = PngID;
	}
	else
	{
		ASSERT(0);
		TRACE("PNGButton::Create 请重新设个一个正确的图片！");
	}
}

void CPngButton::OnPaint()
{  
	CPaintDC dc(this); 
	CRect client_rect;
	GetClientRect(client_rect);
	AFX_BUTTON_STATE state;

	if (m_bHover)
	{
		state = ButtonsIsHighlighted;
	} 
	else
	{
		if (m_bPushed)
		{
			state = ButtonsIsPressed;
		} 
		else
		{
			state = ButtonsIsRegular;
		}
	}

	if (m_bFillBackground)
	{
		FillBackground(&dc, client_rect, state);
	}
	DoPaint(&dc, client_rect, state);
}

void CPngButton::FillBackground(CDC* pDC, CRect& rect, AFX_BUTTON_STATE state)
{
	afxGlobalData.DrawParentBackground(this, pDC, rect);
	return;
}

HFONT CPngButton::GetCaptionFont () const
{
	return (HFONT)m_CaptionFont.GetSafeHandle ();
}

void CPngButton::SetCaptionFont (const LOGFONT& lf)
{
	if (m_CaptionFont.GetSafeHandle () != NULL)
	{
		m_CaptionFont.DeleteObject ();
	}

	m_CaptionFont.CreateFontIndirect (&lf);
}

void CPngButton::DoPaint(CDC* pDC, CRect& rect, AFX_BUTTON_STATE state)
{
	int index = 0;
	int nSubCount = 4;
	if (m_bCheckButton)
	{
		nSubCount = 5;
		index = m_bChecked ? 2 : 0;
		if (m_bHover)
		{
			index += 1;
		}
		if (!IsWindowEnabled())
		{
			index = 4;
		}
	}
	else
	{
		switch (state)
		{
		case ButtonsIsHighlighted:
			index += 1;
			break;
		case ButtonsIsPressed:
			index += 2;
			break;
		}
		if (!IsWindowEnabled())
		{
			index = 3;
		}
	}

	Gdiplus::Image * pImg = ResImagePool::Get(m_imgID);

	if (pImg)
	{
		CImageListEx imgBtn(pImg, nSubCount);
		imgBtn.Draw(pDC->GetSafeHdc(), index, rect);
	}
	else
	{
		ASSERT(FALSE);
		TRACE("PNGButton 请重新设个一个正确的图片！");
	}

	if (!m_strCaption.IsEmpty())
	{
		HGDIOBJ  hOldFont    = pDC->SelectObject (&m_CaptionFont);
		int      nOldMode    = pDC->SetBkMode    (TRANSPARENT);
		COLORREF crTextColor = pDC->SetTextColor (RGB(255,255,255)/*GetCaptionTextColor()*/);
		COLORREF crBkColor   = pDC->SetBkColor   (afxGlobalData.clrBtnFace);

		rect.left += 3;
		pDC->DrawText (m_strCaption, rect, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);

		pDC->SetTextColor (crTextColor);
		pDC->SetBkColor   (crBkColor);
		pDC->SetBkMode    (nOldMode);
		pDC->SelectObject (hOldFont);
	}

// 	if (m_bHover && ! m_bCheckButton)
// 	{
// 		OnDrawFocusRect(pDC, rect);
// 	}
}

void CPngButton::OnDrawFocusRect(CDC* pDC, const CRect& rectClient)
{
	ASSERT_VALID(pDC);

	CRect rectFocus = rectClient;
	rectFocus.DeflateRect(1, 1);

	COLORREF clrBckgr = afxGlobalData.clrBtnFace;

// 	if (!m_bWinXPTheme || m_bDontUseWinXPTheme)
	{
		rectFocus.DeflateRect(1, 1);
		pDC->Draw3dRect(rectFocus, clrBckgr, clrBckgr);
	}
	pDC->DrawFocusRect(rectFocus);
}

void CPngButton::OnMouseHover(UINT nFlags, CPoint point)
{   
	m_bHover = TRUE;
	CWnd::OnMouseHover(nFlags, point);
}


void CPngButton::OnMouseLeave()
{   
	m_bHover = FALSE;
	if (IsWindowEnabled())
	{
		Invalidate();
	}
	CWnd::OnMouseLeave();
}


void CPngButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (IsWindowEnabled())
	{
		SetCursor(::LoadCursor(NULL, m_bCheckButton ? IDC_ARROW : IDC_HAND));
		if(!m_bHover) 
		{ 
			TRACKMOUSEEVENT   tme; 
			tme.cbSize		=   sizeof(TRACKMOUSEEVENT); 
			tme.dwFlags		=   TME_LEAVE|TME_HOVER; //
			tme.hwndTrack   =   GetSafeHwnd(); 
			tme.dwHoverTime	=   80; 
			_TrackMouseEvent(&tme);  
			m_bHover   =   true;    
			//OnMouseHover(nFlags,point);
			Invalidate();
		} 
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CPngButton::OnLButtonDown(UINT nFlags, CPoint point)   
{
	if (IsWindowEnabled())
	{
		m_bPushed = TRUE;
		m_bCaptured = TRUE;
		SetCapture();
	}
	CWnd::OnLButtonDown(nFlags, point);  
}  

void CPngButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsWindowEnabled())
	{
		m_bPushed = FALSE;

		if (m_bCaptured)
		{
			m_bCaptured = FALSE;
			m_bChecked = !m_bChecked;
			ReleaseCapture();
			GetParent()->SendMessage(WM_COMMAND,GetDlgCtrlID()|0,(LONG)GetSafeHwnd());
		}
		Invalidate();
	}
	CWnd::OnLButtonUp(nFlags, point);
}

BOOL CPngButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
