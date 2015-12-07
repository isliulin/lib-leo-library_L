#pragma once

class CTrayIcon
{
	// Construction/destruction
public:
	CTrayIcon();
	CTrayIcon(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID);
	virtual ~CTrayIcon();

	// Operations
public:
	BOOL Enabled() { return m_bEnabled; }
	BOOL Visible() { return !m_bHidden; }

	//Create the tray icon
	int Create(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID);

	//Change or retrieve the Tooltip text
	BOOL    SetTooltipText(LPCTSTR pszTooltipText);
	BOOL    SetTooltipText(UINT nID);
	CString GetTooltipText() const;

	//Change or retrieve the icon displayed
	BOOL  SetTrayIcon(HICON hIcon);
	BOOL  SetTrayIcon(LPCTSTR lpIconName);
	BOOL  SetTrayIcon(UINT nIDResource);
	BOOL  SetStandardIcon(LPCTSTR lpIconName);
	BOOL  SetStandardIcon(UINT nIDResource);
	HICON GetTrayIcon() const;
	void  HideTrayIcon();
	void  ShowTrayIcon();
	void  RemoveTrayIcon();
	void  MoveToRight();

	//Change or retrieve the window to send notification messages to
	BOOL  SetNotificationWnd(CWnd* pNotifyWnd);
	CWnd* GetNotificationWnd() const;

	//Default handler for tray notification message
	virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayIcon)
	//}}AFX_VIRTUAL

	// Implementation
protected:
	BOOL			m_bEnabled;	// does O/S support tray icon?
	BOOL			m_bHidden;	// Has the icon been hidden?
	CWnd *			m_parent;
public:
	NOTIFYICONDATA	m_tnd;
};

inline CTrayIcon::CTrayIcon()
{
	memset(&m_tnd, 0, sizeof(m_tnd));
	m_bEnabled = FALSE;
	m_bHidden  = FALSE;
}

inline CTrayIcon::CTrayIcon(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szToolTip, 
					 HICON icon, UINT uID)
{
	Create(pWnd, uCallbackMessage, szToolTip, icon, uID);
	m_bHidden = FALSE;
}

inline BOOL CTrayIcon::Create(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szToolTip, 
					   HICON icon, UINT uID)
{
	// this is only for Windows 95 (or higher)
	VERIFY(m_bEnabled = ( GetVersion() & 0xff ) >= 4);
	if (!m_bEnabled) return FALSE;

	//Make sure Notification window is valid
	VERIFY(m_bEnabled = (pWnd && ::IsWindow(pWnd->GetSafeHwnd())));
	if (!m_bEnabled) return FALSE;

	//Make sure we avoid conflict with other messages
	ASSERT(uCallbackMessage >= WM_USER);

	//Tray only supports tooltip text up to 64 characters
	ASSERT(_tcslen(szToolTip) <= 64);

	m_parent	 = pWnd;
	// load up the NOTIFYICONDATA structure
	m_tnd.cbSize = sizeof(NOTIFYICONDATA);
	m_tnd.hWnd	 = pWnd->GetSafeHwnd();
	m_tnd.uID	 = uID;
	m_tnd.hIcon  = icon;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = uCallbackMessage;
	_tcscpy_s (m_tnd.szTip, 128, szToolTip);

	// Set the tray icon
	VERIFY(m_bEnabled = Shell_NotifyIcon(NIM_ADD, &m_tnd));
	return m_bEnabled;
}

inline CTrayIcon::~CTrayIcon()
{
	RemoveTrayIcon();
}


/////////////////////////////////////////////////////////////////////////////
// CTrayIcon icon manipulation

inline void CTrayIcon::MoveToRight()
{
	HideTrayIcon();
	ShowTrayIcon();
}

inline void CTrayIcon::RemoveTrayIcon()
{
	if (!m_bEnabled) return;

	m_tnd.uFlags = 0;
	Shell_NotifyIcon(NIM_DELETE, &m_tnd);
	m_bEnabled = FALSE;
}

inline void CTrayIcon::HideTrayIcon()
{
	if (m_bEnabled && !m_bHidden) {
		m_tnd.uFlags = NIF_ICON;
		Shell_NotifyIcon (NIM_DELETE, &m_tnd);
		m_bHidden = TRUE;
	}
}

inline void CTrayIcon::ShowTrayIcon()
{
	if (m_bEnabled && m_bHidden) {
		m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		Shell_NotifyIcon(NIM_ADD, &m_tnd);
		m_bHidden = FALSE;
	}
}

inline BOOL CTrayIcon::SetTrayIcon(HICON hIcon)
{
	if (!m_bEnabled) return FALSE;

	m_tnd.uFlags = NIF_ICON;
	m_tnd.hIcon = hIcon;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

inline BOOL CTrayIcon::SetTrayIcon(LPCTSTR lpszIconName)
{
	HICON hIcon = AfxGetApp()->LoadIcon(lpszIconName);

	return SetTrayIcon(hIcon);
}

inline BOOL CTrayIcon::SetTrayIcon(UINT nIDResource)
{
	HICON hIcon = AfxGetApp()->LoadIcon(nIDResource);

	return SetTrayIcon(hIcon);
}

inline BOOL CTrayIcon::SetStandardIcon(LPCTSTR lpIconName)
{
	HICON hIcon = LoadIcon(NULL, lpIconName);

	return SetTrayIcon(hIcon);
}

inline BOOL CTrayIcon::SetStandardIcon(UINT nIDResource)
{
	HICON hIcon = LoadIcon(NULL, MAKEINTRESOURCE(nIDResource));

	return SetTrayIcon(hIcon);
}

inline HICON CTrayIcon::GetTrayIcon() const
{
	HICON hIcon = NULL;
	if (m_bEnabled)
		hIcon = m_tnd.hIcon;

	return hIcon;
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon tooltip text manipulation

inline BOOL CTrayIcon::SetTooltipText(LPCTSTR pszTip)
{
	if (!m_bEnabled) return FALSE;

	m_tnd.uFlags = NIF_TIP;
	_tcscpy_s(m_tnd.szTip, 128, pszTip);

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

inline BOOL CTrayIcon::SetTooltipText(UINT nID)
{
	CString strText;
	VERIFY(strText.LoadString(nID));

	return SetTooltipText(strText);
}

inline CString CTrayIcon::GetTooltipText() const
{
	CString strText;
	if (m_bEnabled)
		strText = m_tnd.szTip;

	return strText;
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon notification window stuff

inline BOOL CTrayIcon::SetNotificationWnd(CWnd* pWnd)
{
	if (!m_bEnabled) return FALSE;

	//Make sure Notification window is valid
	ASSERT(pWnd && ::IsWindow(pWnd->GetSafeHwnd()));

	m_tnd.hWnd = pWnd->GetSafeHwnd();
	m_tnd.uFlags = 0;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

inline CWnd* CTrayIcon::GetNotificationWnd() const
{
	return CWnd::FromHandle(m_tnd.hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon implentation of OnTrayNotification

inline LRESULT CTrayIcon::OnTrayNotification(UINT wParam, LONG lParam) 
{
	//Return quickly if its not for this tray icon
	if (wParam != m_tnd.uID)
		return 0L;

	CMenu menu, *pSubMenu;
	CPoint pos;
	switch(lParam)
	{
	case WM_RBUTTONDOWN:
		if (!menu.LoadMenu(m_tnd.uID)) return 0;
		if (!(pSubMenu = menu.GetSubMenu(0))) return 0;

		// Make first menu item the default (bold font)
		::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);

		//Display and track the popup menu
		GetCursorPos(&pos);
		::SetForegroundWindow(m_tnd.hWnd);  
		::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y, 0, m_tnd.hWnd, NULL);

		//pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this, NULL);
		menu.DestroyMenu();
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		// 		if (!menu.LoadMenu(m_tnd.uID)) return 0;
		// 		if (!(pSubMenu = menu.GetSubMenu(0))) return 0;
		// 
		// 		// double click received, the default action is to execute first menu item
		// 		::SetForegroundWindow(m_tnd.hWnd);
		// 		::SendMessage(m_tnd.hWnd, WM_COMMAND, pSubMenu->GetMenuItemID(0), 0);
		// 		menu.DestroyMenu();
// 		if (m_parent->IsWindowVisible())
// 		{
// 			m_parent->ShowWindow(SW_HIDE);
// 		} 
// 		else
// 		{
// 			m_parent->ShowWindow(SW_SHOW);
// 		}
		break;
	}
	return 1;
}