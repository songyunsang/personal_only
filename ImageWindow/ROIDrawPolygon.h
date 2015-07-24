#pragma once
#include "ROIDrawBase.h"
#include "ROIGeometryPolygon.h"

class CROIDrawPolygon : public CROIDrawBase
{
public:
	CROIDrawPolygon(const CROIGeometryBase* pROIGeometry, const ROIDrawPara& DrawPara, const CCoordinateTransformer* pCoordTransform);
	~CROIDrawPolygon(void);

public:
	virtual void Draw(CDC* pDC) const;

private:
	const CROIGeometryPolygon* m_pROIGeometry;
};

