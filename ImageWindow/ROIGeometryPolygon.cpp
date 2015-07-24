#include "stdafx.h"
#include "ROIGeometryPolygon.h"


CROIGeometryPolygon::CROIGeometryPolygon(const vector<CPoint>& vecPoint)
	: CROIGeometryBase(TRIANGLE)
	, m_vecPoint(vecPoint)
{
}


CROIGeometryPolygon::~CROIGeometryPolygon(void)
{
}

BOOL CROIGeometryPolygon::PtInROI(const CPoint& pos) const
{
	if( m_vecPoint.size() < 3 ) return false;

	const CPoint&	P = pos;
	const CPoint*	V = &m_vecPoint[0];
	long			n = (long)m_vecPoint.size();

    int followIndex = n - 1;
    bool isOddNodes = false;
 
    /**
        * 아래 알고리즘은 "Point in Polygon" 알고리즘이다.
        * 다만 좌우 양 방향을 체크하는 것이 아니라 왼쪽 방향만을 체크한다.
        */
    for (int frontIndex = 0; frontIndex < n; frontIndex++) {
        CPoint frontPoint   = V[frontIndex];
        CPoint followPoint  = V[followIndex];
             
        if (frontPoint.y < P.y && followPoint.y >= P.y 
            || followPoint.y < P.y && frontPoint.y >= P.y) {
                 
            /**
                * "직선의 기울기 m을 갖는 yPosf에 해당하는 x" < xPosf 인지 체크
                * 두 점을 지나는 직선의 방정식 참고.
                *      y - y1 = M * (x - x1)
                *      M = (y2 - y1) / (x2 - x1)
                */
            if (frontPoint.x + (P.y - frontPoint.y) / (float)(followPoint.y - frontPoint.y) 
                * (followPoint.x - frontPoint.x) < (float)P.x) {
                isOddNodes = !isOddNodes;
            }
        }
             
        followIndex = frontIndex;
    }
 
    /**
        * "기울기 m을 갖는 yPosf에 해당하는 x" < xPosf의 개수가 홀수이면
        * 다각형안에 포함된 점이다.
        */
    return isOddNodes;
}

BOOL CROIGeometryPolygon::ToExport(ROIData& o_data) const
{
	o_data.nType	= GetType();

	o_data.vecPoint = m_vecPoint;
	o_data.rt		= POLYGON2RECT(m_vecPoint);

	return TRUE;
}
