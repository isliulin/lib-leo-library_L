#pragma once

class MyEvent  
{
public:
	MyEvent() { m_hObject = ::CreateEvent(NULL, TRUE, FALSE, NULL); }
	~MyEvent() { if (m_hObject) CloseHandle(m_hObject); m_hObject = NULL; }
	operator HANDLE() const { return m_hObject;}
	BOOL SetEvent() { return ::SetEvent(m_hObject); }
	BOOL ResetEvent() { ASSERT(m_hObject != NULL); return ::ResetEvent(m_hObject); }
private:
	HANDLE m_hObject;
};

