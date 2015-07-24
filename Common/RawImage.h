#pragma once

class CRawImage
{
public:
	CRawImage(void);
	~CRawImage(void);

public:
	BOOL CreateClone			(const CRawImage *pObject);
	BOOL CreateClone			(const CRawImage &object);
	BOOL Create					(CBitmap* i_pBitmap);
	BOOL Create					(long nImgWithBytes, long nImgSizeX, long nImgSizeY, long nBpp);
	BOOL Create					(long nImgSizeX, long nImgSizeY, long nBpp);
	BOOL CreateCompatibleImage	(const CRawImage *pObject);
	BOOL CreateCompatibleImage	(const CRawImage &object);
	BOOL FreeMemory				();
	BOOL Load_Bmp				(LPCTSTR lpFileName);
	BOOL Save_Bmp				(LPCTSTR lpFileName) const;
	BOOL Save_Bmp				(LPCTSTR lpFileName, const CRect &rtROI) const;

	BOOL SoftGain				(float fGainFactor);

	void SetPtr					(long nImgWithBytes, long nImgSizeX, long nImgSizeY, long nBpp, BYTE *pBuffer);
	void SetPtr					(CRawImage *pImage);

	BOOL Copy					(const CRawImage *pImage);
	BOOL Copy					(const CRawImage &image);
	BOOL Copy					(const CRawImage *pImage, const CRect& rtROI);
	void Clear					();

	void Cut					(const CRect& rtROI);

	BOOL ConvertCompatibleBitmap(CDC *pDC, CBitmap *o_pBitmap);

	BOOL IsEqualImageFormat	(const CRawImage *pImage) const;
	BOOL IsEqualImageFormat	(const CRawImage &image) const;
	BOOL IsEqualImageSize	(const CRawImage *pImage);

	long		GetImageStepX()				const;
	long		GetImageSizeX()						const;
	long		GetImageSizeY()						const;
	long		GetImageBpp()						const;
	const BYTE*	GetImageBuffer()					const;
	CRect		GetTotalROI()						const;
	const BYTE*	GetMem(long x, long y)				const;
	const BYTE*	GetMem(const CPoint &pt)			const;
	const BYTE*	GetMem(const CRect &rtROI)			const;
	BYTE		GetSubpixValue_Bilinear(float x, float y) const;
	BYTE		GetSubpixValue_Bicubic(float x, float y) const;

	BYTE*		GetImageBuffer();
	BYTE*		GetMem(long x, long y);
	BYTE*		GetMem(const CPoint &pt);
	BYTE*		GetMem(const CRect &rtROI);

protected:
	long	__nImageStepX;
	long	__nImageSizeX;
	long	__nImageSizeY;
	long	__nBpp;

	long	__nAllocateSize;
	BOOL	__bAllocateMemory;
	BYTE*	__pbyImageAllocBuffer;
	BYTE*	__pbyImageBuffer;
};
