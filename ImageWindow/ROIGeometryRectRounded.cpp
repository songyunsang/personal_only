#include "stdafx.h"
#include "ROIGeometryRectRounded.h"

CROIGeometryRectRounded::CROIGeometryRectRounded()
	: CROIGeometryBase(RECTANGLEROUNDED)
	, m_rt(0, 0, 0, 0)
	, m_rxLT(0), m_ryLT(0)
	, m_rxRT(0), m_ryRT(0)
	, m_rxRB(0), m_ryRB(0)
	, m_rxLB(0), m_ryLB(0)

{
}
CROIGeometryRectRounded::CROIGeometryRectRounded(const CRect& rt, int rxLT, int ryLT, int rxRT, int ryRT, int rxRB, int ryRB, int rxLB, int ryLB)
	: CROIGeometryBase(RECTANGLEROUNDED)
	, m_rt(rt)
	, m_rxLT(rxLT), m_ryLT(ryLT)
	, m_rxRT(rxRT), m_ryRT(ryRT)
	, m_rxRB(rxRB), m_ryRB(ryRB)
	, m_rxLB(rxLB), m_ryLB(ryLB)
{

}

CROIGeometryRectRounded::CROIGeometryRectRounded(const ROIData& roi)
	: CROIGeometryBase(RECTANGLEROUNDED)
	, m_rt(roi.rt)
	, m_rxLT(roi.rdsLT.cx), m_ryLT(roi.rdsLT.cy)
	, m_rxRT(roi.rdsRT.cx), m_ryRT(roi.rdsRT.cy)
	, m_rxRB(roi.rdsRB.cx), m_ryRB(roi.rdsRB.cy)
	, m_rxLB(roi.rdsLB.cx), m_ryLB(roi.rdsLB.cy)
{

}


CROIGeometryRectRounded::~CROIGeometryRectRounded(void)
{
}

BOOL CROIGeometryRectRounded::PtInROI(const CPoint& pos) const
{
	return m_rt.PtInRect(pos);
}

BOOL CROIGeometryRectRounded::ToExport(ROIData& o_data) const
{
	o_data.nType = GetType();

	o_data.rt = m_rt;

	o_data.rdsLT = CSize(m_rxLT, m_ryLT);
	o_data.rdsRT = CSize(m_rxRT, m_ryRT);
	o_data.rdsRB = CSize(m_rxRB, m_ryRB);
	o_data.rdsLB = CSize(m_rxLB, m_ryLB);

	return TRUE;
}
