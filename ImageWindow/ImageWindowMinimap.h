#pragma once
#include "afxwin.h"
#include "swindowimage.h"

class CImageWindowMinimap
{
	// 이미지 정보
	SWindow_Image __Image;

	BYTE *__pbyImage;
	int __nW, __nH, __nD;		// 현재 뷰어에 할당된 이미지 사이즈 정보 (100x100x8)
	CSize __szImageSrcSize;		// 원본 이미지 사이즈 정보				예)1920x1980
	CRect __rtViewROI;			// 현재 화면에 보여지는 이미지 영역
	
	CSize __cnMinimapsize;		// 미니맵 사이즈
	CRect __rtMinimapRect;		// 현재 윈도우에서 미니맵의 위치

	void __DRAW_Image(HDC hdc, unsigned char *imgdata, long width, long height, long widthBytes, int bpp, int toptobottom, CRect rtDIBRect, CRect rtDCRect);
	void __MoveMainImage(CPoint point);

	BOOL m_bIsCaptured;

public:
	void SetMinimapSize(CSize cnMinimapsize);
	void SetImage(const BYTE *pbyImage, int nWidth, int nHeight, int nWidthbytes, int nBpp);
	void SetImage(SWindow_Image &image);
	void SetCurrentViewPos(CRect rtPos);
	void DrawAlphaBlendDraw(CDC *pDC, CRect rtClient);

	void SetCapture(CPoint point);
	BOOL GetPosition(CPoint point, CPoint& ptDest);
	void ReleaseCapture(CPoint point);
	BOOL IsCaptured()	{ return m_bIsCaptured;	}

public:
	CImageWindowMinimap(void);
	~CImageWindowMinimap(void);
};
