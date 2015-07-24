#include "stdafx.h"
#include "ROIEditFactory.h"
#include "ROIEditRect.h"
#include "ROIEditRectRounded.h"
#include "ROIEditPolygon.h"

CROIEditFactory::CROIEditFactory(void)
{
}


CROIEditFactory::~CROIEditFactory(void)
{
}

CROIEditBase* CROIEditFactory::Create(CROIGeometryBase* pROIGeometry, const CCoordinateTransformer* pCoordTransform)
{
	switch(pROIGeometry->GetType())
	{
	case RECTANGLE:
		return new CROIEditRect			(pROIGeometry, pCoordTransform);

	case RECTANGLEROUNDED:
		return new CROIEditRectRounded	(pROIGeometry, pCoordTransform);

	case TRIANGLE:
		return new CROIEditPolygon		(pROIGeometry, pCoordTransform);
	}

	return nullptr;
}