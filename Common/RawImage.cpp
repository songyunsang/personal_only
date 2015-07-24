#include "StdAfx.h"
#include "RawImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRawImage::CRawImage(void)
: __bAllocateMemory(FALSE)
, __nAllocateSize(0)
, __nImageStepX(0)
, __nImageSizeX(0)
, __nImageSizeY(0)
, __nBpp(0)
, __pbyImageBuffer(NULL)
, __pbyImageAllocBuffer(NULL)
{
}

CRawImage::~CRawImage(void)
{
	FreeMemory();
}

BOOL CRawImage::CreateClone(const CRawImage *pObject)
{
	if(pObject)
	{
		Create(pObject->__nImageSizeX, pObject->__nImageSizeY, pObject->__nBpp);
		return Copy(pObject);
	}

	FreeMemory();
	return TRUE;
}

BOOL CRawImage::CreateClone(const CRawImage &object)
{
	Create(object.__nImageSizeX, object.__nImageSizeY, object.__nBpp);

	return Copy(object);
}

BOOL CRawImage::Create(CBitmap* i_pBitmap)
{
	BITMAP bmpInfo;
	i_pBitmap->GetBitmap(&bmpInfo);

	if(!Create(bmpInfo.bmWidthBytes, bmpInfo.bmWidth, bmpInfo.bmHeight, bmpInfo.bmBitsPixel))
	{
		return FALSE;
	}

	i_pBitmap->GetBitmapBits(bmpInfo.bmWidthBytes * bmpInfo.bmHeight, __pbyImageBuffer);

	return TRUE;
}

BOOL CRawImage::Create(long nImgWithBytes, long nImgSizeX, long nImgSizeY, long nBpp)
{
	LPVOID lpMsgBuf;

	long nNewImgWidthBytes = nImgWithBytes;

	if(__pbyImageAllocBuffer && __bAllocateMemory)
	{
		// 이미 할당되었으나 사이즈가 다르면 재할당 하고 같으면 그냥 TRUE를 리턴한다.
		if(nNewImgWidthBytes * nImgSizeY > __nAllocateSize)
		{
			FreeMemory();
		}
		else
		{
			__nImageSizeX		= nImgSizeX;
			__nImageSizeY		= nImgSizeY;
			__nImageStepX	= nNewImgWidthBytes;
			__pbyImageBuffer	= __pbyImageAllocBuffer;		// 새로 만드는 작업이므로 __pbyImageBuffer 를 원복한다. 이현민
			__nBpp				= nBpp;

			return TRUE;
		}
	}

	// 메모리 할당을 해야 하면 GlobalAlloc을 사용한다.

	__bAllocateMemory	= TRUE;
	__nAllocateSize		= nNewImgWidthBytes * nImgSizeY;
	__pbyImageBuffer	= __pbyImageAllocBuffer	= (BYTE*)GlobalAlloc(GPTR, __nAllocateSize);

//	DWORD dwErr = GetLastError();

	if(__pbyImageBuffer == NULL)
	{
		// 할당이 안될경우 로그에 원인을 남긴다.
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);

		LocalFree(lpMsgBuf);

		MEMORYSTATUS ms;
		GlobalMemoryStatus(&ms);

		ASSERT(0);

		__nAllocateSize			= 0;
		__bAllocateMemory		= FALSE;
		__pbyImageBuffer		= NULL;
		__pbyImageAllocBuffer	= NULL;
		__nImageSizeX			= 0;
		__nImageSizeY			= 0;
		__nBpp					= 0;
		__nImageStepX		= 0;

		return FALSE;
	}

	__nImageSizeX		= nImgSizeX;
	__nImageSizeY		= nImgSizeY;
	__nBpp				= nBpp;
	__nImageStepX	= nNewImgWidthBytes;

	return TRUE;
}

BOOL CRawImage::Create(long nImgSizeX, long nImgSizeY, long nBpp)
{
	return Create((nImgSizeX+3) / 4 * 4 * (nBpp / 8), nImgSizeX, nImgSizeY, nBpp);
}

BOOL CRawImage::CreateCompatibleImage(const CRawImage *pObject)
{
	return Create(	pObject->GetImageStepX(),
					pObject->GetImageSizeX(),
					pObject->GetImageSizeY(),
					pObject->GetImageBpp());
}

BOOL CRawImage::CreateCompatibleImage(const CRawImage &object)
{
	return CreateCompatibleImage(&object);
}

BOOL CRawImage::FreeMemory()
{
	if(__bAllocateMemory && __pbyImageAllocBuffer)
	{
		GlobalFree(__pbyImageAllocBuffer);

		__bAllocateMemory		= FALSE;
		__nAllocateSize			= 0;
		__pbyImageAllocBuffer	= NULL;
	}

	__pbyImageBuffer		= NULL;
	__nImageSizeX			= 0;
	__nImageSizeY			= 0;
	__nBpp					= 0;
	__nImageStepX		= 0;

	return TRUE;
}

BOOL CRawImage::Save_Bmp(LPCTSTR lpFileName) const
{
	return Save_Bmp(lpFileName, GetTotalROI());
}

BOOL CRawImage::Load_Bmp(LPCTSTR lpFileName)
{
	CFile file;
	if(!file.Open(lpFileName, CFile::modeRead)) return FALSE;

	CArchive ar(&file, CArchive::load);

	// ZMap Load
	BITMAPINFOHEADER bi;
	BITMAPFILEHEADER bf;
	long btpp;

	//---------------------------------------------------------------------------------------
	// 파일의 2바이트가 BM인지 확인한다.
	//---------------------------------------------------------------------------------------
	if(!ar.Read(&bf, sizeof(BITMAPFILEHEADER)))
	{
		return FALSE;
	}

	if(bf.bfType != 0x4d42)
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------------------
	// BITMAPINFOHEADER를 읽어온다.
	//---------------------------------------------------------------------------------------
	if(!ar.Read(&bi, sizeof(BITMAPINFOHEADER)))
	{
		return FALSE;
	}

	BOOL bFlip = FALSE;

	if(bi.biHeight<0)	
	{
		bi.biHeight=bi.biHeight*-1;
		bFlip =TRUE;
	}
	if(bi.biBitCount != 8 && bi.biBitCount != 16 && bi.biBitCount != 24 && bi.biBitCount != 32)
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------------------
	// bi.Width와 bi.Height만큼 공간을 체크한다.
	//---------------------------------------------------------------------------------------
	btpp = bi.biBitCount/8;

	long aWidthByte		= (bi.biWidth * btpp);
	long aRealWidthByte	= ((aWidthByte + 3) / 4) * 4;

	if(!Create(bi.biWidth, bi.biHeight, bi.biBitCount)) return FALSE;

	//---------------------------------------------------------------------------------------
	// 8비트 영상일 경우 4*256만큼 건너뛴다. (Gray 팔레트는 읽을 필요가 없다.)
	//---------------------------------------------------------------------------------------

	if(bi.biBitCount == 8)
	{
		BYTE pBuffer[4*256];
		if(ar.Read(pBuffer, 4* 256) < 0)
		{
			delete []__pbyImageAllocBuffer;

			__nImageSizeX			= 0;
			__nImageSizeY			= 0;
			__nImageStepX		= 0;
			__nBpp					= 0;
			__nAllocateSize			= 0;
			__pbyImageBuffer		= NULL;
			__pbyImageAllocBuffer	= NULL;

			return FALSE;
		}
	}

	//---------------------------------------------------------------------------------------
	//  이미지의 사이즈만큼 읽어들인다.
	//---------------------------------------------------------------------------------------
	
	if(bFlip)
	{
		for(int i=0; i<bi.biHeight; i++)
		{
			ar.Read(__pbyImageBuffer + (i * __nImageStepX), aRealWidthByte);
		}
	}
	else
	{
		for(int i=bi.biHeight-1; i>=0; --i)
		{
			ar.Read(__pbyImageBuffer + (i * __nImageStepX), aRealWidthByte);
		}
	}

	 file.Close();

	return TRUE;
}

BOOL CRawImage::Save_Bmp(LPCTSTR lpFileName, const CRect &rtROI) const
{
	//=================================
    // 1. 컬러가 256Gray인지/RGB24인지 체크한다.
	//=================================
//	if(__nBpp != 8 && __nBpp != 24 && __nBpp != 32) return FALSE;

	CFile FileBmp;

	if(!FileBmp.Open(lpFileName, CFile::modeCreate | CFile::modeWrite))
	{
		return FALSE;
	}

	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	int nWidth = rtROI.Width();
	int nHeight = rtROI.Height();

	RGBQUAD pal[256];
	int i;
	int btpp = __nBpp/8;
	long nOrgWidthBytes = nWidth * btpp;
	long awidth = (nOrgWidthBytes+4) - ((nOrgWidthBytes%4)?(nOrgWidthBytes%4):4);
	int ntempbuff = awidth - nOrgWidthBytes;

	char tempbuffer[12] = {0,};
	long start, end, inc;

	//=================================
	// 3. 비트맵 파일 헤더를 저장
	//=================================
	memset( &bfh, 0, sizeof( BITMAPFILEHEADER ) );
	bfh.bfType = 0x4d42;
	bfh.bfSize = sizeof( bfh ) + (nWidth * nHeight * (__nBpp/8)) + sizeof( BITMAPINFOHEADER );
	if(__nBpp != 8)
		bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
	else bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER ) + (sizeof(RGBQUAD) * 256);

	FileBmp.Write(&bfh, sizeof(BITMAPFILEHEADER));

	//=================================
	// 4. 비트맵 정보 헤더를 저장
	//=================================
	memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
	bih.biSize = sizeof( bih );
	bih.biWidth = nWidth;
	bih.biHeight = nHeight;
	bih.biPlanes = 1;
	bih.biBitCount = (unsigned short)__nBpp;

	FileBmp.Write(&bih, sizeof(BITMAPINFOHEADER));

	//=================================
	// 5. 8bit Gray 팔레트를 저장
	//=================================
	if(__nBpp == 8)
	{
		for(i=0; i<256; ++i)
		{
			pal[i].rgbBlue	= (BYTE)i;
			pal[i].rgbGreen	= (BYTE)i;
			pal[i].rgbRed	= (BYTE)i;
			pal[i].rgbReserved = 0;
		}
		FileBmp.Write(pal, sizeof(RGBQUAD) * 256);
	}

	//=================================
	// 6. 데이터를 저장한다.
	//=================================
	BOOL bFlip = TRUE;
	
	if(bFlip)
	{
		start	= rtROI.bottom-1;
		end		= rtROI.top-1;
		inc		= -1;
	}
	else
	{
		start	= rtROI.top;
		end		= rtROI.bottom;
		inc		= 1;
	}

	for(i=start; i!=end; i+=inc)
	{
		// 1줄을 저장한다. 
		FileBmp.Write(__pbyImageBuffer + (i*__nImageStepX) + (rtROI.left*btpp), nWidth * btpp);

		// 빈칸을 저장한다. 
		if(ntempbuff)
		{
			FileBmp.Write(tempbuffer, ntempbuff);
		}
	}

	/***********************************
      7. 파일을 닫는다.
	***********************************/

	FileBmp.Close();

	return TRUE;
}

BOOL CRawImage::SoftGain(float fGainFactor)
{
	if(fGainFactor==1.f) return TRUE;

	if(__nBpp!=8)
	{
		ASSERT(!_T("아직 지원안함"));
		return FALSE;
	}

	long nOffsetY = 0;

	for(long nY=0; nY<__nImageSizeY; ++nY, nOffsetY+= __nImageStepX)
	{
		long nOffset = nOffsetY;

		for(long nX=0; nX<__nImageSizeX; ++nX, ++nOffset)
		{
			float fValue = __pbyImageBuffer[nOffset] * fGainFactor;

			fValue = max(fValue, 0.f);
			fValue = min(fValue, 255.f);
		
			__pbyImageBuffer[nOffset] = (BYTE)fValue;
		}
	}

	return TRUE;
}

void CRawImage::SetPtr(long nImgWithBytes, long nImgSizeX, long nImgSizeY, long nBpp, BYTE *pBuffer)
{
	FreeMemory();

	__bAllocateMemory		= FALSE;
	__nImageStepX		= nImgWithBytes;
	__nImageSizeX			= nImgSizeX;
	__nImageSizeY			= nImgSizeY;
	__nBpp					= nBpp;
	__pbyImageBuffer		= pBuffer;
	__pbyImageAllocBuffer	= NULL;
}

void CRawImage::SetPtr(CRawImage *pImage)
{
	FreeMemory();

	if(pImage)
	{
		__bAllocateMemory		= FALSE;
		__nImageStepX		= pImage->__nImageStepX;
		__nImageSizeX			= pImage->__nImageSizeX;
		__nImageSizeY			= pImage->__nImageSizeY;
		__nBpp					= pImage->__nBpp;
		__pbyImageBuffer		= pImage->__pbyImageBuffer;
		__pbyImageAllocBuffer	= NULL;
	}
}

BOOL CRawImage::Copy(const CRawImage *pImage)
{
	return Copy(*pImage);
}

BOOL CRawImage::Copy(const CRawImage &image)
{
	if(__nImageSizeX	== image.__nImageSizeX &&
		__nImageSizeY	== image.__nImageSizeY &&
		__nBpp			== image.__nBpp)
	{
		BYTE *pbyImage1 = __pbyImageBuffer;
		BYTE *pbyImage2 = image.__pbyImageBuffer;

		long nCopySize = __nImageSizeX * (__nBpp / 8);

		for(long nY=0; nY<__nImageSizeY; ++nY)
		{
			memcpy(pbyImage1, pbyImage2, nCopySize);

			pbyImage1 += __nImageStepX;
			pbyImage2 += image.__nImageStepX;
		}

		TRUE;
	}

	return FALSE;
}


BOOL CRawImage::Copy(const CRawImage *pImage, const CRect& rtROI)
{
	if(rtROI==pImage->GetTotalROI()) return Copy(pImage);

	if(pImage->GetImageBpp()	!= __nBpp)			return FALSE;
	if(__nImageSizeX			< rtROI.Width())	return FALSE;
	if(__nImageSizeY			< rtROI.Height())	return FALSE;

	long nByte = __nBpp / 8;
	BYTE *pbyImage1 = __pbyImageBuffer;
	BYTE *pbyImage2 = pImage->__pbyImageBuffer + rtROI.top * pImage->__nImageStepX + rtROI.left * nByte;

	long nCopySize = rtROI.Width() * (__nBpp / 8);

	for(long nY=rtROI.top; nY<rtROI.bottom; ++nY)
	{
		memcpy(pbyImage1, pbyImage2, nCopySize);

		pbyImage1 += __nImageStepX;
		pbyImage2 += pImage->__nImageStepX;
	}

	return TRUE;
}

void CRawImage::Clear()
{
	if(__pbyImageBuffer == NULL) return;

	for(long nY=0; nY<__nImageSizeY; ++nY)
	{
		//memset(__pbyImageBuffer + nY * __nImgWidthBytes, 0, __nImageSizeX);
		memset(__pbyImageBuffer + nY * __nImageStepX, 0, __nImageSizeX * (__nBpp / 8));
	}
}

void CRawImage::Cut(const CRect& rtROI)
{
	__pbyImageBuffer	+= rtROI.top * __nImageStepX + rtROI.left * (__nBpp / 8);
	__nImageSizeX		 = rtROI.Width();
	__nImageSizeY		 = rtROI.Height();
}

BOOL CRawImage::ConvertCompatibleBitmap(CDC *pDC, CBitmap *o_pBitmap)
{
	o_pBitmap->DeleteObject();
	if(__pbyImageBuffer == NULL || __nBpp==NULL) return FALSE;

	char buf[ sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256 ];		// 비트맵 헤더정보와 팔레트 정보를 셋팅할 버퍼
	BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER*)buf;				// 비트맵 인포헤더
	COLORREF* rgb = (COLORREF*)( buf + sizeof(BITMAPINFOHEADER) );	// 팔레트

	if(__nBpp == 8)
	{
		// 그레이 팔레트 만들기
		for(int i = 0; i < 256; ++i) rgb[i] = RGB( i,i,i );
	}

	dibhdr->biSize = sizeof( BITMAPINFOHEADER );				// 사이즈 
	dibhdr->biWidth = __nImageStepX / (__nBpp/8);			// 가로길이 
	dibhdr->biHeight = -__nImageSizeY;							// 세로길이 
	dibhdr->biPlanes = 1;										// 플레인 수 
	dibhdr->biBitCount = (WORD)__nBpp;							// 비트 퍼 픽셀 
	dibhdr->biCompression = BI_RGB;								// 압축 여부 
	dibhdr->biSizeImage = __nImageStepX *  __nImageSizeY;		// 이미지 사이즈 
	dibhdr->biXPelsPerMeter = 0;												
	dibhdr->biYPelsPerMeter = 0;
	dibhdr->biClrUsed = 0;
	dibhdr->biClrImportant = 0; 

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);

	o_pBitmap->CreateCompatibleBitmap(pDC, __nImageSizeX, __nImageSizeY);
	CBitmap *pOldBitmap = MemDC.SelectObject(o_pBitmap);

	// 팔레트를 얻어 DC에 선택
	::SetStretchBltMode(MemDC.m_hDC, COLORONCOLOR);

	::StretchDIBits(MemDC.m_hDC, 	// hDC
		0,							// DestX
		0,							// DestY
		__nImageSizeX,				// nDestWidth
		__nImageSizeY,				// nDestHeight
		0,							// SrcX
		0,							// SrcY
		__nImageSizeX,				// wSrcWidth
		__nImageSizeY,				// wSrcHeight
		__pbyImageBuffer,			// lpBits
		(LPBITMAPINFO)dibhdr,		// lpBitsInfo
		DIB_RGB_COLORS,				// wUsage
		SRCCOPY);					// dwROP

	MemDC.SelectObject(pOldBitmap);

	return TRUE;
}

BOOL CRawImage::IsEqualImageFormat(const CRawImage *pImage) const
{
	return IsEqualImageFormat(*pImage);
}

BOOL CRawImage::IsEqualImageFormat(const CRawImage &image) const
{
	if(	image.__nImageSizeX		!= __nImageSizeX	||
		image.__nImageSizeY		!= __nImageSizeY	||
		image.__nImageStepX	!= __nImageStepX	||
		image.__nBpp			!= __nBpp)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRawImage::IsEqualImageSize(const CRawImage *pImage)
{
	if(	pImage->__nImageSizeX != __nImageSizeX ||
		pImage->__nImageSizeY != __nImageSizeY)
	{
		return FALSE;
	}

	return TRUE;
}

long CRawImage::GetImageStepX() const
{
	return this ? __nImageStepX : 0;
}

long CRawImage::GetImageSizeX() const
{
	return this ? __nImageSizeX : 0;
}

long CRawImage::GetImageSizeY() const
{
	return this ? __nImageSizeY : 0;
}

long CRawImage::GetImageBpp() const
{
	return this ? __nBpp : 0;
}

BYTE* CRawImage::GetImageBuffer()
{
	return this ? __pbyImageBuffer : NULL;
}

const BYTE* CRawImage::GetImageBuffer() const
{
	return this ? __pbyImageBuffer : NULL;
}

CRect CRawImage::GetTotalROI() const
{
	return this ? CRect(0, 0, __nImageSizeX, __nImageSizeY) : CRect(0, 0, 0, 0);
}

BYTE*	CRawImage::GetMem(long x, long y)
{
	return this ? __pbyImageBuffer + __nImageStepX * y + x * (__nBpp / 8) : NULL;
}

const BYTE*	CRawImage::GetMem(long x, long y) const
{
	return this ? __pbyImageBuffer + __nImageStepX * y + x * (__nBpp / 8) : NULL;
}

BYTE*	CRawImage::GetMem(const CPoint &pt)
{
	return this ? __pbyImageBuffer + __nImageStepX * pt.y + pt.x * (__nBpp / 8) : NULL;
}

const BYTE*	CRawImage::GetMem(const CPoint &pt) const
{
	return this ? __pbyImageBuffer + __nImageStepX * pt.y + pt.x * (__nBpp / 8) : NULL;
}

BYTE*	CRawImage::GetMem(const CRect &rtROI)
{
	return this ? __pbyImageBuffer + __nImageStepX * rtROI.top + rtROI.left * (__nBpp / 8) : NULL;
}

const BYTE*	CRawImage::GetMem(const CRect &rtROI) const
{
	return this ? __pbyImageBuffer + __nImageStepX * rtROI.top + rtROI.left * (__nBpp / 8) : NULL;
}

BYTE CRawImage::GetSubpixValue_Bilinear(float x, float y) const
{
	if(x < 0. || y < 0.)
		return 0;

	long x1 = (long)x;
	long y1 = (long)y;

	if(x1+1 >= __nImageSizeX || y1+1 >= __nImageSizeY)
		return 0;

	float h1 = static_cast<float>(y - y1);
	float h2 = static_cast<float>(y1 + 1 - y);
	float w1 = static_cast<float>(x - x1);
	float w2 = static_cast<float>(x1 + 1 - x);

	long nOffset = y1 * __nImageStepX + x1;

	unsigned char p1 = __pbyImageBuffer[nOffset];
	unsigned char p2 = __pbyImageBuffer[nOffset + 1];
	unsigned char p3 = __pbyImageBuffer[nOffset + __nImageStepX + 1];
	unsigned char p4 = __pbyImageBuffer[nOffset + __nImageStepX];

	return (unsigned char)( w2*h2*p1 + w1*h2*p2 + w1*h1*p3 + w2*h1*p4 + 0.5 );
}

__forceinline float CubicInterpolation(float t, const BYTE *p)
{
	return (p[1] + 0.5f * t*(p[2] - p[0] + t*(2.f*p[0] - 5.f*p[1] + 4.f*p[2] - p[3] + t*(3.f*(p[1] - p[2]) + p[3] - p[0]))));
}

__forceinline float CubicInterpolation(float t, const float *p)
{
	return (p[1] + 0.5f * t*(p[2] - p[0] + t*(2.f*p[0] - 5.f*p[1] + 4.f*p[2] - p[3] + t*(3.f*(p[1] - p[2]) + p[3] - p[0]))));
}

BYTE CRawImage::GetSubpixValue_Bicubic(float x, float y) const
{
	const long x0 = (long)x;
	const long y0 = (long)y;

	if(x0-1 < 0 || y0-1 < 0 || x0+2 >= __nImageSizeX || y0+2 >= __nImageSizeY)
	{
		return 0;
	}

	const float xOffset = x - x0;
	const float yOffset = y - y0;

	BYTE *pbyTemp = __pbyImageBuffer + (y0 - 1) * __nImageStepX + x0 - 1;

	float p[4] =
	{
		CubicInterpolation(xOffset, pbyTemp),
		CubicInterpolation(xOffset, pbyTemp+=__nImageStepX),
		CubicInterpolation(xOffset, pbyTemp+=__nImageStepX),
		CubicInterpolation(xOffset, pbyTemp+=__nImageStepX)
	};

	float fValue = min(CubicInterpolation(yOffset, p), 255.f);

	return BYTE(fValue + 0.5f);
}
