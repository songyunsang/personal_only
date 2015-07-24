#pragma once

class CROIGeometryBase
{
public:
	int  GetType() const	{ return m_nType; }

public:
	virtual BOOL PtInROI(const CPoint& pos) const = NULL;
	virtual BOOL ToExport(ROIData& o_data) const = NULL;
	
private:
	int m_nType;

public:
	CROIGeometryBase(int nType = -1);
	virtual ~CROIGeometryBase(void);
};

