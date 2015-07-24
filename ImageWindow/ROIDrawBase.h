#pragma once
#define DRAWFONTSIZE  (20)

struct ROIDrawPara
{
	long nLineWidth;
	COLORREF crColor;
	BOOL isMovable;
	BOOL isResizable;

	CString strName;
};

class CCoordinateTransformer;

class CROIDrawBase
{
public:
	CROIDrawBase(const ROIDrawPara& DrawPara, const CCoordinateTransformer* pCoordTransform);
	virtual ~CROIDrawBase(void);

public:
	virtual void Draw	(CDC* pDC) const = NULL;

public:	
	ROIDrawPara						m_DrawPara;
protected:
	const CCoordinateTransformer*	m_pCoordTransform;
};

