#pragma once
#include "afxwin.h"
#include "swindowimage.h"

class CImageWindowMinimap
{
	// �̹��� ����
	SWindow_Image __Image;

	BYTE *__pbyImage;
	int __nW, __nH, __nD;		// ���� �� �Ҵ�� �̹��� ������ ���� (100x100x8)
	CSize __szImageSrcSize;		// ���� �̹��� ������ ����				��)1920x1980
	CRect __rtViewROI;			// ���� ȭ�鿡 �������� �̹��� ����
	
	CSize __cnMinimapsize;		// �̴ϸ� ������
	CRect __rtMinimapRect;		// ���� �����쿡�� �̴ϸ��� ��ġ

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
