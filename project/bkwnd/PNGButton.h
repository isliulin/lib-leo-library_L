#pragma once

class CBkImgWnd : public CWnd
{
	DECLARE_DYNCREATE(CBkImgWnd)
public:
	CBkImgWnd(void){}
	~CBkImgWnd(void){}

	UINT m_imgID;
	virtual BOOL Create(CWnd* pParentWnd, CRect& rect, UINT nID, UINT PngID, CCreateContext* pContext = NULL);
	void SetImg(UINT PngID);
public:
	DECLARE_MESSAGE_MAP()  
	afx_msg void OnPaint();
};

// ͼƬ��ť
// ͼ���˳�� ��ť��		0-������1-��껬����2-��갴�£�3-��ť��ֹ
//				CheckBtn��	0-δѡ�У�1-δѡ������껬����2-ѡ�У�3-ѡ������껬����4-��ť��ֹ
class CPngButton : public CWnd
{
	DECLARE_DYNCREATE(CPngButton)   
public:
	CPngButton(void);
	~CPngButton(void);

	virtual BOOL Create(CWnd* pParentWnd, CRect& rect, UINT nID, UINT PngID, BOOL bCheckButton = FALSE, CCreateContext* pContext = NULL);
	void SetImg(UINT PngID);

	UINT m_imgID;
	enum AFX_BUTTON_STATE
	{
		ButtonsIsRegular,
		ButtonsIsPressed,
		ButtonsIsHighlighted,
	};
	BOOL m_bHover;
	BOOL m_bCaptured;

	BOOL m_bPushed;
	BOOL m_bChecked;
	BOOL m_bCheckButton;
	void SetCheckButton(BOOL bCheckButton)
	{
		m_bCheckButton = bCheckButton;
		m_bChecked = FALSE;
		m_bPushed = FALSE;
		if (GetSafeHwnd())
		{
			this->Invalidate(TRUE);
		}
	}

	void SetCheck(BOOL bChecked)
	{
		m_bChecked = bChecked;
		if (GetSafeHwnd())
		{
			this->Invalidate(TRUE);
		}
	}

	BOOL GetCheck()
	{
		return m_bChecked;
	}

	CFont m_CaptionFont;
	void SetCaptionFont (const LOGFONT& lf);
	HFONT GetCaptionFont () const;
	CString m_strCaption;
	void SetCaption(CString& str)
	{
		m_strCaption = str;
	}

	BOOL m_bFillBackground;
public:
	DECLARE_MESSAGE_MAP()  
	afx_msg void OnPaint();

	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);   
protected:
	void OnDrawFocusRect(CDC* pDC, const CRect& rectClient);
	virtual void FillBackground(CDC* pDC, CRect& rect, AFX_BUTTON_STATE state);
	virtual void DoPaint(CDC* pDC, CRect& rect, AFX_BUTTON_STATE state);
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

