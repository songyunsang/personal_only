#include "StdAfx.h"
#include "MouseTrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMouseTrace::CMouseTrace(void)
: m_argb(Color(255,255,255,255))
, m_nPenWidth(1)
{
}

CMouseTrace::~CMouseTrace(void)
{
}

void CMouseTrace::Init(Color argb, long nPenWidth)
{
	Lock();
	m_PointArray.clear();

	m_argb		= argb;
	m_nPenWidth = nPenWidth;

	Unlock();
}

void CMouseTrace::WayPoint(long nPosX, long nPosY)
{
	WayPoint(Point(nPosX, nPosY));
}

void CMouseTrace::WayPoint(const Point& point)
{
	Lock();

	if( m_PointArray.empty() || !m_PointArray.back().Equals(point) ) m_PointArray.push_back(point);
	
	Unlock();
}

CRect CMouseTrace::Boundary()
{
	Lock();

	CRect rtBound(999999,999999,-999999,-999999);

	for(long i = 0 ; i < (long)m_PointArray.size() ; ++i )
	{
		const Point& pt = m_PointArray[i];

		rtBound.left	= min(rtBound.left,		pt.X);
		rtBound.top		= min(rtBound.top,		pt.Y);
		rtBound.right	= max(rtBound.right,	pt.X);
		rtBound.bottom	= max(rtBound.bottom,	pt.Y);
	}

	Unlock();

	const long nInflate = m_nPenWidth / 2 + 1;
	rtBound.InflateRect(nInflate, nInflate, nInflate, nInflate);

	return rtBound;
}
