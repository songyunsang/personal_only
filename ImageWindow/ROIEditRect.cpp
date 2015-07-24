#include "stdafx.h"
#include "ROIEditRect.h"
#include "CoordinateTransformer.h"


CROIEditRect::CROIEditRect(CROIGeometryBase* pROIGeometry, const CCoordinateTransformer* pCoordTransform)
	: CROIEditBase(pCoordTransform)
	, m_pROIGeometry((CROIGeometryRect*)pROIGeometry)
	, nKnobRect		(KNOBRECT)
	, nKnobCircle	(KNOBCIRCLE)
	, nKnobFind		(KNOBFIND)
{
	ASSERT(pROIGeometry->GetType() == RECTANGLE);
}


CROIEditRect::~CROIEditRect(void)
{
	m_pROIGeometry = nullptr;

	LoadCursor(NULL, IDC_ARROW);
}

void CROIEditRect::Build()
{
	CRect rt(m_pROIGeometry->m_rt);
		
	scrL = m_pCoordTransform->TransformX(rt.left);
	scrT = m_pCoordTransform->TransformY(rt.top);
	scrR = m_pCoordTransform->TransformX(rt.right);
	scrB = m_pCoordTransform->TransformY(rt.bottom);
}

void CROIEditRect::Draw(CDC* pDC) const
{
	DrawKnobSymmetry(pDC);
	DrawKnobOrtho(pDC);
}

int	 CROIEditRect::FindKnob(const CPoint& pos) const
{
	CRect rtPos(pos.x - nKnobFind, pos.y - nKnobFind, pos.x + nKnobFind, pos.y + nKnobFind);

	if( rtPos.PtInRect(CPoint(scrL, scrT)) ) return KNOB_LT;
	if( rtPos.PtInRect(CPoint(scrR, scrT)) ) return KNOB_RT;
	if( rtPos.PtInRect(CPoint(scrR, scrB)) ) return KNOB_RB;
	if( rtPos.PtInRect(CPoint(scrL, scrB)) ) return KNOB_LB;

	if( rtPos.PtInRect(CPoint(scrL, (scrT + scrB) / 2)) ) return KNOB_L;
	if( rtPos.PtInRect(CPoint((scrL + scrR) / 2, scrT)) ) return KNOB_T;
	if( rtPos.PtInRect(CPoint(scrR, (scrT + scrB) / 2)) ) return KNOB_R;
	if( rtPos.PtInRect(CPoint((scrL + scrR) / 2, scrB)) ) return KNOB_B;

	if( !(rtPos & CRect(scrL, scrT, scrR, scrB)).IsRectEmpty() ) return KNOB_MOVE;

	return KNOB_NOSELECT;
}

int CROIEditRect::Edit(int nKnob, const CPoint& pos)
{
	return ModifyTo(*m_pROIGeometry, nKnob, pos, *m_pROIGeometry);
}

HCURSOR CROIEditRect::UpdateCursor(int nKnob) const
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

	default:				ASSERT(0); return NULL;								break;

	}
}

BOOL CROIEditRect::OnEditBegin(UINT flags, const CPoint& pos_scr)
{
	Build();

	__nSelKnob = FindKnob(pos_scr);

	if( __nSelKnob == KNOB_NOSELECT ) return FALSE;

	__pos_img_begin.x = m_pCoordTransform->ReverseTransformX(pos_scr.x);
	__pos_img_begin.y = m_pCoordTransform->ReverseTransformY(pos_scr.y);

	return CROIEditBase::OnEditBegin(flags, pos_scr);
}

BOOL CROIEditRect::OnEditing(UINT flags, const CPoint& pos_scr)
{
	if( !IsEditing() ) return FALSE;

	CPoint pos_img;

	pos_img.x = m_pCoordTransform->ReverseTransformX(pos_scr.x);
	pos_img.y = m_pCoordTransform->ReverseTransformY(pos_scr.y);

	CPoint diff = pos_img - __pos_img_begin;

	ModifyTo(*m_pROIGeometry, __nSelKnob, diff, *m_pROIGeometry);

	__pos_img_begin = pos_img;

	return CROIEditBase::OnEditing(flags, pos_scr);
}

BOOL CROIEditRect::OnEditEnd(UINT flags, const CPoint& pos_scr)
{
	if( !IsEditing() ) return FALSE;

	__nSelKnob = KNOB_NOSELECT;

	return CROIEditBase::OnEditEnd(flags, pos_scr);
}

BOOL CROIEditRect::ModifyTo(const CROIGeometryRect& src, int nKnob, const CPoint& diff, CROIGeometryRect& dst)
{
	if( nKnob == KNOB_NOSELECT ) return FALSE;

	long l = src.m_rt.left;
	long t = src.m_rt.top;
	long r = src.m_rt.right;
	long b = src.m_rt.bottom;

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
	}

	CRect rtBound(GetBound());

	if( rtBound.IsRectEmpty() || l >= rtBound.left && r <= rtBound.right && l < (r - 2 * nKnobCircle))
	{
		dst.m_rt.left	= l;
		dst.m_rt.right	= r;
	}

	if( rtBound.IsRectEmpty() || t >= rtBound.top && b <= rtBound.bottom && t < (b - 2 * nKnobCircle))
	{
		dst.m_rt.top	= t;
		dst.m_rt.bottom	= b;
	}

	return TRUE;
}
