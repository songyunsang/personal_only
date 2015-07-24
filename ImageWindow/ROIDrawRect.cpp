#include "stdafx.h"
#include "ROIDrawRect.h"
#include "CoordinateTransformer.h"

CROIDrawRect::CROIDrawRect(const CROIGeometryBase* pROIGeometry, const ROIDrawPara& DrawPara, const CCoordinateTransformer* pCoordTransform)
	:CROIDrawBase(DrawPara, pCoordTransform)
	, m_pROIGeometry((const CROIGeometryRect*)pROIGeometry)
{
	ASSERT(m_pROIGeometry->GetType() == RECTANGLE);
}


CROIDrawRect::~CROIDrawRect(void)
{
	m_pROIGeometry = nullptr;
}

void CROIDrawRect::Draw(CDC* pDC) const
{
	if( !pDC || !m_pROIGeometry || m_pROIGeometry->GetType() != RECTANGLE ) return;

	CPen Pen(PS_SOLID, m_DrawPara.nLineWidth, m_DrawPara.crColor);	

	CPen* pOldPen = pDC->SelectObject(&Pen);

	CRect rt = m_pROIGeometry->m_rt;
	
	CPoint lt(rt.left,	rt.top);
	CPoint rb(rt.right, rt.bottom);

	m_pCoordTransform->Transform(lt);
	m_pCoordTransform->Transform(rb);

	pDC->Rectangle(lt.x, lt.y, rb.x, rb.y);
	
	pDC->SetTextColor(m_DrawPara.crColor);

	pDC->TextOut(lt.x, lt.y - DRAWFONTSIZE, m_DrawPara.strName);

	pDC->SelectObject(pOldPen);
}