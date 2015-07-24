#pragma once
#include <vector>
#include <list>

using namespace std;
// CGDIPlusPlotChart

typedef struct plot_struct 
{
	short					index;
	CString					name;
	Gdiplus::Color			color;
	int						thickness;
	std::vector<double>		datalist;
	std::vector<BOOL>		markvisible;
} plot_typedef;


class CGDIPlusPlotChart : public CStatic
{
public:
	void UpdateScreenParameter(const CRect& rtBounds);

	double PlotToScreenX(double x) const;
	double PlotToScreenY(double y) const;

	double ScreenToPlotX(double x) const;
	double ScreenToPlotY(double y) const;

	double __plot_to_screen_x;
	double __plot_to_screen_y;

private:
	void DrawCaption(CRect rtBounds);
	void DrawXLabel(CRect rtBounds);
	void DrawYLabel(CRect rtBounds);
	void DrawQuadrant(CRect rtBounds);
    void DrawGridX();
    void DrawGridY();
    void DrawPlotXY();
	void DrawCurve(short index);

	void DrawMark();
	void DrawPlotName();
	


public:
	CString GetCaption();
	void SetCaption(LPCTSTR lpszNewValue);
	CString GetXLabel();
	void SetXLabel(LPCTSTR lpszNewValue);
	CString GetYLabel();
	void SetYLabel(LPCTSTR lpszNewValue);
	void SetRange(double xMin, double xMax, double yMin, double yMax);
	void ClearGraph();

public:	
	long InsertPlot(long nIndex, LPCTSTR szName, COLORREF rgb, int nThickness);
	BOOL ErasePlot(long nIndex);

	BOOL SetData		(long nPlotIndex, const vector<double>& vecData);
	BOOL SetMarkVisible	(long nPlotIndex, const vector<BOOL>& vecMarkVisible);

public:
	DECLARE_DYNAMIC(CGDIPlusPlotChart)

public:
	CGDIPlusPlotChart();
	virtual ~CGDIPlusPlotChart();
		
private:
	ULONG_PTR						m_gdiplusToken;
	Gdiplus::Graphics				*m_MemoryGraphics;

	CString							m_caption, m_xLabel, m_yLabel;
	double							m_xMin, m_xMax, m_yMin, m_yMax;
	int								m_QuadrantX, m_QuadrantY, m_QuadrantWidth, m_QuadrantHeight;

	std::list<plot_typedef>					m_plotlist;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

};
