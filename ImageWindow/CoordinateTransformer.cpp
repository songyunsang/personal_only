#include "stdafx.h"
#include "CoordinateTransformer.h"

CCoordinateTransformer::CCoordinateTransformer(void)
{
	m_ptOffsetPre = CPoint(0, 0);
	m_ptOffsetPost = CPoint(0, 0);

	m_fScaleX = 1;
	m_fScaleY = 1;
}

CCoordinateTransformer::~CCoordinateTransformer(void)
{
}

void CCoordinateTransformer::SetOffsetPre(const CPoint& offset)
{
	m_ptOffsetPre = offset;
}

void CCoordinateTransformer::SetOffsetPost(const CPoint& offset)
{
	m_ptOffsetPost = offset;
}
void CCoordinateTransformer::SetScale(double fScaleX, double fScaleY)
{
	m_fScaleX = fScaleX;
	m_fScaleY = fScaleY;
}
