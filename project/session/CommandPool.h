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
	virtual void AnalysisCmd(); // 分析、拆解命令
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
		// 包尾:0xAD
		if (0xAD == pBuf[i])
		{ // 处理可能的包尾
			AnalysisCmd();
		}
		if (MAX_COMMAND_BUF_LEN == m_nCommandBufferLength)
		{
			// 出现意外，清空
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
	// 长度
	if (m_nCommandBufferLength < 5)
	{
		return;
	}
	// 包头: 0xAA
	int nLeft = Find(0xAA);
	if ( nLeft > 0)
	{
		PopLeft(nLeft);
	}
	else if (-1 == nLeft)
	{
		PopLeft(m_nCommandBufferLength);
	}
	// 长度
	if (m_nCommandBufferLength < 5)
	{
		return;
	}
	int iDataLen = ((UCHAR)m_pCommandBuffer[1])*256 + m_pCommandBuffer[2]; 
	if (m_nCommandBufferLength < (3+iDataLen+2))
	{
		return;
	}
	// 校验和: 起始字开始
	if ((unsigned char)GetCheckSum(m_pCommandBuffer, 3+iDataLen) != (unsigned char)m_pCommandBuffer[3+iDataLen+1-1])
	{
		// 校验错误，剔除
		PopLeft(1);
		// 重新解析
		AnalysisCmd();
		return;
	}
	// 包尾:0xAD
	if (0xAD != m_pCommandBuffer[3+iDataLen+2-1])
	{
		// 包尾错误，剔除
		PopLeft(1);
		// 重新解析
		AnalysisCmd();
		return;
	}
	// 拷贝指令
	BYTE pCmdBuf[1024];
	memcpy_s(pCmdBuf, iDataLen, &m_pCommandBuffer[3], iDataLen);
	pCmdBuf[iDataLen] = NULL;
	// 移除原命令内存，（已经拷贝到新内存中了）
	PopLeft(3+iDataLen+2);
	// 触发命令
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
	// 起始字符
	pCmdBuf[nBytes++] = 0XAA;
	// 长度 2字节
	pCmdBuf[nBytes++] = (BYTE) (nLenIn >> 8);
	pCmdBuf[nBytes++] = (BYTE) (nLenIn);
	// 数据域
	for (int i=0; i<nLenIn; i++)
	{
		pCmdBuf[nBytes++] = pBufIn[i];
	}
	// 校验和
	pCmdBuf[nBytes++] = (BYTE) GetCheckSum(pCmdBuf, nBytes);
	// 包尾 0xAD
	pCmdBuf[nBytes++] = 0xAD;
	nCmdBufLen = nBytes;
	return nCmdBufLen;
}
