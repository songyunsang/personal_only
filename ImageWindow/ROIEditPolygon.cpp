#include "stdafx.h"
#include "ROIEditPolygon.h"
#include "CoordinateTransformer.h"


CROIEditPolygon::CROIEditPolygon(CROIGeometryBase* pROIGeometry, const CCoordinateTransformer* pCoordTransform)
	: CROIEditBase(pCoordTransform)
	, m_pROIGeometry((CROIGeometryPolygon*)pROIGeometry)
	, nKnobCircle	(KNOBCIRCLE)
	, nKnobFind		(KNOBFIND)
{
	ASSERT(pROIGeometry->GetType() == TRIANGLE);
}


CROIEditPolygon::~CROIEditPolygon(void)
{
	m_pROIGeometry = nullptr;

	LoadCursor(NULL, IDC_ARROW);
}

void CROIEditPolygon::Build()
{
	const vector<CPoint>& vecPoint = m_pROIGeometry->m_vecPoint;

	m_vecPointScreen.resize(vecPoint.size());

	for(long i = 0 ; i < (long)vecPoint.size() ; ++i)
	{
		m_vecPointScreen[i].x = m_pCoordTransform->TransformX(vecPoint[i].x);
		m_vecPointScreen[i].y = m_pCoordTransform->TransformY(vecPoint[i].y);
	}
}

void CROIEditPolygon::Draw(CDC* pDC) const
{
	for(long i = 0 ; i < (long)m_vecPointScreen.size() ; ++i)
	{
		DrawKnobCircle(pDC, m_vecPointScreen[i]);
	}
}

int	 CROIEditPolygon::FindKnob(const CPoint& pos) const
{
	CRect rtPos(pos.x - nKnobFind, pos.y - nKnobFind, pos.x + nKnobFind, pos.y + nKnobFind);

	for(long i = (long)m_vecPointScreen.size() - 1 ; i >= 0 ; --i)
	{
		if( rtPos.PtInRect(m_vecPointScreen[i]) ) return i;
	}

	CPoint pos_on_image;

	pos_on_image.x = m_pCoordTransform->ReverseTransformX(pos.x);
	pos_on_image.y = m_pCoordTransform->ReverseTransformY(pos.y);

	if( m_pROIGeometry->PtInROI(pos_on_image) )
	{
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			return KNOB_MOVE;
	}

	return KNOB_NOSELECT;
}

int CROIEditPolygon::Edit(int nKnob, const CPoint& pos)
{
	if( nKnob == KNOB_NOSELECT ) return -1;

	vector<CPoint>& vecPoint = m_pROIGeometry->m_vecPoint;

	if( nKnob == KNOB_MOVE )
	{
		for(long i = 0 ; i < (long)vecPoint.size() ; ++i)
		{
			vecPoint[i] += pos;
		}
	}
	else
	{
		if( OutOfRange(vecPoint, nKnob) ) return -1;

		vecPoint[nKnob] += pos;
	}

	return 0;
}

HCURSOR CROIEditPolygon::UpdateCursor(int nKnob) const
{
	switch(nKnob)
	{
	case KNOB_NOSELECT:
		return SetCursor(LoadCursor(NULL, IDC_ARROW));

	case KNOB_MOVE:
		return SetCursor(LoadCursor(NULL, IDC_SIZEALL));
	}

	return SetCursor(LoadCursor(NULL, IDC_SIZENESW));
}

BOOL CROIEditPolygon::OnEditBegin(UINT flags, const CPoint& pos_scr)
{
	Build();

	__nSelKnob = FindKnob(pos_scr);

	if( __nSelKnob == KNOB_NOSELECT ) return FALSE;

	__pos_img_begin.x = m_pCoordTransform->ReverseTransformX(pos_scr.x);
	__pos_img_begin.y = m_pCoordTransform->ReverseTransformY(pos_scr.y);

	return CROIEditBase::OnEditBegin(flags, pos_scr);
}

BOOL CROIEditPolygon::OnEditing(UINT flags, const CPoint& pos_scr)
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

BOOL CROIEditPolygon::OnEditEnd(UINT flags, const CPoint& pos_scr)
{
	if( !IsEditing() ) return FALSE;

	__nSelKnob = KNOB_NOSELECT;

	return CROIEditBase::OnEditEnd(flags, pos_scr);
}

BOOL CROIEditPolygon::ModifyTo(const CROIGeometryPolygon& src, int nKnob, const CPoint& diff, CROIGeometryPolygon& dst)
{
	if( nKnob == KNOB_NOSELECT ) return FALSE;

	vector<CPoint>& vecPoint = m_pROIGeometry->m_vecPoint;

	if( nKnob == KNOB_MOVE )
	{
		for(long i = 0 ; i < (long)vecPoint.size() ; ++i)
		{
			vecPoint[i] += diff;
		}
	}
	else
	{
		if( OutOfRange(vecPoint, nKnob) ) return FALSE;

		vecPoint[nKnob] += diff;
	}

	return TRUE;
}


