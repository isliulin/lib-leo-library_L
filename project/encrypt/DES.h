// DES.h: interface for the CDES class.
// �꼾��� ����
// 2010��5��15��
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
	//��׼DES ��/����
	//Type = ENCRYPT���ܣ� DECREYPT ����
	void DES_Original(char Out[8], char In[8], const PSubKey pSubKey, bool Type); 
	bool DES_EDE2(char *Out, char *In, long datalen, const char *Key, int keylen, bool Type); //����DES���� EDE2ģʽ 
protected:
	bool SubKey[2][16][48];//������Կ��16������Կ 
	char Tmp[256], deskey[16]; 
protected:
	inline void MakeSubKey(PSubKey pSubKey, const char Key[8]); //��������Կ
	inline void MakeKey(const char* Key, int len); //����DES_EDE2��Կ
private:
	inline void Transform(bool *Out, bool *In, const char *Table, int len); //ͨ���û�����
	inline void Xor(bool *InA, const bool *InB, int len); //������� 
	inline void MoveLeft(bool *In, int len, int loop); //ѭ������ 
	inline void Byte2Bit(bool *Out, const char *In, int bits);	//�ֽ�ת����λ 
	inline void Bit2Byte(char *Out, const bool *In, int bits);	//λת���ֽ� 
	inline void funS(bool Out[32], const bool In[48]); //S ���û� 
	inline void funF(bool In[32], const bool Ki[48]); //F ���� 
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
	//����
	des.DES_EDE2(oubuff, inbuff, sizeof(inbuff), skey,sizeof(skey), ENCRYPT);
	//����
	des.DES_EDE2(oubuff2, oubuff, sizeof(oubuff), skey, sizeof(skey), DECRYPT);
*/

#endif // !defined(AFX_DES_H__392A1620_7EC8_471F_AF89_913F62B65327__INCLUDED_)
