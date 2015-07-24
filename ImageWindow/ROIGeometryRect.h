#pragma once
#include "ROIGeometryBase.h"

class CROIGeometryRect : public CROIGeometryBase
{
public:
	virtual BOOL PtInROI(const CPoint& pos) const;
	virtual BOOL ToExport(ROIData& o_data) const;
public:
	CRect m_rt;

public:
	CROIGeometryRect(const CRect& rt = CRect(0, 0, 0, 0));
	~CROIGeometryRect(void);
};

