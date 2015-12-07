#pragma once
#include "..\publish\encrypt\DES.h"

class CMyEncrypt :
	public CDES
{
public:
	CMyEncrypt(void);
	~CMyEncrypt(void);

	char skey[32];

	bool _Encrypt(char *Out, char *In, long datalen);
	bool _Decrypt(char *Out, char *In, long datalen);
};
