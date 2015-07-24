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
        * �Ʒ� �˰����� "Point in Polygon" �˰����̴�.
        * �ٸ� �¿� �� ������ üũ�ϴ� ���� �ƴ϶� ���� ���⸸�� üũ�Ѵ�.
        */
    for (int frontIndex = 0; frontIndex < n; frontIndex++) {
        CPoint frontPoint   = V[frontIndex];
        CPoint followPoint  = V[followIndex];
             
        if (frontPoint.y < P.y && followPoint.y >= P.y 
            || followPoint.y < P.y && frontPoint.y >= P.y) {
                 
            /**
                * "������ ���� m�� ���� yPosf�� �ش��ϴ� x" < xPosf ���� üũ
                * �� ���� ������ ������ ������ ����.
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
        * "���� m�� ���� yPosf�� �ش��ϴ� x" < xPosf�� ������ Ȧ���̸�
        * �ٰ����ȿ� ���Ե� ���̴�.
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
