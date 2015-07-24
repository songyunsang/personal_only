#include "stdafx.h"
#include "ROIDrawRectRounded.h"
#include "CoordinateTransformer.h"


CROIDrawRectRounded::CROIDrawRectRounded(const CROIGeometryBase* pROIGeometry, const ROIDrawPara& DrawPara, const CCoordinateTransformer* pCoordTransform)
	:CROIDrawBase(DrawPara, pCoordTransform)
	, m_pROIGeometry((const CROIGeometryRectRounded*)pROIGeometry)
{
	ASSERT(m_pROIGeometry->GetType() == RECTANGLEROUNDED);
}


CROIDrawRectRounded::~CROIDrawRectRounded(void)
{
	m_pROIGeometry = nullptr;
}

void CROIDrawRectRounded::Draw(CDC* pDC) const
{
	if( !pDC || !m_pROIGeometry || m_pROIGeometry->GetType() != RECTANGLEROUNDED ) return;

	CPen Pen(PS_SOLID, m_DrawPara.nLineWidth, m_DrawPara.crColor);	

	CPen* pOldPen = pDC->SelectObject(&Pen);

	CRect rt(m_pROIGeometry->m_rt);

	rt.NormalizeRect();

	int l = rt.left;
	int t = rt.top;
	int r = rt.right;
	int b = rt.bottom;

	int rxLT = m_pROIGeometry->m_rxLT;
	int rxRT = m_pROIGeometry->m_rxRT;
	int rxRB = m_pROIGeometry->m_rxRB;
	int rxLB = m_pROIGeometry->m_rxLB;

	int ryLT = m_pROIGeometry->m_ryLT;
	int ryRT = m_pROIGeometry->m_ryRT;
	int ryRB = m_pROIGeometry->m_ryRB;
	int ryLB = m_pROIGeometry->m_ryLB;

	//rx = (r - l) / 2;
	//ry = (b - t) / 2;

	CRect LT = CRect(l, t, l + 2 * rxLT, t + 2 * ryLT);
	CRect RT = CRect(r - 2 * rxRT, t, r, t + 2 * ryRT);
	CRect RB = CRect(r - 2 * rxRB, b - 2 * ryRB, r, b);
	CRect LB = CRect(l, b - 2 * ryLB, l + 2 * rxLB, b);

	CPoint p1LT = CPoint(LT.left + rxLT, LT.top);
	CPoint p2LT = CPoint(LT.left, LT.top + ryLT);
	CPoint p1RT = CPoint(RT.right, RT.top + ryRT);
	CPoint p2RT = CPoint(RT.right - rxRT, RT.top);
	CPoint p1RB = CPoint(RB.right - rxRB, RB.bottom);
	CPoint p2RB = CPoint(RB.right, RB.bottom - ryRB);
	CPoint p1LB = CPoint(LB.left, LB.bottom - ryLB);
	CPoint p2LB = CPoint(LB.left + rxLB, LB.bottom);

	std::vector<CRect>	vecCorner;
	std::vector<CPoint> vecMoveTo;
	std::vector<CPoint> vecArcTo;

	vecCorner.push_back(LT);
	vecCorner.push_back(LB);
	vecCorner.push_back(RB);
	vecCorner.push_back(RT);

	vecMoveTo.push_back(p1LT);
	vecMoveTo.push_back(p1LB);
	vecMoveTo.push_back(p1RB);
	vecMoveTo.push_back(p1RT);

	vecArcTo.push_back(p2LT);
	vecArcTo.push_back(p2LB);
	vecArcTo.push_back(p2RB);
	vecArcTo.push_back(p2RT);

	for(long i = 0 ; i < (long)vecCorner.size() ; ++i)
	{
		CRect& rtCorner = vecCorner[i];

		rtCorner.left	= m_pCoordTransform->TransformX(rtCorner.left	);
		rtCorner.top	= m_pCoordTransform->TransformY(rtCorner.top	);
		rtCorner.right	= m_pCoordTransform->TransformX(rtCorner.right	);
		rtCorner.bottom	= m_pCoordTransform->TransformY(rtCorner.bottom	);

		m_pCoordTransform->Transform(vecMoveTo[i]);
		m_pCoordTransform->Transform(vecArcTo[i]);
	}

	CPoint ptBegin = vecMoveTo[0];

	pDC->MoveTo(ptBegin);

	for(long i = 0 ; i < (long)vecArcTo.size() ; ++i)
	{
		if( vecCorner[i].IsRectEmpty() )
		{
			pDC->LineTo(vecMoveTo[i]);
			pDC->LineTo(vecArcTo[i]);
		}
		else
		{
			pDC->ArcTo(vecCorner[i], vecMoveTo[i], vecArcTo[i]);
		}
	}			

	pDC->LineTo(ptBegin);

	pDC->SetTextColor(m_DrawPara.crColor);

	pDC->TextOut(vecCorner[0].left, vecCorner[0].top - DRAWFONTSIZE, m_DrawPara.strName);

	pDC->SelectObject(pOldPen);
}
