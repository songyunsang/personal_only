#include "stdafx.h"
#include "ROIEditor.h"

void CROIEditor::Generate(const std::vector<CPoint>& vecROI)
{
	m_vecROI = vecROI;

	m_vecKnob.clear();

	for(long i = 0 ; i < (long)vecROI.size() ; ++i)
	{
		CRect rtROI;

		rtROI.left		= vecROI[i].x - KNOBSIZE;
		rtROI.top		= vecROI[i].y - KNOBSIZE;
		rtROI.right		= vecROI[i].x + KNOBSIZE;
		rtROI.bottom	= vecROI[i].y + KNOBSIZE;

		std::vector<long> vecPointIndex;
		std::vector<CPoint> vecCoef;

		vecPointIndex	.push_back(i);
		vecCoef			.push_back(CPoint(1, 1));
		m_vecKnob		.push_back(CKnob(vecPointIndex, vecCoef, rtROI));
	}
}

void CROIEditor::Generate(const CRect& rtROI)
{
	int L = rtROI.left;
	int T = rtROI.top;
	int R = rtROI.right;
	int B = rtROI.bottom;

	int MX = (L + R) / 2;
	int MY = (T + B) / 2;

	std::vector<CPoint>	vecKnobPos;
	std::vector<long>	vecPointIndex;
	std::vector<CPoint>	vecCoef;

	m_vecROI.clear();
	m_vecROI.push_back(CPoint(L, T));	long nLT = 0;
	m_vecROI.push_back(CPoint(R, B));	long nRB = 1;

	CPoint k11 = CPoint(1, 1);
	CPoint k10 = CPoint(1, 0);
	CPoint k01 = CPoint(0, 1);

	std::vector<std::vector<long>>	vecPointIndexList;
	std::vector<std::vector<CPoint>>	vecCoefList;

	vecKnobPos			.push_back(CPoint(L, T));	vecPointIndex.push_back(nLT); vecCoef.push_back(k11);	
	vecPointIndexList	.push_back(vecPointIndex);	vecPointIndex.clear();
	vecCoefList			.push_back(vecCoef);		vecCoef.clear();

	vecKnobPos			.push_back(CPoint(MX, T));	vecPointIndex.push_back(nLT); vecCoef.push_back(k01);
	vecPointIndexList	.push_back(vecPointIndex);	vecPointIndex.clear();
	vecCoefList			.push_back(vecCoef);		vecCoef.clear();

	vecKnobPos			.push_back(CPoint(R, T));	vecPointIndex.push_back(nRB); vecCoef.push_back(k10);
													vecPointIndex.push_back(nLT); vecCoef.push_back(k01);
	vecPointIndexList	.push_back(vecPointIndex);	vecPointIndex.clear();
	vecCoefList			.push_back(vecCoef);		vecCoef.clear();

	vecKnobPos			.push_back(CPoint(L, MY));	vecPointIndex.push_back(nLT); vecCoef.push_back(k10);
	vecPointIndexList	.push_back(vecPointIndex);	vecPointIndex.clear();
	vecCoefList			.push_back(vecCoef);		vecCoef.clear();

	vecKnobPos			.push_back(CPoint(R, MY));	vecPointIndex.push_back(nRB); vecCoef.push_back(k10);
	vecPointIndexList	.push_back(vecPointIndex);	vecPointIndex.clear();
	vecCoefList			.push_back(vecCoef);		vecCoef.clear();

	vecKnobPos			.push_back(CPoint(L, B));	vecPointIndex.push_back(nLT); vecCoef.push_back(k10);
													vecPointIndex.push_back(nRB); vecCoef.push_back(k01);
	vecPointIndexList	.push_back(vecPointIndex);	vecPointIndex.clear();
	vecCoefList			.push_back(vecCoef);		vecCoef.clear();

	vecKnobPos			.push_back(CPoint(MX, B));	vecPointIndex.push_back(nRB); vecCoef.push_back(k01);
	vecPointIndexList	.push_back(vecPointIndex);	vecPointIndex.clear();
	vecCoefList			.push_back(vecCoef);		vecCoef.clear();

	vecKnobPos			.push_back(CPoint(R, B));	vecPointIndex.push_back(nRB); vecCoef.push_back(k11);
	vecPointIndexList	.push_back(vecPointIndex);	vecPointIndex.clear();
	vecCoefList			.push_back(vecCoef);		vecCoef.clear();

	m_vecKnob.clear();

	for(long i = 0 ; i <(long)vecKnobPos.size() ; ++i)
	{
		CRect rtROI;

		rtROI.left		= vecKnobPos[i].x - KNOBSIZE;
		rtROI.top		= vecKnobPos[i].y - KNOBSIZE;
		rtROI.right		= vecKnobPos[i].x + KNOBSIZE;
		rtROI.bottom	= vecKnobPos[i].y + KNOBSIZE;

		m_vecKnob.push_back(CKnob(vecPointIndexList[i], vecCoefList[i], rtROI));
	}
}

void CROIEditor::Apply(long nKnobIndex, const CPoint& diff)
{
	if( OutOfRange(m_vecKnob, nKnobIndex) ) return;

	CKnob& knob = m_vecKnob[nKnobIndex];

	for(long i = 0 ; i < (long)knob.vecPointIndex.size(); ++i)
	{
		long	index	= knob.vecPointIndex[i];
		CPoint	coef	= knob.vecCoef[i];

		long dx = diff.x * coef.x;
		long dy = diff.y * coef.y;

		m_vecROI[index].x += dx;
		m_vecROI[index].y += dy;

		knob.rtROI.OffsetRect(diff);
	}
}
	
const std::vector<CPoint>& CROIEditor::GetROI() const
{
	return m_vecROI;
}

long CROIEditor::GetKnobCount() const
{
	return (long)m_vecKnob.size();
}

const CRect& CROIEditor::GetKnobROI(long nIndex) const
{
	if( OutOfRange(m_vecKnob, nIndex) )
	{
		const static CRect dummy(0, 0, 0, 0);

		return dummy;
	}

	return m_vecKnob[nIndex].rtROI; 
}

long CROIEditor::FindKnobIndex(const CPoint& pos) const
{
	for(long i = 0 ; i < (long)m_vecKnob.size() ; ++i)
	{
		if( m_vecKnob[i].rtROI.PtInRect(pos) ) return i;
	}

	return -1;
}