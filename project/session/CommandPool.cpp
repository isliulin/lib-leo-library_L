// #include "stdafx.h"
// #include "CommandPool.h"
// 
// void CommandPool::PostCmd(char* pBuf, int nLen)
// {
// }
// 
// void CommandPool::AnalysisCmd()
// {
// // 	// 长度
// // 	if (m_nBufLen < 6)
// // 	{
// // 		return;
// // 	}
// // 	// 包头: 0xAA
// // 	int nLeft = Find((char)0xAA);
// // 	if ( nLeft > 0)
// // 	{
// // 		PopLeft(nLeft);
// // 	}
// // 	else if (-1 == nLeft)
// // 	{
// // 		PopLeft(m_nBufLen);
// // 	}
// // 	// 长度
// // 	if (m_nBufLen < 6)
// // 	{
// // 		return;
// // 	}
// // 	int iCmdLen = (UCHAR)m_pBuf[1]*256 + m_pBuf[2]; 
// // 	if ((1+iCmdLen) > m_nBufLen)
// // 	{
// // 		return;
// // 	}
// // 
// // 	// 校验和: 长度 状态字 命令字 数据
// // 	unsigned char checksum = 0;
// // 	for (int i=0; i<(iCmdLen-2); i++)
// // 	{
// // 		checksum += (unsigned char)m_pBuf[i+1];
// // 	}
// // 	if (checksum != (unsigned char)m_pBuf[1+iCmdLen-1-1])
// // 	{
// // 		// 校验错误，剔除
// // 		PopLeft(1);
// // 		// 重新解析
// // 		AnalysisCmd();
// // 		return;
// // 	}
// // 	// 包尾:0xAD
// // 	if (0xAD != (unsigned char)m_pBuf[1+iCmdLen-1])
// // 	{
// // 		// 包尾错误，剔除
// // 		PopLeft(1);
// // 		// 重新解析
// // 		AnalysisCmd();
// // 		return;
// // 	}
// // 
// // 	// 提取指令
// // 	int iCmdLen = (UCHAR)m_pBuf[1]*256 + m_pBuf[2];
// // 	PostCmd();
// // 	// 移除一命令
// // 	PopLeft(1+iCmdLen);
// }
