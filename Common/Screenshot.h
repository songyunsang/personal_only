#pragma once

class CScreenshot
{
public:
	CScreenshot(void){}
	~CScreenshot(void){}

public:
	static bool Shot(CDC* pDC, const CRect& rtROI, HBITMAP hBitmap);
};


