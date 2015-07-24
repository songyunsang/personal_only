#pragma once
#include "ROIEditBase.h"
#include "ROIGeometryRectRounded.h"

enum
{
	KNOB_NORESIZE = -1,
	KNOB_RESIZE_ONE = 0,
	KNOB_RESIZE_ALL,
	KNOB_RESIZE_HORZ,
	KNOB_RESIZE_VERT,

};

class CROIEditRectRounded : public CROIEditBase
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

	inline void DrawKnobDiamond(CDC* pDC, const CPoint& pos) const
	{
		CPoint p[4];
		p[0] = CPoint(pos.x, pos.y - nKnobDia);
		p[1] = CPoint(pos.x + nKnobDia, pos.y);
		p[2] = CPoint(pos.x, pos.y + nKnobDia);
		p[3] = CPoint(pos.x - nKnobDia, pos.y);

		pDC->Polygon(p, 4);
	}

	inline void DrawOutline(CDC* pDC) const
	{
		CPen Pen(PS_SOLID, 1, RGB(255,255,64));	

		CPen* pOldPen = pDC->SelectObject(&Pen);

		pDC->MoveTo(p1LT);
		pDC->LineTo(L, T);
		pDC->LineTo(p2LT);

		pDC->MoveTo(p1RT);
		pDC->LineTo(R, T);
		pDC->LineTo(p2RT);

		pDC->MoveTo(p1RB);
		pDC->LineTo(R, B);
		pDC->LineTo(p2RB);

		pDC->MoveTo(p1LB);
		pDC->LineTo(L, B);
		pDC->LineTo(p2LB);
	
		pDC->SelectObject(pOldPen);

		Pen.DeleteObject();
	}

	inline void DrawKnobSymmetry(CDC* pDC) const
	{
		DrawKnobCircle(pDC, CPoint(L, T));
		DrawKnobCircle(pDC, CPoint(R, T));
		DrawKnobCircle(pDC, CPoint(R, B));
		DrawKnobCircle(pDC, CPoint(L, B));
	}

	inline void DrawKnobOrtho(CDC* pDC) const
	{
		DrawKnobRectangle(pDC, CPoint(L, (T + B) / 2));
		DrawKnobRectangle(pDC, CPoint((L + R) / 2, T));
		DrawKnobRectangle(pDC, CPoint(R, (T + B) / 2));
		DrawKnobRectangle(pDC, CPoint((L + R) / 2, B));
	}

	inline void DrawKnobEditRadius(CDC* pDC) const
	{
		switch(__nKnobResize)
		{
		case KNOB_NORESIZE:
			{
				CBrush	brush(RGB(255,255,0));
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				DrawKnobDiamond(pDC, p1LT);
				DrawKnobDiamond(pDC, p1RT);
				DrawKnobDiamond(pDC, p1RB);
				DrawKnobDiamond(pDC, p1LB);
				DrawKnobDiamond(pDC, p2LT);
				DrawKnobDiamond(pDC, p2RT);
				DrawKnobDiamond(pDC, p2RB);
				DrawKnobDiamond(pDC, p2LB);

				pDC->SelectObject(pOldBrush);
			}
			break;

		case KNOB_RESIZE_ONE:
			{
				CBrush	brush(RGB(255,255,0));
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				DrawKnobDiamond(pDC, p1LT);
				DrawKnobDiamond(pDC, p1RT);
				DrawKnobDiamond(pDC, p1RB);
				DrawKnobDiamond(pDC, p1LB);
				DrawKnobDiamond(pDC, p2LT);
				DrawKnobDiamond(pDC, p2RT);
				DrawKnobDiamond(pDC, p2RB);
				DrawKnobDiamond(pDC, p2LB);

				CBrush	brushSel(RGB(255, 20, 111));
				pDC->SelectObject(&brushSel);
				switch(__nKnob)
				{
				case KNOB_LTP1: DrawKnobDiamond(pDC, p1LT); break;
				case KNOB_RTP2:	DrawKnobDiamond(pDC, p2RT);	break;
				case KNOB_RBP1:	DrawKnobDiamond(pDC, p1RB);	break;
				case KNOB_LBP2:	DrawKnobDiamond(pDC, p2LB);	break;
				case KNOB_LTP2: DrawKnobDiamond(pDC, p2LT);	break;
				case KNOB_RTP1:	DrawKnobDiamond(pDC, p1RT);	break;
				case KNOB_RBP2:	DrawKnobDiamond(pDC, p2RB);	break;
				case KNOB_LBP1:	DrawKnobDiamond(pDC, p1LB);	break;
				}

				pDC->SelectObject(pOldBrush);
			}
			break;

		case KNOB_RESIZE_ALL:
			{
				CBrush	brush(RGB(255, 20, 111));
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				DrawKnobDiamond(pDC, p1LT);
				DrawKnobDiamond(pDC, p1RT);
				DrawKnobDiamond(pDC, p1RB);
				DrawKnobDiamond(pDC, p1LB);
				DrawKnobDiamond(pDC, p2LT);
				DrawKnobDiamond(pDC, p2RT);
				DrawKnobDiamond(pDC, p2RB);
				DrawKnobDiamond(pDC, p2LB);

				pDC->SelectObject(pOldBrush);
			}
			break;

		case KNOB_RESIZE_HORZ:
			{
				CBrush	brush(RGB(255,255,0));
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				DrawKnobDiamond(pDC, p1RT);
				DrawKnobDiamond(pDC, p1LB);
				DrawKnobDiamond(pDC, p2LT);
				DrawKnobDiamond(pDC, p2RB);

				CBrush	brushSel(RGB(255, 20, 111));
				pDC->SelectObject(&brushSel);
				DrawKnobDiamond(pDC, p1LT);
				DrawKnobDiamond(pDC, p1RB);
				DrawKnobDiamond(pDC, p2RT);
				DrawKnobDiamond(pDC, p2LB);

				pDC->SelectObject(pOldBrush);
			}
			break;

		case KNOB_RESIZE_VERT:
			{
				CBrush brush(RGB(255,255,0));
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				DrawKnobDiamond(pDC, p1LT);
				DrawKnobDiamond(pDC, p1RB);
				DrawKnobDiamond(pDC, p2RT);
				DrawKnobDiamond(pDC, p2LB);

				CBrush	brushSel(RGB(255, 20, 111));
				pDC->SelectObject(&brushSel);
				DrawKnobDiamond(pDC, p1RT);
				DrawKnobDiamond(pDC, p1LB);
				DrawKnobDiamond(pDC, p2LT);
				DrawKnobDiamond(pDC, p2RB);

				pDC->SelectObject(pOldBrush);
			}
			break;			
		}

	}
	
	BOOL ModifyTo(const CROIGeometryRectRounded& src, int nKnob, const CPoint& diff, CROIGeometryRectRounded& dst);
	
	long GetRadius(int nKnob) const;

private:
	CROIGeometryRectRounded* m_pROIGeometry;

private:
	CPoint __pos_img_begin;

	int __nKnob;
	int __nKnobResize;

private:
	int L, T, R, B;
	CRect LT, RT, RB, LB;

	CPoint p1LT, p2LT;
	CPoint p1RT, p2RT;
	CPoint p1RB, p2RB;
	CPoint p1LB, p2LB;

	int nKnobRect;
	int nKnobCircle;
	int nKnobDia;
	int nKnobFind;

public:
	CROIEditRectRounded(CROIGeometryBase* pROIGeometry, const CCoordinateTransformer* pCoordTransform);
	~CROIEditRectRounded(void);
};

