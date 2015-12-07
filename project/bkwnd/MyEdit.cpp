// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MyEdit.h"


// CEditEx

IMPLEMENT_DYNAMIC(CEditEx, CEdit)

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, &CEditEx::OnEnChange)
END_MESSAGE_MAP()

void CEditEx::OnEnChange()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEdit::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString szEdit;
	this->GetWindowText(szEdit);
	int nLenDef = szEdit.GetLength();
	int nRet = GetStringSpecifiedMaxLen(szEdit, m_nMaxTextLen);
	if (nLenDef > nRet)
	{
		this->SetWindowText(szEdit);
	}
}

// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEditEx)

CMyEdit::CMyEdit()
{

}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEditEx)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CMyEdit message handlers



void CMyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_TAB:
		GetParent()->SetFocus();
		break;
	case VK_RETURN:
		GetParent()->SetFocus();
		break;
	}
	CEditEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CMyEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			GetParent()->SetFocus();
			this->ShowWindow(SW_HIDE);
			return TRUE;
		} 
		else if (pMsg->wParam == VK_ESCAPE)
		{
			this->SetWindowText(m_szDefault);
			GetParent()->SetFocus();
			this->ShowWindow(SW_HIDE);
			return TRUE;
		}
	}

	return CEditEx::PreTranslateMessage(pMsg);
}

