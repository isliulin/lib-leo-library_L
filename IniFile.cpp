#include "StdAfx.h"
#include ".\inifile.h"



CIniFile::CIniFile()
{
	TCHAR szAppName[MAX_PATH];
	int  len;

	::GetModuleFileName(NULL, szAppName, MAX_PATH);
	len = wcslen(szAppName);
	for(int i=len; i>0; i--)
	{
		if(szAppName[i] == '.')
		{
			szAppName[i+1]='\0';
			break;
		}
	}
	wcscat(szAppName, _T("ini"));
	IniFileName = szAppName;
}

CIniFile::~CIniFile()
{

}

CString CIniFile::GetString(CString AppName,CString KeyName,CString Default)
{
	TCHAR buf[MAX_LENGTH];
	::GetPrivateProfileString(AppName, KeyName, Default, buf, sizeof(buf), IniFileName);
	return buf;
}

double CIniFile::GetDouble(CString AppName,CString KeyName,double Default)
{
	TCHAR buf[MAX_LENGTH];
	CString temp;
	temp.Format(_T("%f"),Default);
	::GetPrivateProfileString(AppName, KeyName, temp, buf, sizeof(buf), IniFileName);
	return _wtof(buf);
}

int CIniFile::GetInt(CString AppName,CString KeyName,int Default)
{
	return ::GetPrivateProfileInt(AppName, KeyName, Default, IniFileName);
}

unsigned long CIniFile::GetDWORD(CString AppName,CString KeyName,unsigned long Default)
{
	TCHAR buf[MAX_LENGTH];
	CString temp;
	temp.Format(_T("%u"),Default);
	::GetPrivateProfileString(AppName, KeyName, temp, buf, sizeof(buf), IniFileName);
	return _wtol(buf);
}




BOOL CIniFile::SetString(CString AppName,CString KeyName,CString Data)
{
	return ::WritePrivateProfileString(AppName, KeyName, Data, IniFileName);
}

BOOL CIniFile::SetInt(CString AppName,CString KeyName,int Data)
{
	CString temp;
	temp.Format(_T("%d"),Data);
	return ::WritePrivateProfileString(AppName, KeyName, temp, IniFileName);
}

BOOL CIniFile::SetDouble(CString AppName,CString KeyName,double Data)
{
	CString temp;
	temp.Format(_T("%f"),Data);
	return ::WritePrivateProfileString(AppName, KeyName, temp, IniFileName);
}

BOOL CIniFile::SetDWORD(CString AppName,CString KeyName,unsigned long Data)
{
	CString temp;
	temp.Format(_T("%u"),Data);
	return ::WritePrivateProfileString(AppName, KeyName, temp, IniFileName);
}

CString CIniFile::Encrypt(CString S, WORD Key) // 加密函数
{
	CString Result,str;
	int i,j;

	Result=S;									// 初始化结果字符串
	for(i=0; i<S.GetLength(); i++)				// 依次对字符串中各字符进行操作
	{
		Result.SetAt(i, S.GetAt(i)^(Key>>8));	 // 将密钥移位后与字符异或
		Key = ((BYTE)Result.GetAt(i)+Key)*C1+C2; // 产生下一个密钥
	}
	S=Result;		// 保存结果
	Result.Empty(); // 清除结果
	for(i=0; i<S.GetLength(); i++) // 对加密结果进行转换
	{
		j=(BYTE)S.GetAt(i); // 提取字符
		// 将字符转换为两个字母保存
		str=_T("12"); // 设置str长度为2
		str.SetAt(0, 65+j/26);//这里将65改大点的数例如256，密文就会变乱码，效果更好，相应的，解密处要改为相同的数
		str.SetAt(1, 65+j%26);
		Result += str;
	}
	return Result;
}

CString CIniFile::Decrypt(CString S, WORD Key) // 解密函数
{
	CString Result,str;
	int i,j;

	Result.Empty(); // 清除结果
	for(i=0; i < S.GetLength()/2; i++) // 将字符串两个字母一组进行处理
	{
		j = (((BYTE)S.GetAt(2*i)-65)*26);//相应的，解密处要改为相同的数

		j += (BYTE)S.GetAt(2*i+1)-65;
		str=_T("1"); // 设置str长度为1
		str.SetAt(0, j);
		Result+=str; // 追加字符，还原字符串
	}
	S=Result; // 保存中间结果
	for(i=0; i<S.GetLength(); i++) // 依次对字符串中各字符进行操作
	{
		Result.SetAt(i, (BYTE)S.GetAt(i)^(Key>>8)); // 将密钥移位后与字符异或
		Key = ((BYTE)S.GetAt(i)+Key)*C1+C2; // 产生下一个密钥
	}
	return Result;
}