// #include "stdafx.h"
// #include "CommandPool.h"
// 
// void CommandPool::PostCmd(char* pBuf, int nLen)
// {
// }
// 
// void CommandPool::AnalysisCmd()
// {
// // 	// ����
// // 	if (m_nBufLen < 6)
// // 	{
// // 		return;
// // 	}
// // 	// ��ͷ: 0xAA
// // 	int nLeft = Find((char)0xAA);
// // 	if ( nLeft > 0)
// // 	{
// // 		PopLeft(nLeft);
// // 	}
// // 	else if (-1 == nLeft)
// // 	{
// // 		PopLeft(m_nBufLen);
// // 	}
// // 	// ����
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
// // 	// У���: ���� ״̬�� ������ ����
// // 	unsigned char checksum = 0;
// // 	for (int i=0; i<(iCmdLen-2); i++)
// // 	{
// // 		checksum += (unsigned char)m_pBuf[i+1];
// // 	}
// // 	if (checksum != (unsigned char)m_pBuf[1+iCmdLen-1-1])
// // 	{
// // 		// У������޳�
// // 		PopLeft(1);
// // 		// ���½���
// // 		AnalysisCmd();
// // 		return;
// // 	}
// // 	// ��β:0xAD
// // 	if (0xAD != (unsigned char)m_pBuf[1+iCmdLen-1])
// // 	{
// // 		// ��β�����޳�
// // 		PopLeft(1);
// // 		// ���½���
// // 		AnalysisCmd();
// // 		return;
// // 	}
// // 
// // 	// ��ȡָ��
// // 	int iCmdLen = (UCHAR)m_pBuf[1]*256 + m_pBuf[2];
// // 	PostCmd();
// // 	// �Ƴ�һ����
// // 	PopLeft(1+iCmdLen);
// }
