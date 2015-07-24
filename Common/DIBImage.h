#pragma once

class CDIBImage
{
public:
	CDIBImage(void);
	~CDIBImage(void);

public:
	HBITMAP Create(CDC* pDC, long nSizeX, long nSizeY, long nBpp);
	void	SaveBMP(LPCTSTR szFilePath);
	long	GetSizeX();
	long	GetSizeY();
	long	GetBpp();
	long	GetStepX();
	HBITMAP	GetDIB();
	BYTE*	GetPtr();
		
private:
	HBITMAP DIBAlloc(CDC* pDC, long nSizeX, long nSizeY, long nBpp, BOOL bTopToBottom);
	
	void	DIBRelease();

	BYTE*	m_pBitmapInfoBuffer;

private:
	HBITMAP	m_hDIB;

	VOID* m_pPixelRef;

	long m_nSizeX;
	long m_nSizeY;
	long m_nBpp;

	long m_nStepX;	
};


