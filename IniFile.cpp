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

CString CIniFile::Encrypt(CString S, WORD Key) // ���ܺ���
{
	CString Result,str;
	int i,j;

	Result=S;									// ��ʼ������ַ���
	for(i=0; i<S.GetLength(); i++)				// ���ζ��ַ����и��ַ����в���
	{
		Result.SetAt(i, S.GetAt(i)^(Key>>8));	 // ����Կ��λ�����ַ����
		Key = ((BYTE)Result.GetAt(i)+Key)*C1+C2; // ������һ����Կ
	}
	S=Result;		// ������
	Result.Empty(); // ������
	for(i=0; i<S.GetLength(); i++) // �Լ��ܽ������ת��
	{
		j=(BYTE)S.GetAt(i); // ��ȡ�ַ�
		// ���ַ�ת��Ϊ������ĸ����
		str=_T("12"); // ����str����Ϊ2
		str.SetAt(0, 65+j/26);//���ｫ65�Ĵ���������256�����ľͻ�����룬Ч�����ã���Ӧ�ģ����ܴ�Ҫ��Ϊ��ͬ����
		str.SetAt(1, 65+j%26);
		Result += str;
	}
	return Result;
}

CString CIniFile::Decrypt(CString S, WORD Key) // ���ܺ���
{
	CString Result,str;
	int i,j;

	Result.Empty(); // ������
	for(i=0; i < S.GetLength()/2; i++) // ���ַ���������ĸһ����д���
	{
		j = (((BYTE)S.GetAt(2*i)-65)*26);//��Ӧ�ģ����ܴ�Ҫ��Ϊ��ͬ����

		j += (BYTE)S.GetAt(2*i+1)-65;
		str=_T("1"); // ����str����Ϊ1
		str.SetAt(0, j);
		Result+=str; // ׷���ַ�����ԭ�ַ���
	}
	S=Result; // �����м���
	for(i=0; i<S.GetLength(); i++) // ���ζ��ַ����и��ַ����в���
	{
		Result.SetAt(i, (BYTE)S.GetAt(i)^(Key>>8)); // ����Կ��λ�����ַ����
		Key = ((BYTE)S.GetAt(i)+Key)*C1+C2; // ������һ����Կ
	}
	return Result;
}