#pragma once

class CAppExt
{
public:

	CAppExt(void)
	{
	}

	~CAppExt(void)
	{
	}

	static CString GetModuleDirectory(void)
	{
		TCHAR szFull[_MAX_PATH];
		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szDir[_MAX_DIR];

		::GetModuleFileName(NULL, szFull, sizeof(szFull)/sizeof(TCHAR));
		_tsplitpath_s(szFull, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, NULL, 0, NULL, 0);
		_tcscpy_s(szFull, MAX_PATH, szDrive);
		_tcscat_s(szFull, MAX_PATH, szDir);

		return CString(szFull);
	}

	static TCHAR* WSAErrorToString(int nErrorCode)
	{
		TCHAR* pszMsg = NULL;
		if (nErrorCode != 0)
		{
			switch(nErrorCode)
			{
			case WSAEADDRINUSE: 
				pszMsg = _T("The specified address is already in use.\n");
				break;
			case WSAEADDRNOTAVAIL: 
				pszMsg = _T("The specified address is not available from the local machine.\n");
				break;
			case WSAEAFNOSUPPORT: 
				pszMsg = _T("Addresses in the specified family cannot be used with this socket.\n");
				break;
			case WSAECONNREFUSED: 
				pszMsg = _T("The attempt to connect was forcefully rejected.\n");
				break;
			case WSAEDESTADDRREQ: 
				pszMsg = _T("A destination address is required.\n");
				break;
			case WSAEFAULT: 
				pszMsg = _T("The lpSockAddrLen argument is incorrect.\n");
				break;
			case WSAEINVAL: 
				pszMsg = _T("The socket is already bound to an address.\n");
				break;
			case WSAEISCONN: 
				pszMsg = _T("The socket is already connected.\n");
				break;
			case WSAEMFILE: 
				pszMsg = _T("No more file descriptors are available.\n");
				break;
			case WSAENETUNREACH: 
				pszMsg = _T("The network cannot be reached from this host at this time.\n");
				break;
			case WSAENOBUFS: 
				pszMsg = _T("No buffer space is available. The socket cannot be connected.\n");
				break;
			case WSAENOTCONN: 
				pszMsg = _T("The socket is not connected.\n");
				break;
			case WSAENOTSOCK: 
				pszMsg = _T("The descriptor is a file, not a socket.\n");
				break;
			case WSAETIMEDOUT: 
				pszMsg = _T("The attempt to connect timed out without establishing a connection. \n");
				break;
			default:
				static TCHAR szError[256];
				_stprintf_s(szError, 256, _T("OnConnect error: %d"), nErrorCode);
				pszMsg = szError;
				break;
			}
			return pszMsg;
		}
		return NULL;
	}
};

#ifndef _CONSOLE
	inline int SetComboBoxCurSel(CComboBox& wndCombo, DWORD dwData)
	{
		for (int iItem=0; iItem < wndCombo.GetCount(); iItem++)
		{
			if (wndCombo.GetItemData(iItem) == dwData)
			{
				wndCombo.SetCurSel(iItem);
				return iItem;
			}
		}
		wndCombo.SetCurSel(-1);
		return -1;
	}
#endif
