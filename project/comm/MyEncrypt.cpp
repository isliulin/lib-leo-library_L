#include "StdAfx.h"
#include "MyEncrypt.h"


CMyEncrypt::CMyEncrypt(void)
{
	memset(skey, 'a', 32);
	memcpy(skey,"12345678",8);
}

CMyEncrypt::~CMyEncrypt(void)
{
}

bool CMyEncrypt::_Encrypt( char *Out, char *In, long datalen )
{
	//º”√‹
	return DES_EDE2(Out, In, datalen, skey,sizeof(skey), ENCRYPT);
}

bool CMyEncrypt::_Decrypt( char *Out, char *In, long datalen )
{
	//Ω‚√‹
	return DES_EDE2(Out, In, datalen, skey, sizeof(skey), DECRYPT);
}