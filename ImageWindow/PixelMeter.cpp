#include "StdAfx.h"
#include "Resource.h"
#include <math.h>
#include <vector>
#include "PixelMeter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPixelMeter::CPixelMeter(const CCoordinateTransformer* pCoordTransform)
	: m_bEnable			(FALSE)
	, m_ptBegin			(CPoint(0, 0))
	, m_ptEnd			(CPoint(0, 0))
	, m_ptCorner		(CPoint(0, 0))
	, m_fOnePixelUnit	(1.f)
	, m_pCoordTransform(pCoordTransform)
{
	LOGFONT logfont;

	logfont.lfHeight			= 12;
	logfont.lfWidth				= 0;
	logfont.lfEscapement		= 0;
	logfont.lfOrientation		= 0;
	logfont.lfWeight			= FW_BOLD;
	logfont.lfItalic			= FALSE;
	logfont.lfUnderline			= FALSE;
	logfont.lfStrikeOut			= 0;
	logfont.lfCharSet			= ANSI_CHARSET;
	logfont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	logfont.lfQuality			= DEFAULT_QUALITY;
	logfont.lfPitchAndFamily	= DEFAULT_PITCH;

	// 2011 12 09 씨큐어 함수로 변경
	//_tcscpy(logfont.lfFaceName,_T("돋움체"));
	_tcscpy_s(logfont.lfFaceName, LF_FACESIZE, _T("System"));

	m_fntDistanceDisplayBig.CreateFontIndirect(&logfont);

	logfont.lfHeight = 10;
	logfont.lfWeight = FW_NORMAL;

	m_fntDistanceDisplaySmall.CreateFontIndirect(&logfont);
}

CPixelMeter::~CPixelMeter(void)
{
	m_fntDistanceDisplayBig.DeleteObject();
	m_fntDistanceDisplaySmall.DeleteObject();

	m_pCoordTransform = nullptr;
}

void CPixelMeter::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;

	if( !m_bEnable )
	{
		SetBegin(CPoint(0, 0));
	}
}

void CPixelMeter::SetUnit(float fOnePixelUnit, LPCTSTR lpszUnitName)
{
	m_fOnePixelUnit = fOnePixelUnit;
	m_strUnitName	= lpszUnitName;
}

void CPixelMeter::SetBegin(const CPoint &ptBegin)
{
	m_ptBegin = m_ptEnd = ptBegin;
}

CPoint CPixelMeter::__CalcCorner(CPoint a, CPoint b)
{
	if( a.x > b.x ) std::swap(a, b);

	float diag = float(b.y - a.y) / (float)(b.x - a.x);

	return diag > 0 ? CPoint(a.x, b.y) : CPoint(b.x, a.y);
}

void CPixelMeter::SetEnd(const CPoint &ptEnd)
{
	m_ptEnd 	= ptEnd;
	m_ptCorner 	= __CalcCorner(m_ptBegin, m_ptEnd);
}

void CPixelMeter::Draw(CDC *pdc)
{
	if( !IsEnable() ) return;
	if( m_ptBegin == m_ptEnd ) return;

	uDrawPixelTrace(pdc);
	uDrawPixelDistance(pdc);
}

void CPixelMeter::uDrawPixelTrace(CDC *pdc)
{
	CPoint A	= m_ptBegin;
	CPoint B	= m_ptEnd;
	CPoint C	= m_ptCorner;

	m_pCoordTransform->Transform(A);	// Image to Screen
	m_pCoordTransform->Transform(B);	// Image to Screen
	m_pCoordTransform->Transform(C);	// Image to Screen

	__DrawPixelTrace(pdc, A, C, RGB(192, 192, 192), FALSE);
	__DrawPixelTrace(pdc, B, C, RGB(192, 192, 192), FALSE);
	__DrawPixelTrace(pdc, A, B, RGB(255, 255, 255), TRUE);

	__DrawPixelMark(pdc, A, 4);
	__DrawPixelMark(pdc, B, 4);
}

void CPixelMeter::uDrawPixelDistance(CDC *pdc)
{
	CPoint a	= m_ptBegin;
	CPoint b	= m_ptEnd;
	CPoint c	= m_ptCorner;

	pdc->SetBkColor(RGB(0, 0, 0));

	pdc->SetTextColor(RGB(192, 192, 192));

	__DrawPixelDistance(pdc, a, c, FALSE);
	__DrawPixelDistance(pdc, b, c, FALSE);

	pdc->SetTextColor(RGB(255, 255, 255));

	__DrawPixelDistance(pdc, a, b, TRUE);

	pdc->SetTextColor(RGB(0, 0, 0));
	pdc->SetBkColor(RGB(255, 255, 255));

}

void CPixelMeter::__DrawPixelMark( CDC *pdc, CPoint point, int r)
{
	if( !pdc ) return;

	CPoint LT, RT, RB, LB;

	LT = CPoint(point.x - r, point.y - r);
	RT = CPoint(point.x + r, point.y - r);
	RB = CPoint(point.x + r, point.y + r);
	LB = CPoint(point.x - r, point.y + r);

	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(237, 28, 36));
	CPen *pOldPen = pdc->SelectObject(&pen);

	pdc->MoveTo(LT);
	pdc->LineTo(RB);
	pdc->MoveTo(RT);
	pdc->LineTo(LB);

	pdc->SelectObject(pOldPen);
}

void CPixelMeter::__DrawPixelTrace( CDC *pdc, CPoint pt1, CPoint pt2, COLORREF rgb, BOOL bBig)
{
	if( !pdc ) return;

	if( pt1 == pt2 ) return;

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, rgb);

	CPen *pOldPen = pdc->SelectObject(&pen);

	pdc->MoveTo(pt1);
	pdc->LineTo(pt2);

	pdc->SelectObject(pOldPen);
}

void CPixelMeter::__DrawPixelDistance( CDC *pdc, CPoint pt1, CPoint pt2, BOOL bBig)
{
	if( !pdc ) return;

	if( pt1 == pt2 ) return;

	int dx = abs(pt1.x - pt2.x);
	int dy = abs(pt1.y - pt2.y);

	float fDistance = sqrt((float)(dx * dx + dy * dy)) * m_fOnePixelUnit;

	CPoint center((pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2);

	CFont *pOldFont = pdc->SelectObject(bBig ? &m_fntDistanceDisplayBig : &m_fntDistanceDisplaySmall);

	CString strDistance;
	strDistance.Format(_T("%3.1f %s"), fDistance, m_strUnitName);

	m_pCoordTransform->Transform(center);

	pdc->TextOut(center.x, center.y, strDistance);

	pdc->SelectObject(pOldFont);
}
