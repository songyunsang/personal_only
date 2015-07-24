#pragma once
#define KNOBSIZE					(3)

#include <vector>

struct EditROI{

	ROIData Data;
	long	nLineWidth;

	CString strName;

	COLORREF crColor;
	BOOL	isMovable;
	BOOL	isResizable;
};

class CROIEditor
{
public:
	void Generate(const std::vector<CPoint>& vecROI);
	void Generate(const CRect& rtROI);

	void Apply(long nKnobIndex, const CPoint& diff);
	
	const std::vector<CPoint>& GetROI() const;

	long			GetKnobCount() const;		// display, select ��
	const CRect&	GetKnobROI(long nIndex) const;			// display, select ��

	long			FindKnobIndex(const CPoint& pos) const;

private:
	class CKnob
	{
	public:
		std::vector<long>	vecPointIndex;		// ����� PointIndex
		std::vector<CPoint>	vecCoef;				// ����� ����� ��� : -1, 0, 1
		CRect	rtROI;

	public:
		CKnob(std::vector<long> index, const std::vector<CPoint>& coef, const CRect& roi)
			: vecPointIndex(index)
			, vecCoef(coef)
			, rtROI(roi) {}
	};	

private:
	std::vector<CKnob>	m_vecKnob;		// ������ ������
	std::vector<CPoint>	m_vecROI;		// ���� ������ ROI����
};