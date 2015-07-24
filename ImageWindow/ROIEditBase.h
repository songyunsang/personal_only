#pragma once

#define KNOBRECT	(4)
#define KNOBCIRCLE	(5)
#define KNOBDIA		(5)
#define KNOBFIND	(7)

enum 
{
	KNOB_MOVE = -2,
	KNOB_NOSELECT = -1,
	KNOB_LT,
	KNOB_RT,
	KNOB_RB,
	KNOB_LB,
	KNOB_L,
	KNOB_T,
	KNOB_R,
	KNOB_B,
	KNOB_LTP1,
	KNOB_RTP1,
	KNOB_RBP1,
	KNOB_LBP1,
	KNOB_LTP2,
	KNOB_RTP2,
	KNOB_RBP2,
	KNOB_LBP2,
};

class CCoordinateTransformer;

class CROIEditBase
{
public:
	virtual void	Draw(CDC* pDC) const				= NULL;
	virtual int		FindKnob(const CPoint& pos) const	= NULL;
	virtual int		Edit(int nKnob, const CPoint& pos)	= NULL;
	virtual void	Build()								= NULL;
	virtual HCURSOR	UpdateCursor(int nKnob)	const		= NULL;

	virtual BOOL	OnEditBegin	(UINT flags, const CPoint& pos_scr){ m_bEditing = TRUE; return TRUE; }
	virtual BOOL	OnEditing	(UINT flags, const CPoint& pos_scr){ return TRUE; }
	virtual BOOL	OnEditEnd	(UINT flags, const CPoint& pos_scr){ m_bEditing = FALSE;return TRUE; }

public:
	void SetBound(const CRect& rtBound) {m_rtBound = rtBound;}
	const CRect& GetBound() const		{return m_rtBound; }

protected:
	BOOL IsEditing() const	{ return m_bEditing; }

protected:
	const CCoordinateTransformer*	m_pCoordTransform;
		
private:
	BOOL	m_bEditing;
	CRect	m_rtBound;

public:
	CROIEditBase(const CCoordinateTransformer* pCoordTransform);
	virtual ~CROIEditBase(void);
};

