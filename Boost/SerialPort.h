// SerialPort.h: interface for the CSerialPort class.
//
// PURPOSE:	This class can read, write and watch one serial port.
//			The class creates a thread for reading and writing so the main program is not blocked.
//
// ACTION: You can overwrite the virtual function that when something happened on the port will call it. 
//
// CREATION DATE: 18-03-2010
//
// AUTHOR: �꼾���
// Postscript: ������ο� Remon Spekreijse �� CSerialPort��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALPORT_H__2987D703_9090_4668_995B_3DD16035F13A__INCLUDED_)
#define AFX_SERIALPORT_H__2987D703_9090_4668_995B_3DD16035F13A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define DEFAULT_COM_MASK_EVENT		EV_RXCHAR | EV_CTS
#define DEFAULT_COM_MASK_EVENT		EV_RXCHAR | EV_ERR | EV_CTS | EV_DSR | EV_BREAK | EV_TXEMPTY | EV_RING | EV_RLSD
#define TIMEOUT_NOCHANGE -1

class CSerialPort  
{
public:

	// contruction and destruction
	CSerialPort();
	virtual ~CSerialPort();

	// port initialisation											
	BOOL CSerialPort::InitPort(	UINT  portnr,		// portnumber (1..16)
								UINT  baud,			// baudrate
								char  parity,		// parity 
								UINT  databits,		// databits 
								UINT  stopbits,		// stopbits 
								DWORD dwCommEvents,	// EV_RXCHAR, EV_CTS etc
								UINT  writebuffersize);	// size to the writebuffer //pPortOwner = NULLʱ����ӵ�д���,����������Ϣ,��Ҫ�����������¼�������
	// start/stop comm watching
	BOOL		StartMonitoring();
	BOOL		RestartMonitoring();
	BOOL		StopMonitoring();
	void		ClosePort();
	// Get the Attributes
	DWORD		GetWriteBufferSize();
	DWORD		GetCommEvents();
	DCB			GetDCB();
	
	//Operation
	//�ı�У��
	BOOL		ChangeParity(BYTE bParity);
	//д��������
	void		WriteToPort(char* pBuf, int nLen);
	//��ջ���
	BOOL		FlushFileBuffersST();
	//��ĳ��ʱֵΪTIMEOUT_NOCHANGEʱ�����������ı��豸�ĸó�ʱֵ��
	BOOL		SetCommTimeoutsST(DWORD ReadIntervalTimeout = TIMEOUT_NOCHANGE, 
								DWORD ReadTotalTimeoutConstant = TIMEOUT_NOCHANGE, 
								DWORD ReadTotalTimeoutMultiplier = TIMEOUT_NOCHANGE,
								DWORD WriteTotalTimeoutConstant = TIMEOUT_NOCHANGE,
								DWORD WriteTotalTimeoutMultiplier = TIMEOUT_NOCHANGE);
	
//////////////////////////////////////////////////////////////////////////
//Virtual Operation
	//EV_CTS�¼�����
	virtual void OnCTS(){}
	//EV_RXFLAG�¼�����
	virtual void OnRXFlag(){}
	//EV_BREAK�¼�����
	virtual void OnBreak(){}
	//EV_ERR�¼�����
	virtual void OnErr(){}
	//EV_RING�¼�����
	virtual void OnRing(){}
	//EV_TXEMPTY�¼�����
	virtual void OnTXEmpty(){}
	//EV_RXCHAR�¼�����
	virtual void OnReceiveChar(unsigned char * pBuf, int nLen){}

protected:
	// protected functions
	void		ProcessErrorMessage(char* ErrorText);
	static UINT	CommThread(LPVOID pParam);
	static void	ReceiveChar(CSerialPort* port, COMSTAT comstat);
	static void	WriteChar(CSerialPort* port);
	
	// thread
	CWinThread*			m_Thread;
	
	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync;
	BOOL				m_bThreadAlive;
	
	// handles
	HANDLE				m_hShutdownEvent;
	HANDLE				m_hComm;
	HANDLE				m_hWriteEvent;
	
	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3];
	
	// structures
	OVERLAPPED			m_ov;
	COMMTIMEOUTS		m_CommTimeouts;
	DCB					m_dcb;
	
	// owner window
	CWnd*				m_pOwner;
	
	// misc
	UINT				m_nPortNr;
	char*				m_szWriteBuffer;
	DWORD				m_nBytesToWrite;
	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;
};

#endif // !defined(AFX_SERIALPORT_H__2987D703_9090_4668_995B_3DD16035F13A__INCLUDED_)
