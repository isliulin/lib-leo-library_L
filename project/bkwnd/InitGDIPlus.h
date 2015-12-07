#pragma once
#include <GdiPlus.h>

// #ifndef _MYGDI_NO_DEFAULT_LIBS
// #pragma comment(lib, "gdiplus.lib")
// #endif  // 

class CInitGDIPlus
{
public:
	CInitGDIPlus(){
		//Startup();
	}
	~CInitGDIPlus(){
		Shutdown();
	}

	bool Startup(){
		bool fRet = true;
		if( m_dwToken == 0 )
		{
			Gdiplus::GdiplusStartupInput input;
			Gdiplus::GdiplusStartupOutput output;
			Gdiplus::Status status = Gdiplus::GdiplusStartup( &m_dwToken, &input, &output );
			if( status != Gdiplus::Ok )
				fRet = false;
		}
		return fRet;
	}
	void Shutdown(){
		if( m_dwToken != 0 )
		{
			Gdiplus::GdiplusShutdown( m_dwToken );
		}
		m_dwToken = 0;
	}
private:
	ULONG_PTR m_dwToken;
};
