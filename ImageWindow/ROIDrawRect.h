#pragma once
#include "ROIDrawBase.h"
#include "ROIGeometryRect.h"

class CROIDrawRect : public CROIDrawBase
{
public:
	CROIDrawRect(const CROIGeometryBase* pROIGeometry, const ROIDrawPara& DrawPara, const CCoordinateTransformer* pCoordTransform);
	~CROIDrawRect(void);

public:
	virtual void Draw	(CDC* pDC) const;

private:
	const CROIGeometryRect* m_pROIGeometry;
};

