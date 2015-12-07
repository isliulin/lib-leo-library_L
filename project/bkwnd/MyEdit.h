#pragma once


inline int GetStringSpecifiedMaxLen(CString& rString, int nMaxLen=16)	//��ȡxx�ֽ�
{
	//��ȡxx�ֽ�
#ifdef UNICODE
	while(WideCharToMultiByte(CP_ACP, 0, rString, rString.GetLength(), NULL, 0, NULL, NULL) >= nMaxLen)
#else
	while(rString.GetLength() >= nMaxLen)
#endif 
	{
		rString.Truncate(rString.GetLength()-1);
	}
	return rString.GetLength();
}


class CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)

public:
	CEditEx() {m_nMaxTextLen=32;}
	virtual ~CEditEx() {}

protected:
	DECLARE_MESSAGE_MAP()
	int m_nMaxTextLen;
public:
	afx_msg void OnEnChange();
	void SetLimitTextByChars(UINT nMax = 32) // �������Ƴ���(���ֽڽ�)
	{
		m_nMaxTextLen = nMax;
	}
};

// CMyEdit

class CMyEdit : public CEditEx
{
	DECLARE_DYNAMIC(CMyEdit)

public:
	CMyEdit();
	virtual ~CMyEdit();

protected:
	DECLARE_MESSAGE_MAP()
	CString m_szDefault;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	void SetDefaultString(CString str)
	{
		m_szDefault = str;
		if (this->GetSafeHwnd())
		{
			this->SetWindowText(m_szDefault);
		}
	}
};
