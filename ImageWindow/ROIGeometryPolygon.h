#pragma once
#include <vector>
#include "ROIGeometryBase.h"

class CROIGeometryPolygon : public CROIGeometryBase
{
public:
	virtual BOOL PtInROI(const CPoint& pos) const;
	virtual BOOL ToExport(ROIData& o_data) const;
public:
	std::vector<CPoint> m_vecPoint;

public:
	CROIGeometryPolygon(const vector<CPoint>& vecPoint = vector<CPoint>());
	~CROIGeometryPolygon(void);
};

