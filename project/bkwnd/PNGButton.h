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

// 图片按钮
// 图像带顺序： 按钮：		0-正常，1-鼠标滑过，2-鼠标按下，3-按钮禁止
//				CheckBtn：	0-未选中，1-未选中下鼠标滑过，2-选中，3-选中下鼠标滑过，4-按钮禁止
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

