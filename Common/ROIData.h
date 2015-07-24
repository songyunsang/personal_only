#pragma once
#include <vector>

enum
{
	RECTANGLE			= 0,
	RECTANGLEROUNDED	= 1,
	TRIANGLE			= 3,
};

struct ROIData
{
	long nType;		// 0 : Rect, 1: RoundRect 3: Triangle

	CRect rt;

	CSize rdsLT;
	CSize rdsRT;
	CSize rdsRB;
	CSize rdsLB;
	std::vector<CPoint> vecPoint;
};

inline CRect POLYGON2RECT(const std::vector<CPoint>& vecPoint)
{
	int l = INT_MAX;
	int t = INT_MAX;
	int r = INT_MIN;
	int b = INT_MIN;

	for(long i = 0 ; i < (long)vecPoint.size() ; ++i )
	{
		l = min(l, vecPoint[i].x);
		t = min(t, vecPoint[i].y);
		r = max(r, vecPoint[i].x);
		b = max(b, vecPoint[i].y);
	}

	return CRect(l, t, r, b);
}

inline ROIData RECT2ROI(const CRect& rt)
{
	ROIData ret;

	ret.nType = RECTANGLE;

	ret.rt = rt;

	return ret;
}

inline ROIData RECTROUNDED2ROI(const CRect& rt)
{
	ROIData ret;

	ret.nType = RECTANGLEROUNDED;

	ret.rt = rt;

	int w = rt.Width();
	int h = rt.Height();

	int rx = w / 4;
	int ry = h / 4;
	
	ret.rdsLT = CSize(rx, ry);
	ret.rdsRT = CSize(rx, ry);
	ret.rdsRB = CSize(rx, ry);
	ret.rdsLB = CSize(rx, ry);
}

inline ROIData POLYGON2ROI(const std::vector<CPoint>& vecPoint)
{
	ROIData ret;

	ret.nType		= TRIANGLE;
	ret.vecPoint	= vecPoint;
	ret.rt			= POLYGON2RECT(vecPoint);

	return ret;
}
