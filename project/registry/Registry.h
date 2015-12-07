// This class is exported from the Registry.dll
class CRegistry {
public:
	CRegistry(void);
	~CRegistry(void);

	// Set regsitry key name to be used by CWinApp's
	// profile member functions; prevents writing to an INI file.
	void SetRegistryKey(LPCTSTR lpszRegistryKey);
	void SetProfileName(LPCTSTR lpszProfileName);
	// helpers for registration
	HKEY GetRegistryKey();
	HKEY GetSectionKey(LPCTSTR lpszSection);

	// Retrieve an integer value from INI file or registry.
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);

	// Sets an integer value to INI file or registry.
	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);

	// Retrieve a string value from INI file or registry.
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
		LPCTSTR lpszDefault = NULL);

	// Sets a string value to INI file or registry.
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
		LPCTSTR lpszValue);

	// Retrieve an arbitrary binary value from INI file or registry.
	BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
		LPBYTE* ppData, UINT* pBytes);

	// Sets an arbitrary binary value to INI file or registry.
	BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
		LPBYTE pData, UINT nBytes);

	//hKey: HKEY_CURRENT_USER ...
	//lpSubKey: Only Exist in the register, else will course error.
	//lpAppName: not null
	void SetRegOpenKey(HKEY hKey = HKEY_CURRENT_USER, LPCTSTR lpSubKey = _T("software"), LPCTSTR lpAppName = _T("YJCG"));
public:
	// Name of registry key . See SetRegistryKey() member function.
	LPCTSTR m_pszRegistryKey;
	LPCTSTR m_pszProfileName;

	//RegOpenKeyEx arguments
	HKEY m_hKey; 
	LPCTSTR m_lpSubKey;
	LPCTSTR m_lpAppName;
};
