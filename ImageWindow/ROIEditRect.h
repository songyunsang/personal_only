#pragma once
#include "ROIEditBase.h"
#include "ROIGeometryRect.h"

class CROIEditRect : public CROIEditBase
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

	inline void DrawKnobRectangle(CDC* pDC, const CPoint& pos) const
	{
		pDC->Rectangle(pos.x - nKnobRect, pos.y - nKnobRect, pos.x + nKnobRect, pos.y + nKnobRect);
	}

	inline void DrawKnobSymmetry(CDC* pDC) const
	{
		DrawKnobCircle(pDC, CPoint(scrL, scrT));
		DrawKnobCircle(pDC, CPoint(scrR, scrT));
		DrawKnobCircle(pDC, CPoint(scrR, scrB));
		DrawKnobCircle(pDC, CPoint(scrL, scrB));
	}

	inline void DrawKnobOrtho(CDC* pDC) const
	{
		DrawKnobRectangle(pDC, CPoint(scrL, (scrT + scrB) / 2));
		DrawKnobRectangle(pDC, CPoint((scrL + scrR) / 2, scrT));
		DrawKnobRectangle(pDC, CPoint(scrR, (scrT + scrB) / 2));
		DrawKnobRectangle(pDC, CPoint((scrL + scrR) / 2, scrB));
	}

	BOOL ModifyTo(const CROIGeometryRect& src, int nKnob, const CPoint& diff, CROIGeometryRect& dst);
	
private:
	CROIGeometryRect* m_pROIGeometry;

private:
	int scrL, scrT, scrR, scrB;

private:
	CPoint __pos_img_begin;

	int __nSelKnob;

private:
	int nKnobRect;
	int nKnobCircle;
	int nKnobFind;

public:
	CROIEditRect(CROIGeometryBase* pROIGeometry, const CCoordinateTransformer* pCoordTransform);
	~CROIEditRect(void);
};

