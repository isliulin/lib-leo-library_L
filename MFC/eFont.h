#pragma once
#include "afxwin.h"

class eFont :public CFont
{
public:
	eFont(int pSize=9  , bool bBold=false, const char *pFontType="Arial")
	{
		memset(&efontlog,0,sizeof(efontlog));

		if(pSize<1||pSize>90)
		{
			throw "the font size is illegal ";

		}

		if(pFontType==NULL)
		{
			throw "font type is illegal ";
		}

		efontlog.lfHeight=pSize;

		StrCpy(efontlog.lfFaceName,(LPCWSTR)pFontType);		// make sure value copy to struc

		if (bBold)
		{
			efontlog.lfWeight=FW_EXTRABOLD;
		}
		else
		{
			efontlog.lfWeight=FW_NORMAL;
		}
		this->CreateFontIndirect(&efontlog);
	}

private:
	LOGFONT efontlog; 

};
