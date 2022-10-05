#include "CardWnd.h"
#include "kswafx/DCEx.h"

#include "resource.h"
#include "CardNum.h"

BEGIN_HANDLE_MSG(C_CardWnd,C_GenericClickWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)

END_HANDLE_MSG()

HDC C_CardWnd::m_hdcBackCardImage = NULL;
HBITMAP C_CardWnd::m_hbmpBackCardImage = NULL;
HBITMAP C_CardWnd::m_hbmpBackCardTmp = NULL;
HDC C_CardWnd::m_hdcCardImage = NULL;
HBITMAP C_CardWnd::m_hbmpCardImage = NULL;
HBITMAP C_CardWnd::m_hbmpCardTmp = NULL;

///////////////////// static member function ///////////////////////////

// ================================================
// Backcard Image can't be tile
// so , input image should be 71x96
// ================================================
int C_CardWnd::SetBackCardImage(LPCTSTR lpszName)
{
	ClearBackCardImage();

	if ( lpszName != NULL )
	{
		m_hbmpBackCardImage = (HBITMAP)LoadImage(NULL,lpszName,
							IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		// if can't load back card image , set to default
		if ( m_hbmpBackCardImage == NULL )
			m_hbmpBackCardImage = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_BACKCARD));
    }
	else
		m_hbmpBackCardImage = (HBITMAP)LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_BACKCARD));

	HDC hDC = GetDC(GetDesktopWindow());
	m_hdcBackCardImage = CreateCompatibleDC(hDC);
	m_hbmpBackCardTmp = (HBITMAP)SelectObject(m_hdcBackCardImage,m_hbmpBackCardImage);
	ReleaseDC(GetDesktopWindow(),hDC);

	return TRUE;
}

int C_CardWnd::ClearBackCardImage()
{
	if ( m_hdcBackCardImage != NULL )
	{
		DeleteObject(SelectObject(m_hdcBackCardImage,m_hbmpBackCardTmp));
		DeleteDC(m_hdcBackCardImage);
		m_hdcBackCardImage = NULL;
	}
	return TRUE;
}

// ==========================================================
// Preload all card image , can increase dramatic performance
// ==========================================================
int C_CardWnd::SetCardImage(LPCTSTR lpszName)
{
	ClearCardImage();

	HDC hDC = GetDC(GetDesktopWindow());
	m_hdcCardImage = CreateCompatibleDC(hDC);
	m_hbmpCardImage = CreateCompatibleBitmap(hDC,CARD_WIDTH*13,CARD_HEIGHT*4);
	m_hbmpCardTmp = (HBITMAP)SelectObject(m_hdcCardImage,m_hbmpCardImage);

	HDC hdcCard = CreateCompatibleDC(hDC);

    HBITMAP hbmpCard,hbmpTmp;
	if ( lpszName != NULL )
	{
		hbmpCard = (HBITMAP)LoadImage(NULL,lpszName,
							IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		// if can't load back card image , set to default
		if ( hbmpCard == NULL )
			hbmpCard = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_CARD));
    }
	else
		hbmpCard = (HBITMAP)LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_CARD));

	hbmpTmp = (HBITMAP)SelectObject(hdcCard,hbmpCard);
	BITMAP test;
	GetObject(hbmpCard,sizeof(BITMAP),&test);
	BitBlt(m_hdcCardImage,0,0,test.bmWidth,test.bmWidth,hdcCard,0,0,SRCCOPY);
	DeleteObject(SelectObject(hdcCard,hbmpTmp));
//    for ( int i=0 ; i<4 ; i++ )
//		for ( int j=1 ; j<=13 ; j++ )
//		{
//			hbmpCard = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE((i*16)+j));
//			hbmpTmp = (HBITMAP)SelectObject(hdcCard,hbmpCard);
//			BitBlt(m_hdcCardImage,( (i*13)+(j-1) )*CARD_WIDTH,0,CARD_WIDTH,CARD_HEIGHT,
  //      			hdcCard,0,0,SRCCOPY);
//			TRACE("=> %d,%d\n",(i*16)+j,( (i*13)+(j-1) ) * CARD_WIDTH);
//			DeleteObject(SelectObject(hdcCard,hbmpTmp));
//		}
	DeleteDC(hdcCard);
	ReleaseDC(GetDesktopWindow(),hDC);

	return TRUE;
}

int C_CardWnd::ClearCardImage()
{
	if ( m_hdcCardImage != NULL )
	{
		DeleteObject(SelectObject(m_hdcCardImage,m_hbmpCardTmp));
		DeleteDC(m_hdcCardImage);
		m_hdcCardImage = NULL;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////

// ============================================================
// empty CardWnd intend to use only PaintCard service
// ============================================================
C_CardWnd::C_CardWnd()
{
}

C_CardWnd::C_CardWnd(C_Wnd *pParentWnd,int CardNum,int PosX,int PosY,int CardStatus) : C_GenericClickWnd(pParentWnd)
{
	m_CardStatus = CardStatus;
    // CardNum must be in 1-52 range
	m_CardNum = CardNum;

	m_hWnd = CreateWindow(
		m_szClassName,
        "",
        WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS,
        PosX,PosY,
        CARD_WIDTH,CARD_HEIGHT,
        pParentWnd->GetHandle(),
        NULL,
        C_App::m_hInstance,
        (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

    //ShowWindow(m_hWnd,SW_SHOWNA);
	//UpdateWindow(m_hWnd);

	m_pParentWnd = pParentWnd;

	HRGN hrgn;
	POINT CardRim[9];

	CardRim[0].x = 2;	CardRim[0].y = 0;
	CardRim[1].x = 0;	CardRim[1].y = 2;
	CardRim[2].x = 0;	CardRim[2].y = 93;
	CardRim[3].x = 2;	CardRim[3].y = 95;
	CardRim[4].x = 68;	CardRim[4].y = 96;
	CardRim[5].x = 71;	CardRim[5].y = 93;
	CardRim[6].x = 71;	CardRim[6].y = 2;
	CardRim[7].x = 69;	CardRim[7].y = 0;
	CardRim[8].x = 2;	CardRim[8].y = 0;
	hrgn = CreatePolygonRgn(CardRim,9,WINDING);
	SetWindowRgn(m_hWnd,hrgn,TRUE);
	SetActiveCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_POINT)) );
	SetDragCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_GRASP)) );
	SetClickCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_CLICK)) );
}

C_CardWnd::~C_CardWnd()
{
	if ( IsWindow(m_hWnd) ) DestroyWindow(m_hWnd);
}

int C_CardWnd::PaintCard(HDC hDC,int x,int y,int nCardNum)
{
	static int PixelBk[12][2] = { {0,0},{1,0},{0,1},
							{70,0},{70,1},{69,0},
							{0,94},{0,95},{1,95},
							{70,95},{70,94},{69,95} };

	COLORREF	ctmp[12];
	int i;
	for (i = 0 ; i <= 11 ;i++)
		ctmp[i] = GetPixel(hDC,x+PixelBk[i][0],y+PixelBk[i][1]);
    if ( nCardNum == 0 )
		BitBlt(hDC,x,y,CARD_WIDTH,CARD_HEIGHT,m_hdcBackCardImage,0,0,SRCCOPY);
    else
    {
    	BitBlt(hDC,x,y,CARD_WIDTH,CARD_HEIGHT,m_hdcCardImage,
					(GET_NUM(nCardNum)-1)*CARD_WIDTH,GET_FACE(nCardNum) * CARD_HEIGHT,SRCCOPY);
    }
	for (i = 0 ; i <= 11 ;i++)
		SetPixel(hDC,x+PixelBk[i][0],y+PixelBk[i][1],ctmp[i]);

	return 0;
}

void C_CardWnd::FaceUp()
{
	m_CardStatus = FACE_UP;
	InvalidateRect(m_hWnd,NULL,TRUE);
}

void C_CardWnd::FaceDown()
{
	m_CardStatus = FACE_DOWN;
	InvalidateRect(m_hWnd,NULL,TRUE);
}

int C_CardWnd::TurnFace()
{
	if ( m_CardStatus==FACE_UP)
		m_CardStatus = FACE_DOWN;
	else
		m_CardStatus = FACE_UP;
	InvalidateRect(m_hWnd,NULL,TRUE);
	return m_CardStatus;
}


///////////////////// Event Handler /////////////////////////
LRESULT C_CardWnd::OnPaint(WPARAM ,LPARAM )
{
    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

	if ( m_CardStatus == FACE_UP )
    	BitBlt(m_hDC,0,0,CARD_WIDTH,CARD_HEIGHT,m_hdcCardImage,
					(GET_NUM(m_CardNum)-1)*CARD_WIDTH,GET_FACE(m_CardNum) * CARD_HEIGHT,SRCCOPY);
	else
    	BitBlt(m_hDC,0,0,CARD_WIDTH,CARD_HEIGHT,m_hdcBackCardImage,0,0,SRCCOPY);
	EndPaint(m_hWnd,&ps);
	return 0;
}

