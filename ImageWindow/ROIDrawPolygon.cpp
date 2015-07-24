#include "stdafx.h"
#include "ROIDrawPolygon.h"
#include "CoordinateTransformer.h"

CROIDrawPolygon::CROIDrawPolygon(const CROIGeometryBase* pROIGeometry, const ROIDrawPara& DrawPara, const CCoordinateTransformer* pCoordTransform)
	: CROIDrawBase(DrawPara, pCoordTransform)
	, m_pROIGeometry((const CROIGeometryPolygon*)pROIGeometry)
{
	ASSERT(m_pROIGeometry->GetType() == TRIANGLE);
}


CROIDrawPolygon::~CROIDrawPolygon(void)
{
}

void CROIDrawPolygon::Draw(CDC* pDC) const
{
	if( !pDC || !m_pROIGeometry || m_pROIGeometry->GetType() != TRIANGLE ) return;

	CPen Pen(PS_SOLID, m_DrawPara.nLineWidth, m_DrawPara.crColor);	

	CPen* pOldPen = pDC->SelectObject(&Pen);

	const vector<CPoint>& vecPoint = m_pROIGeometry->m_vecPoint;

	if(vecPoint.empty()) return;

	vector<CPoint> vecPointScreen(vecPoint.size());

	for(long i = 0 ; i < (long)vecPointScreen.size() ; ++i)
	{
		vecPointScreen[i].x = m_pCoordTransform->TransformX(vecPoint[i].x);
		vecPointScreen[i].y = m_pCoordTransform->TransformY(vecPoint[i].y);
	}

	pDC->MoveTo(vecPointScreen[0]);
	pDC->Polygon(&vecPointScreen[0], (int)vecPointScreen.size());
	
	pDC->SetTextColor(m_DrawPara.crColor);

	CRect rt = POLYGON2RECT(vecPointScreen);

	pDC->TextOut(rt.left, rt.top - DRAWFONTSIZE, m_DrawPara.strName);

	pDC->SelectObject(pOldPen);

}
