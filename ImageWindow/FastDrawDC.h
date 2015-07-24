#pragma once

class CFastDrawDC
{
public:
	CFastDrawDC(void);
public:
	~CFastDrawDC(void);

public:
	BOOL Draw_Ready		(CDC *pDC);
	void Draw_Excute	();

	void Draw_Line		(CPoint &ptS, CPoint &ptE, COLORREF crColor);
	void Draw_Ellipse	(CRect &rtEllipse,		COLORREF crColor);
	void Draw_Rectangle	(CRect &rtRect,			COLORREF crColor);
	void Draw_Point		(LONG &nX, LONG &nY,	COLORREF crColor);
	void Draw_Point		(CPoint &ptPoint,		COLORREF crColor);
	void Draw_Arc		(CPoint &ptCenter, long nRadiusX, long nRadiusY, float fAngleS, float fAngleE, COLORREF crColor, CPoint *pDstPoint_S, CPoint *pDstPoint_E);

private:
	CDC*		__pDC;
	BITMAP		__bmpInfo;
	BYTE*		__pBuffer;
	CRect		__rtScreen;

	void (*__pCallDrawPpoint)(BITMAP *pBmpInfo, BYTE *pBuffer, const LONG &nX, const LONG &nY, COLORREF crColor);

	static void __Draw_Point16(BITMAP *pBmpInfo, BYTE *pBuffer, const LONG &nX, const LONG &nY, COLORREF crColor);
	static void __Draw_Point24(BITMAP *pBmpInfo, BYTE *pBuffer, const LONG &nX, const LONG &nY, COLORREF crColor);
	static void __Draw_Point32(BITMAP *pBmpInfo, BYTE *pBuffer, const LONG &nX, const LONG &nY, COLORREF crColor);
};
