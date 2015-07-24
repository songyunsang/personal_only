#include "stdafx.h"
#include "ROIDrawBase.h"

CROIDrawBase::CROIDrawBase(const ROIDrawPara& DrawPara, const CCoordinateTransformer* pCoordTransform)
	: m_DrawPara(DrawPara)
	, m_pCoordTransform(pCoordTransform)
{
}


CROIDrawBase::~CROIDrawBase(void)
{
	m_pCoordTransform = NULL;
}
