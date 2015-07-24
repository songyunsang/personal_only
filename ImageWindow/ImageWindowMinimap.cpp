#include "StdAfx.h"
#include "ImageWindowMinimap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const long	MINIMAP_MARGIN	= 4;

CImageWindowMinimap::CImageWindowMinimap(void)
: __nW(0)
, __nH(0)
, __nD(0)
, __pbyImage(nullptr)
, __rtViewROI(CRect(0, 0, 0, 0))
, m_bIsCaptured(FALSE)
{
}

CImageWindowMinimap::~CImageWindowMinimap(void)
{
	if(__pbyImage)
	{
		delete []__pbyImage;
		__pbyImage = nullptr;
	}
}

void CImageWindowMinimap::SetMinimapSize(CSize cnMinimapsize)
{
	__cnMinimapsize = cnMinimapsize;
	__cnMinimapsize.cx += __cnMinimapsize.cx % 4;
}

void CImageWindowMinimap::SetImage(SWindow_Image &image)
{
	SetImage(image.pbyImage, image.nWidth, image.nHeight, image.nWidthBytes, image.nBpp);
}

void CImageWindowMinimap::SetImage(const BYTE *pbyImage, int nWidth, int nHeight, int nWidthbytes, int nBpp)
{
	// 이미지 사이즈가 다르다면 재할당
	int nNewSize = nWidth*nHeight*(nBpp/8);
	int nOldSize = __nW*__nH*__nD;
	if(__pbyImage != nullptr && nNewSize != nOldSize)
	{
		delete []__pbyImage;
		__pbyImage = nullptr;
	}
	if(!__pbyImage)
		__pbyImage = new BYTE[__cnMinimapsize.cx * __cnMinimapsize.cy * (nBpp / 8) /*nNewSize*/];

	// 영상을 리사이즈하여 복사
	float fRateX = (float)nWidth  / __cnMinimapsize.cx;
	float fRateY = (float)nHeight / __cnMinimapsize.cy;
	int nNPos(0), nSPos(0), nSYPos(0), nSXPos(0);
	int nB = nBpp / 8;
	for(long y=0; y<__cnMinimapsize.cy; y++)
	{
		nSYPos = (int)(fRateY * y);
		for(long x=0; x<__cnMinimapsize.cx; x++)
		{
			nSXPos = (int)(fRateX * x);
			nNPos = (y * __cnMinimapsize.cx + x) * nB;
			nSPos = (nSYPos * nWidthbytes) + (nSXPos * nB);
			for(long d=0; d<nB; d++)
			{
				__pbyImage[nNPos + d] = pbyImage[nSPos + d];
			}
		}
	}
	
	__nW = __cnMinimapsize.cx;
	__nH = __cnMinimapsize.cy;
	__nD = nBpp;
	__szImageSrcSize.cx = nWidth;
	__szImageSrcSize.cy = nHeight;
}

void CImageWindowMinimap::SetCurrentViewPos(CRect rtPos)
{
	float fRateX = (float)__nW / __szImageSrcSize.cx;
	float fRateY = (float)__nH / __szImageSrcSize.cy;

	__rtViewROI.left	= (long)(rtPos.left		* fRateX + .5f);
	__rtViewROI.top		= (long)(rtPos.top		* fRateY + .5f);
	__rtViewROI.right	= (long)(rtPos.right	* fRateX + .5f);
	__rtViewROI.bottom	= (long)(rtPos.bottom	* fRateY + .5f);
}

void CImageWindowMinimap::__DRAW_Image(HDC hdc, unsigned char *imgdata, long width, long height, long widthBytes, int bpp, int toptobottom, CRect rtDIBRect, CRect rtDCRect)
{
	char buf[ sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256 ];					// 비트맵 헤더정보와 팔레트 정보를 셋팅할 버퍼
	BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER*)buf;							// 비트맵 인포헤더
	COLORREF* rgb = (COLORREF*)( buf + sizeof(BITMAPINFOHEADER) );				// 팔레트

	if(bpp == 8)
	{
		// 그레이 팔레트 만들기 */

		for(int i = 0; i < 256; i++)
		{
			rgb[i] = RGB( i,i,i );
		}
	}

	dibhdr->biSize			= sizeof( BITMAPINFOHEADER );								// 사이즈 
	dibhdr->biWidth			= width;													// 가로길이 
	dibhdr->biHeight		= toptobottom ? -height : height;							// 세로길이 
	dibhdr->biPlanes		= 1;														// 플레인 수 
	dibhdr->biBitCount		= (WORD)bpp;												// 비트 퍼 픽셀 
	dibhdr->biCompression	= BI_RGB;												// 압축 여부 
	dibhdr->biSizeImage		= widthBytes * height;									// 이미지 사이즈 
	dibhdr->biXPelsPerMeter = 0;												
	dibhdr->biYPelsPerMeter = 0;
	dibhdr->biClrUsed		= 0;
	dibhdr->biClrImportant	= 0; 

	::StretchDIBits(hdc, 				// hDC
		rtDCRect.left,					// DestX
		rtDCRect.top,					// DestY
		rtDCRect.Width(),				// nDestWidth
		rtDCRect.Height(),				// nDestHeight
		rtDIBRect.left,					// SrcX
		rtDIBRect.top,					// SrcY
		rtDIBRect.Width(),				// wSrcWidth
		rtDIBRect.Height(),				// wSrcHeight
		imgdata,						// lpBits
		(LPBITMAPINFO)dibhdr,			// lpBitsInfo
		DIB_RGB_COLORS,					// wUsage
		SRCCOPY);						// dwROP
}

void CImageWindowMinimap::DrawAlphaBlendDraw(CDC *pDC, CRect rtClient)
{
	// 미니맵을 그릴 위치를 얻는다.
	rtClient.right -= 10;
	rtClient.bottom -= 10;
	rtClient.left = rtClient.right - __cnMinimapsize.cx;
	rtClient.top = rtClient.bottom - __cnMinimapsize.cy;
	// Width는 4의 배수로 해야 이미지가 제대로 나온다.
	rtClient.right += rtClient.Width() % 4;
	__rtMinimapRect = rtClient;

	// memDC를 생성하여 비트맵 데이터를 그린다.
	BITMAP bmp;
	pDC->GetCurrentBitmap()->GetBitmap(&bmp);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(pDC, bmp.bmWidth, bmp.bmHeight);
	memDC.SelectObject(memBmp);

	// 테두리 그리기
	CBrush bruGray(RGB(0, 0, 0)), *pOldBru;
	pOldBru = memDC.SelectObject(&bruGray);
	CRect rtFrame(__rtMinimapRect);
	rtFrame.InflateRect(MINIMAP_MARGIN, MINIMAP_MARGIN);
	memDC.RoundRect(&rtFrame, CPoint(MINIMAP_MARGIN*2, MINIMAP_MARGIN*2));
	memDC.SelectObject(&pOldBru);

	// 미니맵 그리기
	CRect rtSrc(0, 0, __nW, __nH);
	__DRAW_Image(memDC.m_hDC, __pbyImage, __nW, __nH, __nW*__nD/8, __nD, 1, rtSrc, __rtMinimapRect);

	// 보여지는 영역 그리기
	CBrush bruNull;
	CPen penROI(PS_SOLID, 1, RGB(0, 192, 255)), *pOldPen;
	bruNull.Attach(GetStockObject(NULL_BRUSH));
	pOldBru = memDC.SelectObject(&bruNull);
	pOldPen = memDC.SelectObject(&penROI);
	CRect rtROI(__rtMinimapRect.left + __rtViewROI.left, __rtMinimapRect.top + __rtViewROI.top, __rtMinimapRect.left + __rtViewROI.right, __rtMinimapRect.top + __rtViewROI.bottom);
	memDC.Rectangle(rtROI);
	memDC.SelectObject(pOldBru);
	memDC.SelectObject(pOldPen);
	bruNull.Detach();

	// 반투명하게 그린다.
	BLENDFUNCTION bf;
	bf.BlendOp=AC_SRC_OVER; 
	bf.BlendFlags=0; 
	bf.SourceConstantAlpha=192; 
	bf.AlphaFormat=0;
	AlphaBlend(pDC->m_hDC, rtFrame.left, rtFrame.top, rtFrame.Width(), rtFrame.Height(), memDC.m_hDC, rtFrame.left, rtFrame.top, rtFrame.Width(), rtFrame.Height(), bf);
}

void CImageWindowMinimap::SetCapture(CPoint point)
{
	m_bIsCaptured = __rtMinimapRect.PtInRect(point);
}

BOOL CImageWindowMinimap::GetPosition(CPoint point, CPoint& ptDest)
{
	if( !IsCaptured() ) return FALSE;

	float fRateX = (float)__nW / __szImageSrcSize.cx;
	float fRateY = (float)__nH / __szImageSrcSize.cy;

	point -= __rtMinimapRect.TopLeft();

	ptDest.x = (long)(point.x / fRateX);
	ptDest.y = (long)(point.y / fRateY);

	long l, t, r, b;

	l = 0;
	t = 0;
	r = __szImageSrcSize.cx;
	b = __szImageSrcSize.cy;

	ptDest.x = max(ptDest.x, l);
	ptDest.x = min(ptDest.x, r);
	ptDest.y = max(ptDest.y, t);
	ptDest.y = min(ptDest.y, b);

	return TRUE;
}

void CImageWindowMinimap::ReleaseCapture(CPoint point)
{
	m_bIsCaptured = FALSE;	
}


