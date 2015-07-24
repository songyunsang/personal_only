#pragma once

class CROIGeometryBase;
class CROIEditBase;
class CCoordinateTransformer;

class CROIEditFactory
{
public:
	static CROIEditBase* Create(CROIGeometryBase* pROIGeometry, const CCoordinateTransformer* pCoordTransform);
public:
	CROIEditFactory(void);
	~CROIEditFactory(void);
};

