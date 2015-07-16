/////////////////////////////////////////////////////////
/*
File name 	:   unicode_transfer.h
Author 		: 	LEO
Date 		:	2015-4-21
Email  		:	lion_117@126.com
All Rights Reserved Licensed under the Apache License
description :
unicode_converter is  portable library which could use to 
to transfer character set betweeen  asciii code  and 
widebyte code. this library didnot relay on MFC  library . 

*/
/////////////////////////////////////////////////////////

#pragma once
#include <iostream>
#include <string>
using namespace std;

class Unicode_converter
{
public:
	static std::string WstringToString(const std::wstring str);
	static std::wstring StringToWstring(const std::string str);
};

inline std::string Unicode_converter::WstringToString(const std::wstring str)
{
	// wstring转string
	unsigned int len = str.size() * 4;
	setlocale(LC_CTYPE, "");
	char *p = new char[len];
	wcstombs_s(&len, p,len, str.c_str(), len );
	std::string str1(p);
	delete[] p;
	return str1;
}

inline std::wstring Unicode_converter::StringToWstring(const std::string str)
{

	unsigned len = str.size() * 2;// 预留字节数
	setlocale(LC_CTYPE, "");     //必须调用此函数
	wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
	mbstowcs_s(&len, p,len, str.c_str(), len);// 转换
	std::wstring str1(p);
	delete[] p;
	return str1;
}
