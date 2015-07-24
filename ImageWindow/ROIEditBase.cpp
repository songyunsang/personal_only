#include "stdafx.h"
#include "ROIEditBase.h"


CROIEditBase::CROIEditBase(const CCoordinateTransformer* pCoordTransform)
	: m_pCoordTransform(pCoordTransform)
	, m_bEditing(FALSE)
	, m_rtBound(0,0,0,0)
{
}


CROIEditBase::~CROIEditBase(void)
{
	m_pCoordTransform = nullptr;
}
