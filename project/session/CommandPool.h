// #include <afxmt.h>
#pragma once

unsigned int GetCheckSum(BYTE* pBuf, int nLen);
int MakeCmd(BYTE* pBufIn, int nLenIn, BYTE* pCmdBuf, int& nCmdBufLen);

// #include <boost/thread/shared_mutex.hpp>
// #include <boost/thread.hpp>
// typedef boost::shared_mutex rwmutex;
// typedef boost::shared_lock<rwmutex> readLock;
// typedef boost::unique_lock<rwmutex> writeLock;

#define MAX_COMMAND_BUF_LEN 4096
class CommandPool
{
public:
	CommandPool(){
		m_nCommandBufferLength = 0;
		m_pCommandBuffer = new BYTE[MAX_COMMAND_BUF_LEN];
	}
	~CommandPool(){
		delete[] m_pCommandBuffer;
	}
	
public:
	virtual void ReceiveChar(BYTE* pBuf, int nLen);
	//
	virtual void PostCmd(BYTE* pBuf, int nLen){}
	virtual void AnalysisCmd(); // �������������
protected:
	int Find(BYTE xChar);
	void PopLeft(int nLeft);
protected:
	BYTE* m_pCommandBuffer;
	int  m_nCommandBufferLength;
//  	rwmutex _rwmutex;
};
 
inline void CommandPool::ReceiveChar(BYTE* pBuf, int nLen)
{
	if ((m_nCommandBufferLength+nLen) > MAX_COMMAND_BUF_LEN)
	{
		ASSERT(FALSE);
		//nLen = MAX_COMMAND_BUF_LEN - m_nCommandBufferLength;
	}
// 	 writeLock rdLock(_rwmutex);
	//TRACE2("CommandPool: %d + %d ", m_nCommandBufferLength, nLen);
	for (int i=0; i<nLen; i++)
	{
		m_pCommandBuffer[m_nCommandBufferLength++] = pBuf[i];
		// ��β:0xAD
		if (0xAD == pBuf[i])
		{ // ������ܵİ�β
			AnalysisCmd();
		}
		if (MAX_COMMAND_BUF_LEN == m_nCommandBufferLength)
		{
			// �������⣬���
			PopLeft(m_nCommandBufferLength);
		}
	}
}

inline int CommandPool::Find(BYTE xChar)
{
	for (int i=0; i<m_nCommandBufferLength; i++)
	{
		if (xChar == m_pCommandBuffer[i])
		{
			return i;
		}
	}
	return -1;
}

inline void CommandPool::PopLeft(int nLeft)
{
	//TRACE2("CommandPool: %d - %d ", m_nCommandBufferLength, nLeft);
	if (nLeft>m_nCommandBufferLength)
	{
		m_nCommandBufferLength = 0;
		return;
	}

	for (int i=nLeft, j=0; i<m_nCommandBufferLength; i++, j++)
	{
		m_pCommandBuffer[j] = m_pCommandBuffer[i];
	}
	m_nCommandBufferLength -= nLeft;
	//TRACE1(" = %d\n", m_nCommandBufferLength);
}

inline void CommandPool::AnalysisCmd()
{
	// ����
	if (m_nCommandBufferLength < 5)
	{
		return;
	}
	// ��ͷ: 0xAA
	int nLeft = Find(0xAA);
	if ( nLeft > 0)
	{
		PopLeft(nLeft);
	}
	else if (-1 == nLeft)
	{
		PopLeft(m_nCommandBufferLength);
	}
	// ����
	if (m_nCommandBufferLength < 5)
	{
		return;
	}
	int iDataLen = ((UCHAR)m_pCommandBuffer[1])*256 + m_pCommandBuffer[2]; 
	if (m_nCommandBufferLength < (3+iDataLen+2))
	{
		return;
	}
	// У���: ��ʼ�ֿ�ʼ
	if ((unsigned char)GetCheckSum(m_pCommandBuffer, 3+iDataLen) != (unsigned char)m_pCommandBuffer[3+iDataLen+1-1])
	{
		// У������޳�
		PopLeft(1);
		// ���½���
		AnalysisCmd();
		return;
	}
	// ��β:0xAD
	if (0xAD != m_pCommandBuffer[3+iDataLen+2-1])
	{
		// ��β�����޳�
		PopLeft(1);
		// ���½���
		AnalysisCmd();
		return;
	}
	// ����ָ��
	BYTE pCmdBuf[1024];
	memcpy_s(pCmdBuf, iDataLen, &m_pCommandBuffer[3], iDataLen);
	pCmdBuf[iDataLen] = NULL;
	// �Ƴ�ԭ�����ڴ棬���Ѿ����������ڴ����ˣ�
	PopLeft(3+iDataLen+2);
	// ��������
	PostCmd(pCmdBuf, iDataLen);
	AnalysisCmd();
}

inline unsigned int GetCheckSum(BYTE* pBuf, int nLen)
{
	unsigned int nCheckSum = 0;
	for (int i=0; i<nLen; i++)
	{
		nCheckSum += pBuf[i];
	}
	return nCheckSum;
}

inline int MakeCmd(BYTE* pBufIn, int nLenIn, BYTE* pCmdBuf, int& nCmdBufLen)
{
	nCmdBufLen = 0;
	if (nLenIn > 1010 || nLenIn<0)
	{
		ASSERT(FALSE);
		return -2;
	}

	memset(pCmdBuf, 0, nLenIn+12);

	int nBytes = 0;
	// ��ʼ�ַ�
	pCmdBuf[nBytes++] = 0XAA;
	// ���� 2�ֽ�
	pCmdBuf[nBytes++] = (BYTE) (nLenIn >> 8);
	pCmdBuf[nBytes++] = (BYTE) (nLenIn);
	// ������
	for (int i=0; i<nLenIn; i++)
	{
		pCmdBuf[nBytes++] = pBufIn[i];
	}
	// У���
	pCmdBuf[nBytes++] = (BYTE) GetCheckSum(pCmdBuf, nBytes);
	// ��β 0xAD
	pCmdBuf[nBytes++] = 0xAD;
	nCmdBufLen = nBytes;
	return nCmdBufLen;
}
