#include "stdafx.h"
#include "DIBImage.h"


CDIBImage::CDIBImage(void)
	: m_hDIB(NULL)
	, m_nSizeX(0)
	, m_nSizeY(0)
	, m_nBpp(0)
	, m_nStepX(0)
	, m_pBitmapInfoBuffer(NULL)
{
}

CDIBImage::~CDIBImage(void)
{
	DIBRelease();
}

long CDIBImage::GetStepX()
{
	return m_nStepX;	
}

HBITMAP	CDIBImage::GetDIB()
{
	return m_hDIB;
}

BYTE* CDIBImage::GetPtr()
{
	return (BYTE*)m_pPixelRef;
}

long CDIBImage::GetSizeX()
{
	return m_nSizeX;
}

long CDIBImage::GetSizeY()
{
	return m_nSizeY;
}

long CDIBImage::GetBpp()
{
	return m_nBpp;
}

HBITMAP CDIBImage::DIBAlloc(CDC* pDC, long width, long height, long bpp, BOOL bTopToBottom)
{
	if( m_nSizeX == width && m_nSizeY == height && m_nBpp == bpp ) return m_hDIB;

	DIBRelease();

	m_pBitmapInfoBuffer = new BYTE[sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256];
	memset(m_pBitmapInfoBuffer, NULL, sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256);

	BITMAPINFOHEADER *dibhdr = (BITMAPINFOHEADER *)m_pBitmapInfoBuffer;							// 비트맵 인포헤더
	COLORREF *pPallete = (COLORREF *)(m_pBitmapInfoBuffer + sizeof(BITMAPINFOHEADER) );				// 팔레트

	if(bpp == 8)
	{
		for(int i = 0; i < 256; i++) pPallete[i] = RGB( i, i, i );							// 그레이 팔레트 만들기 */
	}

	long widthBytes = (((width * bpp + 31) & ~31) >> 3);

	dibhdr->biSize = sizeof( BITMAPINFOHEADER );								// 사이즈
	dibhdr->biWidth = width;							// 가로길이
	dibhdr->biHeight = bTopToBottom ? -height : height;							// 세로길이
	dibhdr->biPlanes = 1;														// 플레인 수
	dibhdr->biBitCount = (WORD)bpp;												// 비트 퍼 픽셀
	dibhdr->biCompression = BI_RGB;												// 압축 여부
	dibhdr->biSizeImage = widthBytes * height;									// 이미지 사이즈
	dibhdr->biXPelsPerMeter = 0;
	dibhdr->biYPelsPerMeter = 0;
	dibhdr->biClrUsed = 0;
	dibhdr->biClrImportant = 0;

	m_hDIB = ::CreateDIBSection(pDC->GetSafeHdc(), (BITMAPINFO*)m_pBitmapInfoBuffer, DIB_RGB_COLORS, (void **)&(m_pPixelRef), 0, 0);

	if( m_hDIB == NULL)
	{
		ASSERT(0);
		m_nSizeX = 0;
		m_nSizeY = 0;
		m_nBpp = 0;

		return NULL;
	}

	m_nSizeX = width;
	m_nSizeY = height;
	m_nBpp = bpp;
	m_nStepX = widthBytes;

	return m_hDIB;
}

void CDIBImage::DIBRelease()
{	
	DeleteObject(m_hDIB);

	m_hDIB = NULL;

	delete [] m_pBitmapInfoBuffer;

	m_pBitmapInfoBuffer = nullptr;

}

HBITMAP CDIBImage::Create(CDC* pDC, long nSizeX, long nSizeY, long nBpp)
{
	return DIBAlloc(pDC, nSizeX, nSizeY, nBpp, TRUE);
}

void CDIBImage::SaveBMP(LPCTSTR szFilePath)
{
	BITMAPFILEHEADER bmpFileHeader;

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pBitmapInfoBuffer;

	// DIB 파일의 내용을 구성한다.
	bmpFileHeader.bfReserved1 = 0;
	bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfSize  = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pBitmapInfo->bmiHeader.biSizeImage;
	bmpFileHeader.bfType  = 'MB';
	bmpFileHeader.bfOffBits  = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// DIB 파일을 생성한다.
	CFile file;
	BOOL bOK = file.Open(szFilePath, CFile::modeWrite | CFile::modeCreate,NULL);
	if(!bOK)
	{
		return;
	}
	file.Write(&bmpFileHeader, sizeof(BITMAPFILEHEADER));
	file.Write(&pBitmapInfo->bmiHeader, sizeof(BITMAPINFOHEADER));
	file.Write(m_pPixelRef, pBitmapInfo->bmiHeader.biSizeImage);
	file.Close();
}