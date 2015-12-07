// DES.h: interface for the CDES class.
// 雨季茶馆 整理
// 2010年5月15日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DES_H__392A1620_7EC8_471F_AF89_913F62B65327__INCLUDED_)
#define AFX_DES_H__392A1620_7EC8_471F_AF89_913F62B65327__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDES  
{
public:
	CDES();
	virtual ~CDES();
public:
	typedef bool (*PSubKey)[16][48];
	enum    {ENCRYPT=0,DECRYPT=1};
	//标准DES 加/解密
	//Type = ENCRYPT加密， DECREYPT 解密
	void DES_Original(char Out[8], char In[8], const PSubKey pSubKey, bool Type); 
	bool DES_EDE2(char *Out, char *In, long datalen, const char *Key, int keylen, bool Type); //三重DES加密 EDE2模式 
protected:
	bool SubKey[2][16][48];//两个密钥的16轮子密钥 
	char Tmp[256], deskey[16]; 
protected:
	inline void MakeSubKey(PSubKey pSubKey, const char Key[8]); //生成子密钥
	inline void MakeKey(const char* Key, int len); //生成DES_EDE2密钥
private:
	inline void Transform(bool *Out, bool *In, const char *Table, int len); //通用置换函数
	inline void Xor(bool *InA, const bool *InB, int len); //异或运算 
	inline void MoveLeft(bool *In, int len, int loop); //循环左移 
	inline void Byte2Bit(bool *Out, const char *In, int bits);	//字节转换成位 
	inline void Bit2Byte(char *Out, const bool *In, int bits);	//位转换字节 
	inline void funS(bool Out[32], const bool In[48]); //S 盒置换 
	inline void funF(bool In[32], const bool Ki[48]); //F 函数 
};
/*
	CDES des;
    char skey[32] = {NULL};
    char inbuff[32] = {NULL};
	char oubuff[32] = {NULL};
	char oubuff2[32] = {NULL};
	memset(skey, 'a', 32);
	memset(inbuff, 'a', 32);
	memcpy(skey,"12345678",8);
	memcpy(inbuff,"12345678",8);
	//加密
	des.DES_EDE2(oubuff, inbuff, sizeof(inbuff), skey,sizeof(skey), ENCRYPT);
	//解密
	des.DES_EDE2(oubuff2, oubuff, sizeof(oubuff), skey, sizeof(skey), DECRYPT);
*/

#endif // !defined(AFX_DES_H__392A1620_7EC8_471F_AF89_913F62B65327__INCLUDED_)
