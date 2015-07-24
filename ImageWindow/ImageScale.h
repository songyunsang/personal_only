#pragma once

class CImageScale
{
public:
	CImageScale(void);
public:
	~CImageScale(void);

public:
	CRect GetSourceROI();
	CRect GetDestROI();

	void SetLeft		(float fX);
	void SetTop			(float fY);
	void IncLeft		(float fX);
	void IncTop			(float fY);

	void SetZoomInc		(float fZoomFactor);
	void SetZoomLevel	(float fZoomLevelX, float fZoomLevelY);
	void SetSize_View	(long nSizeX, long nSizeY);
	void SetSize_Image	(long nSizeX, long nSizeY);

	void SetOffsetPos	(float fOffsetX, float fOffsetY);

	//BOOL SetRange		(BOOL bUse, int nImageWidth, int nImageHeight, float fL, float fT, float fR, float fB);

	float	GetLeft();
	float	GetTop();
	float	GetRight();
	float	GetBottom();

	float	GetZoomX();
	float	GetZoomY();

	//const long&	GetImageWidth() const;
	//const long&	GetImageHeight() const;

	void	ConvertImageToViewPos		(float fX, float fY, float &fOutX, float &fOutY);
	void	ConvertImageDistToViewDist	(float fDist, float &fOutX, float &fOutY);
	void	ConvertViewToImagePos		(float fX, float fY, float &fOutX, float &fOutY);

	CImageScale& operator = (CImageScale &Object);

private:
	CRect	__rtROI_Source;
	CRect	__rtROI_Dest;

	CRect	__rtROI_Range;

	float	__fImageLeft;
	float	__fImageTop;
	float	__fImageRight;
	float	__fImageBottom;

	float	__fImageOffsetX;
	float	__fImageOffsetY;
	//BOOL	__bUseRange;
	//float	__fRangeLeft;
	//float	__fRangeTop;
	//float	__fRangeRight;
	//float	__fRangeBottom;

	long	__nSize_ViewX;
	long	__nSize_ViewY;
	long	__nSize_ImageX;
	long	__nSize_ImageY;

	float	__fZoomScaleX;
	float	__fZoomScaleY;

	float	__fZoomScaleImageX;
	float	__fZoomScaleImageY;
	
	long	__nImageWidth;
	long	__nImageHeight;

	void __CalcData();
};
