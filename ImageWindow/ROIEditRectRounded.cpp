#include "stdafx.h"
#include "ROIEditRectRounded.h"
#include "CoordinateTransformer.h"


CROIEditRectRounded::CROIEditRectRounded(CROIGeometryBase* pROIGeometry, const CCoordinateTransformer* pCoordTransform)
	: CROIEditBase(pCoordTransform)
	, m_pROIGeometry((CROIGeometryRectRounded*)pROIGeometry)
	, nKnobRect		(KNOBRECT)
	, nKnobCircle	(KNOBCIRCLE)
	, nKnobDia		(KNOBDIA)
	, nKnobFind		(KNOBFIND)
	, __nKnob		(KNOB_NOSELECT)
	, __nKnobResize	(KNOB_NORESIZE)
{
	ASSERT(pROIGeometry->GetType() == RECTANGLEROUNDED);
}


CROIEditRectRounded::~CROIEditRectRounded(void)
{
	m_pROIGeometry = nullptr;

	LoadCursor(NULL, IDC_ARROW);
}

void CROIEditRectRounded::Build()
{
	CRect rt(m_pROIGeometry->m_rt);
		
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

	LT = CRect(l, t, l + 2 * rxLT, t + 2 * ryLT);
	RT = CRect(r - 2 * rxRT, t, r, t + 2 * ryRT);
	RB = CRect(r - 2 * rxRB, b - 2 * ryRB, r, b);
	LB = CRect(l, b - 2 * ryLB, l + 2 * rxLB, b);

	p1LT = CPoint(LT.left + rxLT, LT.top);
	p2LT = CPoint(LT.left, LT.top + ryLT);

	p1RT = CPoint(RT.right, RT.top + ryRT);
	p2RT = CPoint(RT.right - rxRT, RT.top);

	p1RB = CPoint(RB.right - rxRB, RB.bottom);
	p2RB = CPoint(RB.right, RB.bottom - ryRB);

	p1LB = CPoint(LB.left, LB.bottom - ryLB);
	p2LB = CPoint(LB.left + rxLB, LB.bottom);

	L			= m_pCoordTransform->TransformX(l);
	T			= m_pCoordTransform->TransformY(t);
	R			= m_pCoordTransform->TransformX(r);
	B			= m_pCoordTransform->TransformY(b);

	LT.left		= m_pCoordTransform->TransformX(LT.left		);
	LT.top		= m_pCoordTransform->TransformY(LT.top		);
	LT.right	= m_pCoordTransform->TransformX(LT.right	);
	LT.bottom	= m_pCoordTransform->TransformY(LT.bottom	);

	RT.left		= m_pCoordTransform->TransformX(RT.left		);
	RT.top		= m_pCoordTransform->TransformY(RT.top		);
	RT.right	= m_pCoordTransform->TransformX(RT.right	);
	RT.bottom	= m_pCoordTransform->TransformY(RT.bottom	);

	RB.left		= m_pCoordTransform->TransformX(RB.left		);
	RB.top		= m_pCoordTransform->TransformY(RB.top		);
	RB.right	= m_pCoordTransform->TransformX(RB.right	);
	RB.bottom	= m_pCoordTransform->TransformY(RB.bottom	);

	LB.left		= m_pCoordTransform->TransformX(LB.left		);
	LB.top		= m_pCoordTransform->TransformY(LB.top		);
	LB.right	= m_pCoordTransform->TransformX(LB.right	);
	LB.bottom	= m_pCoordTransform->TransformY(LB.bottom	);

	m_pCoordTransform->Transform(p1LT);
	m_pCoordTransform->Transform(p1RT);
	m_pCoordTransform->Transform(p1RB);
	m_pCoordTransform->Transform(p1LB);

	m_pCoordTransform->Transform(p2LT);
	m_pCoordTransform->Transform(p2RT);
	m_pCoordTransform->Transform(p2RB);
	m_pCoordTransform->Transform(p2LB);
}

void CROIEditRectRounded::Draw(CDC* pDC) const
{
	DrawOutline(pDC);
	DrawKnobSymmetry(pDC);
	DrawKnobOrtho(pDC);
	DrawKnobEditRadius(pDC);
}

int	 CROIEditRectRounded::FindKnob(const CPoint& pos) const
{
	CRect rtPos(pos.x - nKnobFind, pos.y - nKnobFind, pos.x + nKnobFind, pos.y + nKnobFind);

	if( rtPos.PtInRect(p1LT) ) return KNOB_LTP1;
	if( rtPos.PtInRect(p1RT) ) return KNOB_RTP1;
	if( rtPos.PtInRect(p1RB) ) return KNOB_RBP1;
	if( rtPos.PtInRect(p1LB) ) return KNOB_LBP1;

	if( rtPos.PtInRect(p2LT) ) return KNOB_LTP2;
	if( rtPos.PtInRect(p2RT) ) return KNOB_RTP2;
	if( rtPos.PtInRect(p2RB) ) return KNOB_RBP2;
	if( rtPos.PtInRect(p2LB) ) return KNOB_LBP2;

	if( rtPos.PtInRect(CPoint(L, T)) ) return KNOB_LT;
	if( rtPos.PtInRect(CPoint(R, T)) ) return KNOB_RT;
	if( rtPos.PtInRect(CPoint(R, B)) ) return KNOB_RB;
	if( rtPos.PtInRect(CPoint(L, B)) ) return KNOB_LB;

	if( rtPos.PtInRect(CPoint(L, (T + B) / 2)) ) return KNOB_L;
	if( rtPos.PtInRect(CPoint((L + R) / 2, T)) ) return KNOB_T;
	if( rtPos.PtInRect(CPoint(R, (T + B) / 2)) ) return KNOB_R;
	if( rtPos.PtInRect(CPoint((L + R) / 2, B)) ) return KNOB_B;

	if( !(rtPos & CRect(L, T, R, B)).IsRectEmpty() ) return KNOB_MOVE;

	return KNOB_NOSELECT;
}

int CROIEditRectRounded::Edit(int nKnob, const CPoint& pos)
{
	return ModifyTo(*m_pROIGeometry, nKnob, pos, *m_pROIGeometry);
}

HCURSOR CROIEditRectRounded::UpdateCursor(int nKnob) const
{
	switch(nKnob)
	{
	case KNOB_NOSELECT:		return SetCursor(LoadCursor(NULL, IDC_ARROW));		break;
	case KNOB_MOVE:			return SetCursor(LoadCursor(NULL, IDC_SIZEALL));	break;
	case KNOB_LT:			return SetCursor(LoadCursor(NULL, IDC_SIZENWSE));	break;
	case KNOB_RT:			return SetCursor(LoadCursor(NULL, IDC_SIZENESW));	break;
	case KNOB_RB:			return SetCursor(LoadCursor(NULL, IDC_SIZENWSE));	break;
	case KNOB_LB:			return SetCursor(LoadCursor(NULL, IDC_SIZENESW));	break;
	case KNOB_L:			return SetCursor(LoadCursor(NULL, IDC_SIZEWE));		break;
	case KNOB_T:			return SetCursor(LoadCursor(NULL, IDC_SIZENS));		break;
	case KNOB_R:			return SetCursor(LoadCursor(NULL, IDC_SIZEWE));		break;
	case KNOB_B:			return SetCursor(LoadCursor(NULL, IDC_SIZENS));		break;
	case KNOB_LTP1:			return SetCursor(LoadCursor(NULL, IDC_SIZEWE));		break;
	case KNOB_RTP1:			return SetCursor(LoadCursor(NULL, IDC_SIZENS));		break;
	case KNOB_RBP1:			return SetCursor(LoadCursor(NULL, IDC_SIZEWE));		break;
	case KNOB_LBP1:			return SetCursor(LoadCursor(NULL, IDC_SIZENS));		break;
	case KNOB_LTP2:			return SetCursor(LoadCursor(NULL, IDC_SIZENS));		break;
	case KNOB_RTP2:			return SetCursor(LoadCursor(NULL, IDC_SIZEWE));		break;
	case KNOB_RBP2:			return SetCursor(LoadCursor(NULL, IDC_SIZENS));		break;
	case KNOB_LBP2:			return SetCursor(LoadCursor(NULL, IDC_SIZEWE));		break;

	default:				ASSERT(0); return NULL;								break;

	}
}

BOOL CROIEditRectRounded::OnEditBegin(UINT flags, const CPoint& pos_scr)
{
	Build();

	__nKnob = FindKnob(pos_scr);

	if( __nKnob == KNOB_NOSELECT ) return FALSE;

	__nKnobResize = KNOB_RESIZE_ONE;

	__pos_img_begin.x = m_pCoordTransform->ReverseTransformX(pos_scr.x);
	__pos_img_begin.y = m_pCoordTransform->ReverseTransformY(pos_scr.y);

	return CROIEditBase::OnEditBegin(flags, pos_scr);
}

BOOL CROIEditRectRounded::OnEditing(UINT flags, const CPoint& pos_scr)
{
	if( !IsEditing() ) return FALSE;

	CPoint pos_img;

	pos_img.x = m_pCoordTransform->ReverseTransformX(pos_scr.x);
	pos_img.y = m_pCoordTransform->ReverseTransformY(pos_scr.y);

	CPoint diff = pos_img - __pos_img_begin;

	ModifyTo(*m_pROIGeometry, __nKnob, diff, *m_pROIGeometry);

	if( GetKeyState(VK_CONTROL) & 0x80 )
	{
		long r = GetRadius(__nKnob);

		switch(__nKnob)
		{
		case KNOB_LTP1:	
		case KNOB_RTP2:
		case KNOB_RBP1:
		case KNOB_LBP2:
		case KNOB_LTP2:	
		case KNOB_RTP1:
		case KNOB_RBP2:
		case KNOB_LBP1:
			m_pROIGeometry->m_rxLT = r;
			m_pROIGeometry->m_rxRT = r;
			m_pROIGeometry->m_rxRB = r;
			m_pROIGeometry->m_rxLB = r;
			m_pROIGeometry->m_ryLT = r;
			m_pROIGeometry->m_ryRT = r;
			m_pROIGeometry->m_ryRB = r;
			m_pROIGeometry->m_ryLB = r;

			__nKnobResize = KNOB_RESIZE_ALL;
			break;
		}
	}	
	else if( GetKeyState(VK_SHIFT) & 0x80 )
	{
		long r = GetRadius(__nKnob);

		switch(__nKnob)
		{
		case KNOB_LTP1:	
		case KNOB_RTP2:
		case KNOB_RBP1:
		case KNOB_LBP2:
			m_pROIGeometry->m_rxLT = r;
			m_pROIGeometry->m_rxRT = r;
			m_pROIGeometry->m_rxRB = r;
			m_pROIGeometry->m_rxLB = r;

			__nKnobResize = KNOB_RESIZE_HORZ;
			break;

		case KNOB_LTP2:	
		case KNOB_RTP1:
		case KNOB_RBP2:
		case KNOB_LBP1:
			m_pROIGeometry->m_ryLT = r;
			m_pROIGeometry->m_ryRT = r;
			m_pROIGeometry->m_ryRB = r;
			m_pROIGeometry->m_ryLB = r;

			__nKnobResize = KNOB_RESIZE_VERT;
			break;
		}
	}
	else
	{
		__nKnobResize = KNOB_RESIZE_ONE;
	}


	__pos_img_begin = pos_img;

	return CROIEditBase::OnEditing(flags, pos_scr);
}

BOOL CROIEditRectRounded::OnEditEnd(UINT flags, const CPoint& pos_scr)
{
	__nKnob			= KNOB_NOSELECT;
	__nKnobResize	= KNOB_RESIZE_ONE;

	return CROIEditBase::OnEditEnd(flags, pos_scr);
}

BOOL CROIEditRectRounded::ModifyTo(const CROIGeometryRectRounded& src, int nKnob, const CPoint& diff, CROIGeometryRectRounded& dst)
{
	if( nKnob == KNOB_NOSELECT ) return FALSE;

	long l = src.m_rt.left;
	long t = src.m_rt.top;
	long r = src.m_rt.right;
	long b = src.m_rt.bottom;

	int rx = (r - l) / 2;
	int ry = (b - t) / 2;

	int rxLT = m_pROIGeometry->m_rxLT;
	int rxRT = m_pROIGeometry->m_rxRT;
	int rxRB = m_pROIGeometry->m_rxRB;
	int rxLB = m_pROIGeometry->m_rxLB;

	int ryLT = m_pROIGeometry->m_ryLT;
	int ryRT = m_pROIGeometry->m_ryRT;
	int ryRB = m_pROIGeometry->m_ryRB;
	int ryLB = m_pROIGeometry->m_ryLB;

	switch(nKnob)
	{
	case KNOB_MOVE:			l += diff.x, t += diff.y;
							r += diff.x, b += diff.y;	break;
	case KNOB_LT:			l += diff.x, t += diff.y;	break;
	case KNOB_RT:			r += diff.x, t += diff.y;	break;
	case KNOB_RB:			r += diff.x, b += diff.y;	break;
	case KNOB_LB:			l += diff.x, b += diff.y;	break;
	case KNOB_L:			l += diff.x;				break;
	case KNOB_T:			t += diff.y;				break;
	case KNOB_R:			r += diff.x;				break;
	case KNOB_B:			b += diff.y;				break;
	case KNOB_LTP1:			rxLT += diff.x;		rxLT = max(rxLT, 0); rxLT = min(rxLT, rx);break;
	case KNOB_RTP1:			ryRT += diff.y;		ryRT = max(ryRT, 0); ryRT = min(ryRT, ry);break;
	case KNOB_RBP1:			rxRB -= diff.x;		rxRB = max(rxRB, 0); rxRB = min(rxRB, rx);break;
	case KNOB_LBP1:			ryLB -= diff.y;		ryLB = max(ryLB, 0); ryLB = min(ryLB, ry);break;
	case KNOB_LTP2:			ryLT += diff.y;		ryLT = max(ryLT, 0); ryLT = min(ryLT, ry);break;
	case KNOB_RTP2:			rxRT -= diff.x;		rxRT = max(rxRT, 0); rxRT = min(rxRT, rx);break;
	case KNOB_RBP2:			ryRB -= diff.y;		ryRB = max(ryRB, 0); ryRB = min(ryRB, ry);break;
	case KNOB_LBP2:			rxLB += diff.x;		rxLB = max(rxLB, 0); rxLB = min(rxLB, rx);break;		
	}

	
	switch(nKnob)
	{
	case KNOB_MOVE:
	case KNOB_LT:	
	case KNOB_RT:	
	case KNOB_RB:	
	case KNOB_LB:	
	case KNOB_L:	
	case KNOB_T:	
	case KNOB_R:	
	case KNOB_B:	
		{
			const CRect& rtBound = GetBound();

			int maxRX = 0;

			maxRX = max(maxRX, rxLT);
			maxRX = max(maxRX, rxRT);
			maxRX = max(maxRX, rxRB);
			maxRX = max(maxRX, rxLB);
			maxRX = min(maxRX, rx);

			int maxRY = 0;

			maxRY = max(maxRY, ryLT);
			maxRY = max(maxRY, ryRT);
			maxRY = max(maxRY, ryRB);
			maxRY = max(maxRY, ryLB);
			maxRY = min(maxRY, ry);

			if( rtBound.IsRectEmpty() || l >= rtBound.left && r <= rtBound.right && l <= (r - 2 * maxRX))
			{
				dst.m_rt.left = l;
				dst.m_rt.right = r;
			}

			if( rtBound.IsRectEmpty() || t >= rtBound.top && b <= rtBound.bottom && t <= (b - 2 * maxRY))
			{
				dst.m_rt.top = t;
				dst.m_rt.bottom = b;
			}		
		}
		break;

	case KNOB_LTP1: dst.m_rxLT = rxLT; break;
	case KNOB_RTP1:	dst.m_ryRT = ryRT; break;
	case KNOB_RBP1:	dst.m_rxRB = rxRB; break;
	case KNOB_LBP1:	dst.m_ryLB = ryLB; break;
	case KNOB_LTP2:	dst.m_ryLT = ryLT; break;
	case KNOB_RTP2:	dst.m_rxRT = rxRT; break;
	case KNOB_RBP2:	dst.m_ryRB = ryRB; break;
	case KNOB_LBP2:	dst.m_rxLB = rxLB; break;
	}
	
	return TRUE;
}

long CROIEditRectRounded::GetRadius(int nKnob) const
{
	switch(nKnob)
	{
	case KNOB_LTP1:		return m_pROIGeometry->m_rxLT;
	case KNOB_RTP1:		return m_pROIGeometry->m_ryRT;
	case KNOB_RBP1:		return m_pROIGeometry->m_rxRB;
	case KNOB_LBP1:		return m_pROIGeometry->m_ryLB;
	case KNOB_LTP2:		return m_pROIGeometry->m_ryLT;
	case KNOB_RTP2:		return m_pROIGeometry->m_rxRT;
	case KNOB_RBP2:		return m_pROIGeometry->m_ryRB;
	case KNOB_LBP2:		return m_pROIGeometry->m_rxLB;
	}

	return 0;
}