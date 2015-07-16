#pragma once

#define MAX_LENGTH 256

#define C1 52845
#define C2 22719

class CIniFile
{
public:
	CIniFile();
	virtual ~CIniFile();

	void SetIniFileName(CString FileName)
	{ IniFileName = FileName; }

	CString GetIniFileName()
	{ return IniFileName; }

	CString GetString(CString AppName,CString KeyName,CString Default = _T(""));
	int GetInt(CString AppName,CString KeyName,int Default = 0);
	double GetDouble(CString AppName,CString KeyName,double Default = 0);
	unsigned long GetDWORD(CString AppName,CString KeyName,unsigned long Default = 0);

	BOOL SetString(CString AppName,CString KeyName,CString Data);
	BOOL SetInt(CString AppName,CString KeyName,int Data);
	BOOL SetDouble(CString AppName,CString KeyName,double Data);
	BOOL SetDWORD(CString AppName,CString KeyName,unsigned long Data);

	CString Encrypt(CString S, WORD Key);	//加密函数
	CString Decrypt(CString S, WORD Key);	// 解密函数
private:
	CString IniFileName;
	
};
