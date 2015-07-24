#include "stdafx.h"
#include "Screenshot.h"


bool CScreenshot::Shot(CDC* pDC, const CRect& rtROI, HBITMAP hBitmap)
{
	CBitmap bitmap;
	bitmap.Attach(hBitmap);

	BITMAP bi;
	bitmap.GetBitmap(&bi);

	if(bi.bmWidth != rtROI.Width() || bi.bmHeight != rtROI.Height()) return false;

	CDC MemDC;	
	MemDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = MemDC.SelectObject(&bitmap);

	//long nOldStretchMode = pDC->SetStretchBltMode(HALFTONE);

	MemDC.BitBlt(0, 0, rtROI.Width(), rtROI.Height(), pDC, rtROI.left, rtROI.top, /*rtROI.Width(), rtROI.Height(),*/ SRCCOPY);

	//pDC->SetStretchBltMode(nOldStretchMode);

	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC();

	bitmap.Detach();

	return true;
}