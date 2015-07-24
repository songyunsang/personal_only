#pragma once
#include "ROIDrawBase.h"
#include "ROIGeometryRectRounded.h"

class CROIDrawRectRounded : public CROIDrawBase
{
public:
	CROIDrawRectRounded(const CROIGeometryBase* pROIGeometry, const ROIDrawPara& DrawPara, const CCoordinateTransformer* pCoordTransform);
	~CROIDrawRectRounded(void);

public:
	virtual void Draw	(CDC* pDC) const;

private:
	const CROIGeometryRectRounded* m_pROIGeometry;
};

