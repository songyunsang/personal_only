#pragma once

struct SWindow_Image
{
public:	
	const BYTE *pbyImage;															///< 이미지 데이터
	long nWidth;
	long nHeight;
	long nBpp;
	long nImageSize;
	long nWidthBytes;

	SWindow_Image()
	{
		pbyImage	= nullptr;
		nWidth		= 0;
		nHeight		= 0;
		nBpp		= 0;
		nImageSize	= 0;
		nWidthBytes	= 0;
	}

	SWindow_Image& operator =(SWindow_Image* pSrc)										// 버퍼 공유
	{
		nWidth		= pSrc->nWidth;
		nHeight		= pSrc->nHeight;
		nBpp		= pSrc->nBpp;
		nImageSize	= pSrc->nImageSize;
		nWidthBytes	= pSrc->nWidthBytes;
		pbyImage	= pSrc->pbyImage;

		return *this;
	}

	BOOL operator ==(const SWindow_Image& src)
	{
		if(nWidth == src.nWidth && 
			nHeight == src.nHeight && 
			nBpp == src.nBpp &&
			nImageSize == src.nImageSize &&
			nWidthBytes == src.nWidthBytes &&
			pbyImage == src.pbyImage)
			return TRUE;
		return FALSE;
	}
};
