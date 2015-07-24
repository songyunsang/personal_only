#pragma once

#include "CoordinateTransformer.h"

class CPixelMeter
{
public:
	BOOL			IsEnable()	{ return m_bEnable;	}
	const CPoint&	GetBegin()	{ return m_ptBegin;	}
	const CPoint&	GetEnd()	{ return m_ptEnd;	}
	
	void			Enable		(BOOL bEnable);
	void			SetUnit		(float fOnePixelUnit, LPCTSTR lpszUnitName);
	void			SetBegin	(const CPoint& ptBegin);
	void			SetEnd		(const CPoint& ptEnd);

	void			Draw		(CDC* pDC);

private:
	void	uDrawPixelTrace(CDC* pdc);
	void	uDrawPixelDistance(CDC* pdc);

	void	__DrawPixelTrace	( CDC* pdc, CPoint pt1, CPoint pt2, COLORREF rgb, BOOL bBig);
	void	__DrawPixelMark		( CDC* pdc, CPoint point, int r);
	void	__DrawPixelDistance	( CDC* pdc, CPoint pt1, CPoint pt2, BOOL bBig);

	CPoint	__CalcCorner(CPoint a, CPoint b);

private:
	BOOL	m_bEnable;
	CPoint	m_ptBegin;			// A
	CPoint	m_ptEnd;			// B
	CPoint	m_ptCorner;			// C : 삼각형의 꼭지점
	float	m_fOnePixelUnit;	// 한 Pixel 몇 Unit인가
	CString	m_strUnitName;		// 단위명

private:
	CFont	m_fntDistanceDisplayBig;
	CFont	m_fntDistanceDisplaySmall;

	const CCoordinateTransformer* m_pCoordTransform;
public:
	CPixelMeter(const CCoordinateTransformer* pCoordTransform);
	~CPixelMeter(void);
};
