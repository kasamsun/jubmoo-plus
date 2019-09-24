#ifndef _C_BMPDIGIT_H_
#define _C_BMPDIGIT_H_

#include "JubmooApp.h"

#define USE_DELIMETER   0
#define USE_FIXWIDTH	1

class C_BmpDigit
{
public:
	C_BmpDigit();
	~C_BmpDigit();

	// interface
    int SetDigitImage(HINSTANCE hInstance,LPCTSTR lpszName,int nPicMode);
    int ClearDigitImage();

	int PaintNumber(HDC hDC,const RECT& rect, int nNumber,DWORD dwFormat);

private:
	int m_nDelimeter[11];
    int m_nNumWidth[11];
	int m_nDigitImageMode;

	HDC m_hdcDigit;
    HBITMAP m_hbmpDigit,m_hbmpDigitTmp;
	HDC m_hdcDigitMsk;
    HBITMAP m_hbmpDigitMsk,m_hbmpDigitMskTmp;
};

#endif