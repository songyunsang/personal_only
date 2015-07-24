#include "stdafx.h"
#include "ROIGeometryRect.h"


CROIGeometryRect::CROIGeometryRect(const CRect& rt)
	: CROIGeometryBase(RECTANGLE)
	, m_rt(rt)
{
}


CROIGeometryRect::~CROIGeometryRect(void)
{
}

BOOL CROIGeometryRect::PtInROI(const CPoint& pos) const
{
	return m_rt.PtInRect(pos);
}

BOOL CROIGeometryRect::ToExport(ROIData& o_data) const
{
	o_data.nType = GetType();

	o_data.rt = m_rt;

	return TRUE;
}