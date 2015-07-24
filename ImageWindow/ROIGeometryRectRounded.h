#pragma once
#include "ROIGeometryBase.h"

class CROIGeometryRectRounded : public CROIGeometryBase
{
public:
	virtual BOOL PtInROI(const CPoint& pos) const;
	virtual BOOL ToExport(ROIData& o_data) const;

public:
	CRect m_rt;

	int m_rxLT, m_ryLT;
	int m_rxRT, m_ryRT;
	int m_rxRB, m_ryRB;
	int m_rxLB, m_ryLB;

public:
	CROIGeometryRectRounded();
	CROIGeometryRectRounded(const CRect& rt, int rxLT, int ryLT, int rxRT, int ryRT, int rxRB, int ryRB, int rxLB, int ryLB);
	CROIGeometryRectRounded(const ROIData& roi);
	~CROIGeometryRectRounded(void);
};

