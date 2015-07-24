#include "stdafx.h"
#include "ImageScale.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CImageScale::CImageScale(void)
: __fImageLeft(0.f)
, __fImageTop(0.f)
, __fImageRight(0.f)
, __fImageBottom(0.f)
, __nSize_ViewX(0)
, __nSize_ViewY(0)
, __nSize_ImageX(0)
, __nSize_ImageY(0)
, __fZoomScaleX(0.5f)
, __fZoomScaleY(0.5f)
, __fZoomScaleImageX(0.5f)
, __fZoomScaleImageY(0.5f)
, __fImageOffsetX(0.f)
, __fImageOffsetY(0.f)
{
}

CImageScale::~CImageScale(void)
{
}

CRect CImageScale::GetSourceROI()
{
	return __rtROI_Source;
}

CRect CImageScale::GetDestROI()
{
	return __rtROI_Dest;
}

void CImageScale::SetLeft(float fX)
{
	__fImageLeft = fX;
	__CalcData();
}

void CImageScale::SetTop(float fY)
{
	__fImageTop	= fY;
	__CalcData();
}

void CImageScale::IncLeft(float fX)
{
	__fImageLeft += fX;
	__CalcData();
}

void CImageScale::IncTop(float fY)
{
	__fImageTop	+= fY;
	__CalcData();
}

void CImageScale::SetZoomInc(float fZoomFactor)
{
	__fZoomScaleX *= fZoomFactor;
	__fZoomScaleY *= fZoomFactor;

	__CalcData();
}

void CImageScale::SetZoomLevel(float fZoomLevelX, float fZoomLevelY)
{
	__fZoomScaleX = fZoomLevelX;
	__fZoomScaleY = fZoomLevelY;

	__CalcData();
}

void CImageScale::SetSize_View(long nSizeX, long nSizeY)
{
	if(nSizeX == __nSize_ViewX && nSizeY == __nSize_ViewY) return;

	__nSize_ViewX = nSizeX;
	__nSize_ViewY = nSizeY;

	__CalcData();
}

void CImageScale::SetSize_Image(long nSizeX, long nSizeY)
{
	__nSize_ImageX = nSizeX;
	__nSize_ImageY = nSizeY;

	__CalcData();
}
float CImageScale::GetLeft()
{
	return __fImageLeft;
}

float CImageScale::GetTop()
{
	return __fImageTop;
}

float CImageScale::GetRight()
{
	return __fImageRight;
}

float CImageScale::GetBottom()
{
	return __fImageBottom;
}

float CImageScale::GetZoomX()
{
	return __fZoomScaleX;
}
float CImageScale::GetZoomY()
{
	return __fZoomScaleY;
}

//BOOL CImageScale::SetRange(BOOL bUse, int nImageWidth, int nImageHeight, float fL, float fT, float fR, float fB)
//{
//	__nImageWidth = nImageWidth;
//	__nImageHeight = nImageHeight;
//
//	// 사이즈 체크는 밖에서 하고 들어와라
//	if(bUse && fL < 0 || fL >= fR || fT < 0 || fT >= fR || fR < 0 || fB < 0 || fR > nImageWidth || fB > nImageHeight)
//	{
//		__bUseRange		= FALSE;
//		return FALSE;
//	}
//
//	__bUseRange		= bUse;
//
//	__fRangeLeft	= fL;
//	__fRangeTop		= fT;
//	__fRangeRight	= fR;
//	__fRangeBottom	= fB;
//
//	__CalcData();
//
//	return TRUE;
//}

void CImageScale::ConvertImageToViewPos(float fX, float fY, float &fOutX, float &fOutY)
{
	fOutX = (fX - __fImageOffsetX - __rtROI_Source.left	) * __fZoomScaleImageX + __rtROI_Dest.left;
	fOutY = (fY - __fImageOffsetY - __rtROI_Source.top	) * __fZoomScaleImageY + __rtROI_Dest.top;
}

void CImageScale::ConvertImageDistToViewDist(float fDist, float &fOutX, float &fOutY)
{
	fOutX = fDist * __fZoomScaleImageX;
	fOutY = fDist * __fZoomScaleImageY;
}

void CImageScale::ConvertViewToImagePos(float fX, float fY, float &fOutX, float &fOutY)
{
	fOutX = __rtROI_Source.left	+ __fImageOffsetX + (fX-__rtROI_Dest.left) / __fZoomScaleImageX;
	fOutY = __rtROI_Source.top	+ __fImageOffsetY + (fY-__rtROI_Dest.top) / __fZoomScaleImageY;
}

CImageScale& CImageScale::operator = (CImageScale &Object)
{
	__rtROI_Source		= Object.__rtROI_Source;
	__rtROI_Dest		= Object.__rtROI_Dest;

	__fImageLeft		= Object.__fImageLeft;
	__fImageTop			= Object.__fImageTop;
	__fImageRight		= Object.__fImageRight;
	__fImageBottom		= Object.__fImageBottom;

	__nSize_ViewX		= Object.__nSize_ViewX;
	__nSize_ViewY		= Object.__nSize_ViewY;
	__nSize_ImageX		= Object.__nSize_ImageX;
	__nSize_ImageY		= Object.__nSize_ImageY;

	__fZoomScaleX		= Object.__fZoomScaleX;
	__fZoomScaleY		= Object.__fZoomScaleY;

	__fZoomScaleImageX	= Object.__fZoomScaleImageX;
	__fZoomScaleImageY	= Object.__fZoomScaleImageY;

	return *this;
}

void CImageScale::__CalcData()
{
	__fImageRight	= __fImageLeft	+ (__nSize_ViewX / __fZoomScaleX);
	__fImageBottom	= __fImageTop	+ (__nSize_ViewY / __fZoomScaleY);

	//if(__bUseRange)
	//{
	//	__rtROI_Source.left		= (long)max(__fRangeLeft, __fRangeLeft	+ __fImageLeft);
	//	__rtROI_Source.top		= (long)max(__fRangeTop, __fRangeTop	+ __fImageTop);
	//	__rtROI_Source.right	= (long)ceil(min(__fRangeRight, __fRangeLeft	+ __fImageRight));
	//	__rtROI_Source.bottom	= (long)ceil(min(__fRangeBottom, __fRangeTop	+ __fImageBottom));

	//	// 화면을 벗어날 경우		
	//}
	//else
	//{
		__rtROI_Source.left		= (long)(__fImageLeft);
		__rtROI_Source.top		= (long)(__fImageTop);
		__rtROI_Source.right	= (long)(__fImageRight);
		__rtROI_Source.bottom	= (long)(__fImageBottom);
		if(__fImageRight	> __rtROI_Source.right)		__rtROI_Source.right++;
		if(__fImageBottom	> __rtROI_Source.bottom)	__rtROI_Source.bottom++;

	//}

	// 스케일에 따라 무시되는 픽셸을 앞에서 빠지게 함.(이게 맞는지 모르겠음)
//	if(__bUseRange)
//	{
		__rtROI_Dest.left	=				- (long)((__fImageLeft	- __rtROI_Source.left)	* __fZoomScaleX + 0.0000005f);
		__rtROI_Dest.top	=				- (long)((__fImageTop	- __rtROI_Source.top)	* __fZoomScaleY + 0.0000005f);
//	}
//	else
//	{
//		__rtROI_Dest.left	=				- (long)((__fImageLeft			- __rtROI_Source.left)	* __fZoomScaleX + 0.0000005f);
//		__rtROI_Dest.top	=				- (long)((__fImageTop			- __rtROI_Source.top)	* __fZoomScaleY + 0.0000005f);
//		__rtROI_Dest.right	=				__rtROI_Dest.left + (__rtROI_Source.Width() * __fZoomScaleX + 0.0000005f);
//		__rtROI_Dest.bottom	=				__rtROI_Dest.top + (__rtROI_Source.Height() * __fZoomScaleY + 0.0000005f);
//		__rtROI_Dest.right	=				(long)((__rtROI_Source.right - __fImageLeft) * __fZoomScaleX + 0.0000005f);
//		__rtROI_Dest.bottom	=				(long)((__rtROI_Source.bottom - __fImageTop) * __fZoomScaleY + 0.0000005f);
//	}
	__rtROI_Dest.right	= (long)(__rtROI_Dest.left + (__rtROI_Source.Width() * __fZoomScaleX + 0.0000005f));			// __nSize_ViewX - ((min(__fRangeRight, __fRangeLeft + __fImageRight) - __rtROI_Source.right) * __fZoomScaleX) + 0.0000005f;
	__rtROI_Dest.bottom	= (long)(__rtROI_Dest.top + (__rtROI_Source.Height() * __fZoomScaleY + 0.0000005f));			// __nSize_ViewY - ((min(__fRangeBottom, __fRangeTop + __fImageBottom) - __rtROI_Source.bottom)  * __fZoomScaleY) + 0.0000005f;

	__fZoomScaleImageX	= (float)__rtROI_Dest.Width() / __rtROI_Source.Width();
	__fZoomScaleImageY	= (float)__rtROI_Dest.Height() / __rtROI_Source.Height();

}

//const long&	CImageScale::GetImageWidth() const
//{
//	return __nImageWidth;
//}
//
//const long&	CImageScale::GetImageHeight() const
//{
//	return __nImageHeight;
//}
//

void CImageScale::SetOffsetPos(float fOffsetX, float fOffsetY)
{
	__fImageOffsetX = fOffsetX;
	__fImageOffsetY = fOffsetY;
}
