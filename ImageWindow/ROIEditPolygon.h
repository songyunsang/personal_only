#pragma once
#include "ROIEditBase.h"
#include "ROIGeometryPolygon.h"

class CROIEditPolygon : public CROIEditBase
{
public:
	virtual void	Build()								;
	virtual void	Draw(CDC* pDC) const				;
	virtual int		FindKnob(const CPoint& pos) const	;
	virtual int		Edit(int nKnob, const CPoint& pos)	;
	virtual HCURSOR	UpdateCursor(int nKnob) const		;

	virtual BOOL	OnEditBegin	(UINT flags, const CPoint& pos_scr);
	virtual BOOL	OnEditing	(UINT flags, const CPoint& pos_scr);
	virtual BOOL	OnEditEnd	(UINT flags, const CPoint& pos_scr);

private:
	inline void DrawKnobCircle(CDC* pDC, const CPoint& pos) const
	{
		pDC->Ellipse(pos.x - nKnobCircle, pos.y - nKnobCircle, pos.x + nKnobCircle, pos.y + nKnobCircle);
	}

	BOOL ModifyTo(const CROIGeometryPolygon& src, int nKnob, const CPoint& diff, CROIGeometryPolygon& dst);

private:
	CROIGeometryPolygon* m_pROIGeometry;

private:
	vector<CPoint> m_vecPointScreen;

private:
	CPoint __pos_img_begin;

	int __nSelKnob;

private:
	int nKnobCircle;
	int nKnobFind;

public:
	CROIEditPolygon(CROIGeometryBase* pROIGeometry, const CCoordinateTransformer* pCoordTransform);
	~CROIEditPolygon(void);
};

