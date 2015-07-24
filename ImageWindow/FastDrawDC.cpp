#include "stdafx.h"
#include "FastDrawDC.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFastDrawDC::CFastDrawDC(void)
	: __pBuffer(nullptr)
	, __pDC(nullptr)
	, __pCallDrawPpoint(nullptr)
	, __rtScreen(CRect(0, 0, 0, 0))
{
}

CFastDrawDC::~CFastDrawDC(void)
{
	if(__pBuffer)
	{
		delete [] __pBuffer;
		__pBuffer = nullptr;
	}
}

BOOL CFastDrawDC::Draw_Ready(CDC *pDC)
{
	if(pDC == NULL || pDC->m_hDC == NULL) return FALSE;

	__pDC = pDC;

	CBitmap *pBitmap = __pDC->GetCurrentBitmap();

	ASSERT(pBitmap);
	pBitmap->GetBitmap(&__bmpInfo);

	const long nBufferSize = __bmpInfo.bmWidthBytes * __bmpInfo.bmHeight;

	ASSERT(nBufferSize > 0);

	__pBuffer = new BYTE[nBufferSize];
	pBitmap->GetBitmapBits(nBufferSize, __pBuffer);

	switch(__bmpInfo.bmBitsPixel)
	{
	case 16:
		__pCallDrawPpoint = &__Draw_Point16;
		break;
	case 24:
		__pCallDrawPpoint = &__Draw_Point24;
		break;
	case 32:
		__pCallDrawPpoint = &__Draw_Point32;
		break;
	default:
		ASSERT(!"지원하지 않는 Bit 단위 타입임");
		return FALSE;
	}

	__rtScreen = CRect(0, 0, __bmpInfo.bmWidth, __bmpInfo.bmHeight);

	return TRUE;
}

void CFastDrawDC::Draw_Excute()
{
	ASSERT(__pBuffer);

	const long nBufferSize = __bmpInfo.bmWidthBytes * __bmpInfo.bmHeight;

	CBitmap *pBitmap = __pDC->GetCurrentBitmap();
	ASSERT(pBitmap);

	pBitmap->SetBitmapBits(nBufferSize, __pBuffer);

	delete [] __pBuffer;
	__pBuffer = nullptr;
}

void CFastDrawDC::Draw_Line(CPoint &ptS, CPoint &ptE, COLORREF crColor)
{
	ASSERT(__pBuffer);

	CRect rtRect(0, 0, 0, 0);
	rtRect.left		= min(ptS.x, ptE.x);
	rtRect.top		= min(ptS.y, ptE.y);
	rtRect.right	= max(ptS.x, ptE.x) + 1;
	rtRect.bottom	= max(ptS.y, ptE.y) + 1;

	if((__rtScreen & rtRect).IsRectEmpty()) return;
	if(ptS.x < -10000 || ptS.y < -10000) return;
	if(ptE.x < -10000 || ptE.y < -10000) return;
	if(ptS.x > __rtScreen.right + 10000 || ptS.y > __rtScreen.bottom + 10000) return;
	if(ptE.x > __rtScreen.right + 10000 || ptE.y > __rtScreen.bottom + 10000) return;

	int t(0), Distance(0);
	int xErr = 0, yErr = 0;

	int Delta_X	= ptE.x - ptS.x;
	int Delta_Y	= ptE.y - ptS.y;
	const int IncX	= (Delta_X > 0) ? 1 : ((Delta_X == 0) ? 0 : -1);
	const int IncY	= (Delta_Y > 0) ? 1 : ((Delta_Y == 0) ? 0 : -1);

	Delta_X = abs( Delta_X );
	Delta_Y = abs( Delta_Y );

	Distance = (Delta_X > Delta_Y) ? Delta_X : Delta_Y;

	CPoint xy = ptS;

	for(t = 0; t < Distance; t++)
	{
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, xy.x, xy.y, crColor);

		xErr += Delta_X;
		yErr += Delta_Y;

		if(xErr >= Distance)
		{
			xErr -= Distance;
			xy.x += IncX;
		}

		if(yErr >= Distance)
		{
			yErr -= Distance;
			xy.y += IncY;
		}
	}
}

void CFastDrawDC::Draw_Ellipse(CRect &rtEllipse, COLORREF crColor)
{
	ASSERT(__pBuffer);

	if((__rtScreen & rtEllipse).IsRectEmpty()) return;

	int		x = 0;
	int		y = (long)(rtEllipse.Height()	* 0.5f);
	long	a = (long)(rtEllipse.Width()	* 0.5f);
	long	b = (long)(rtEllipse.Height()	* 0.5f);

	long	a_squ = a * a;
	long	b_squ = b * b;
	long	two_a_squ = a_squ << 1;
	long	two_b_squ = b_squ << 1;
	long	d(0), dx(0), dy(0);

	d = b_squ - a_squ * b + (a_squ >> 2);
	dx = 0;
	dy = two_a_squ * b;

	CPoint ptCenter = rtEllipse.CenterPoint();

	while( dx < dy )
	{
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptCenter.x + x, ptCenter.y + y, crColor);
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptCenter.x - x, ptCenter.y + y, crColor);
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptCenter.x + x, ptCenter.y - y, crColor);
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptCenter.x - x, ptCenter.y - y, crColor);

		if( d > 0 )
		{
			y --;
			dy -= two_a_squ;
			d -= dy;
		}
		x ++;
		dx += two_b_squ;
		d += b_squ + dx;
	}

	d += ( 3 * (a_squ - b_squ) / 2 - (dx + dy) / 2 );

	while( y >= 0 )
	{
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptCenter.x + x, ptCenter.y + y, crColor);
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptCenter.x - x, ptCenter.y + y, crColor);
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptCenter.x + x, ptCenter.y - y, crColor);
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptCenter.x - x, ptCenter.y - y, crColor);

		if( d < 0 )
		{
			x ++;
			dx += two_b_squ;
			d += dx;
		}
		y --;
		dy -= two_a_squ;
		d += a_squ - dy;
	}

}

void CFastDrawDC::Draw_Rectangle(CRect &rtRect, COLORREF crColor)
{
	ASSERT(__pBuffer);

	if((__rtScreen & rtRect).IsRectEmpty()) return;

	long nSX = max(rtRect.left, 0);
	long nEX = min(rtRect.right, __bmpInfo.bmWidth);
	long nSY = max(rtRect.top, 0);
	long nEY = min(rtRect.bottom, __bmpInfo.bmHeight);

	for(long nX = nSX; nX < nEX; nX++)
	{
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, nX, rtRect.top,			crColor);
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, nX, rtRect.bottom - 1,	crColor);
	}

	for(long nY = nSY; nY < nEY; nY++)
	{
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, rtRect.left,	nY, crColor);
		(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, rtRect.right - 1,	nY, crColor);
	}
}

void CFastDrawDC::Draw_Point(LONG &nX, LONG &nY, COLORREF crColor)
{
	ASSERT(__pBuffer);

	(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, nX, nY, crColor);
}

void CFastDrawDC::Draw_Point(CPoint &ptPoint, COLORREF crColor)
{
	ASSERT(__pBuffer);

	(*__pCallDrawPpoint)(&__bmpInfo, __pBuffer, ptPoint.x, ptPoint.y, crColor);
}

void CFastDrawDC::Draw_Arc(CPoint &ptCenter, long nRadiusX, long nRadiusY, float fAngleS, float fAngleE, COLORREF crColor, CPoint *pDstPoint_S, CPoint *pDstPoint_E)
{
	const double PI = 3.1415926535897932384626433832795;

	//	BOOL cw = TRUE;	// 시계방향

	float fArc = fAngleE - fAngleS;
	fArc = fabs(fArc) > 360 ? 360.f : fArc;

	long	nSegs		= 45;
	float	fSegAngle	= fArc / nSegs;

	float	fTa		= (float)((fSegAngle / 180.f) * PI);
	float	fAngle	= (float)((fAngleS / 180.f) * PI);

	CPoint ptS;
	CPoint ptE;

	ptS.x = (long)(ptCenter.x + cos(fAngle) * nRadiusX);
	ptS.y = (long)(ptCenter.y + sin(fAngle) * nRadiusY);
	ptE = ptS;

	if(pDstPoint_S) *pDstPoint_S = ptS;

	//g.moveTo(sx,sy);

	for(long i = 0; i < nSegs; i++)
	{
		fAngle += fTa;
		//		float fAm = fAngle-(fTa/2);

		ptE.x = (long)(ptCenter.x + cos(fAngle) * nRadiusX);
		ptE.y = (long)(ptCenter.y + sin(fAngle) * nRadiusY);

		Draw_Line(ptS, ptE, crColor);

		ptS = ptE;
	}

	if(pDstPoint_E) *pDstPoint_E = ptE;
}

void CFastDrawDC::__Draw_Point16(BITMAP *pBmpInfo, BYTE *pBuffer, const LONG &nX, const LONG &nY, COLORREF crColor)
{
	long nPt = nY * pBmpInfo->bmWidthBytes + nX * pBmpInfo->bmBitsPixel / 8;

	if(nX >= 0 && nY >= 0 && nY < pBmpInfo->bmHeight && nX < pBmpInfo->bmWidth)
	{
		USHORT &CurBuffer = (USHORT &)pBuffer[nPt];

		CurBuffer = ((GetRValue(crColor) >> 3) << 11)	|
		            ((GetGValue(crColor) >> 2) << 6)	|
		            ((GetBValue(crColor) >> 3));
	}
}

void CFastDrawDC::__Draw_Point24(BITMAP *pBmpInfo, BYTE *pBuffer, const LONG &nX, const LONG &nY, COLORREF crColor)
{
	long nPt = nY * pBmpInfo->bmWidthBytes + nX * pBmpInfo->bmBitsPixel / 8;

	if(nX >= 0 && nY >= 0 && nY < pBmpInfo->bmHeight && nX < pBmpInfo->bmWidth)
	{
		pBuffer[nPt++	] = GetBValue(crColor);
		pBuffer[nPt++	] = GetGValue(crColor);
		pBuffer[nPt		] = GetRValue(crColor);
	}
}

void CFastDrawDC::__Draw_Point32(BITMAP *pBmpInfo, BYTE *pBuffer, const LONG &nX, const LONG &nY, COLORREF crColor)
{
	long nPt = nY * pBmpInfo->bmWidthBytes + nX * (pBmpInfo->bmBitsPixel / 8);

	if(nX >= 0 && nY >= 0 && nY < pBmpInfo->bmHeight && nX < pBmpInfo->bmWidth)
	{
		pBuffer[nPt++	] = GetBValue(crColor);
		pBuffer[nPt++	] = GetGValue(crColor);
		pBuffer[nPt++	] = GetRValue(crColor);
		pBuffer[nPt		] = 0;
	}
}
