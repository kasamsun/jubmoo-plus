#include "BmpDigit.h"

#include "kswafx/DCEx.h"

C_BmpDigit::C_BmpDigit()
{
	m_hdcDigit = NULL;
	m_hdcDigitMsk = NULL;
}

C_BmpDigit::~C_BmpDigit()
{
	ClearDigitImage();
}

int C_BmpDigit::SetDigitImage(HINSTANCE hInstance,LPCTSTR lpszName,int nPicMode)
{
	HBITMAP hbmpImage;
	if ( hInstance == NULL )
		hbmpImage = (HBITMAP)LoadImage(NULL,lpszName,
							IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	else
		hbmpImage = (HBITMAP)LoadBitmap(hInstance,lpszName);

	// can't load image , return 
	if ( hbmpImage == NULL ) return FALSE; 
	
	ClearDigitImage();
	m_hbmpDigit = hbmpImage;

	HDC hDC = GetDC(GetDesktopWindow());
	m_hdcDigit = CreateCompatibleDC(hDC);
	m_hbmpDigitTmp = (HBITMAP)SelectObject(m_hdcDigit,m_hbmpDigit);
	ReleaseDC(GetDesktopWindow(),hDC);

	BITMAP bm;
	GetObject(m_hbmpDigit,sizeof(bm),&bm);

    // set delimeter
	m_nDigitImageMode = nPicMode;
    if ( m_nDigitImageMode == USE_FIXWIDTH )
    {
    	for ( int i=0 ; i<11 ;i++ )
        {
        	m_nDelimeter[i] = bm.bmWidth/11*i;
            m_nNumWidth[i] = bm.bmWidth/11;
		}
    }
    else
    {
    	// ==================================================
        // use white dot on the bottom line as delimeter
        // ==================================================
    	m_nDelimeter[0] = 0;
        int nLastDelimeter = 0;
        int nNumOfDelimeter = 0;
    	for ( int i=0 ; i<bm.bmWidth ; i++ )
        {
        	if ( GetPixel(m_hdcDigit,i,bm.bmHeight-1) == RGB(255,255,255) )
            {
            	m_nNumWidth[nNumOfDelimeter] = i-nLastDelimeter;
                nNumOfDelimeter++;
                if ( nNumOfDelimeter < 11 )
                	m_nDelimeter[nNumOfDelimeter] = i;
                else
                	break;
                nLastDelimeter = i;
                SetPixel(m_hdcDigit,i,bm.bmHeight-1,TRANSPARENT_COLOR);
            }
		}
    }
    // ==========================================
    // no error handling now. beware in using it
    // ==========================================
    m_hdcDigitMsk = CreateCompatibleDC(m_hdcDigit);
	C_DCEx * pDCEx = new C_DCEx(m_hdcDigitMsk);
    m_hbmpDigitMsk = pDCEx->CreateMaskBitmap(m_hdcDigit,bm.bmWidth,bm.bmHeight,TRANSPARENT_COLOR);
	m_hbmpDigitMskTmp = (HBITMAP)SelectObject(m_hdcDigitMsk,m_hbmpDigitMsk);
	delete pDCEx;

	return TRUE;
}

int C_BmpDigit::ClearDigitImage()
{
	if ( m_hdcDigit != NULL )
	{
		DeleteObject(SelectObject(m_hdcDigit,m_hbmpDigitTmp));
		DeleteDC(m_hdcDigit);
		m_hdcDigit = NULL;
		DeleteObject(SelectObject(m_hdcDigitMsk,m_hbmpDigitMskTmp));
		DeleteDC(m_hdcDigitMsk);
		m_hdcDigitMsk = NULL;
	}
	return TRUE;
}

int C_BmpDigit::PaintNumber(HDC hDC,const RECT& rect, int nNumber,DWORD dwFormat)
{
	// =======================================
	// dwFormat didn't implement yet
	// =======================================
	BITMAP bm;
	GetObject(m_hbmpDigit,sizeof(bm),&bm);

	int nFraction;
    int nQuotient = abs(nNumber);
    int nRightEdge = rect.right;

	C_DCEx * pDCEx = new C_DCEx(hDC);
    do
    {
    	nFraction = nQuotient % 10;
        nQuotient /= 10;

        nRightEdge-=m_nNumWidth[nFraction];
		pDCEx->TransparentBlt(nRightEdge,rect.top,m_nNumWidth[nFraction],bm.bmHeight,
    						m_hdcDigit,m_hdcDigitMsk,m_nDelimeter[nFraction],0);
    }
    while ( nQuotient > 0 );

    if ( nNumber < 0 )
    {
    	nRightEdge-=m_nNumWidth[10];
		pDCEx->TransparentBlt(nRightEdge,rect.top,m_nNumWidth[10],bm.bmHeight,
    						m_hdcDigit,m_hdcDigitMsk,m_nDelimeter[10],0);
	}
	delete pDCEx;

    return 0;
}
