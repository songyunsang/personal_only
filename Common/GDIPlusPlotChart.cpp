// CGDIPlusPlotChart.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include <GdiPlus.h>
#pragma comment(lib, "GdiPlus.lib")		// 瞳젯쌈珂瞳壇痰杰瞳커쩌샀溝固LIB커쩌苟꿴冷GdiPlus.lib

#include "GDIPlusPlotChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define RED_RGB Gdiplus::Color(255, 255, 0, 0)	        
#define BLACK_RGB Gdiplus::Color(255, 0, 0, 0)	        
#define GREEN_RGB Gdiplus::Color(255, 0, 255, 0)        
#define WHITE_RGB Gdiplus::Color(255, 255, 255, 255)    
#define YELLOW_RGB Gdiplus::Color(255, 255, 255, 0)     
#define BLUE_RGB Gdiplus::Color(255, 0, 0, 255)			
#define WHITE_ARGB(X) Gdiplus::Color(X, 255, 255, 255)  
#define LIGHT_WHITE_RGB WHITE_ARGB(86)					
#define CAPTION_SIZE 16							        
#define MARGIN 0								        
#define LABEL_SIZE 12							        
#define SCALE_SIZE 10                                   
#define X_UNIT_ONE 10								    
#define Y_UNIT_ONE 5                                    
#define BUFFER_SIZE 1024                                
#define CURVE_POINTS_SIZE 1024
#define X_CURSOR_STEP 20
#define MAX_RANGE_Y 40
#define MIN_RANGE_Y 10
#define DEBUG_MSGBOX(X) MessageBox(X, "DEBUG", MB_OK | MB_ICONINFORMATION)

IMPLEMENT_DYNAMIC(CGDIPlusPlotChart, CStatic)

CGDIPlusPlotChart::CGDIPlusPlotChart()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	m_MemoryGraphics = NULL;

	m_caption	    = L"GDIPlusPlot";
	m_xLabel	    = L"X AXIS";	
	m_yLabel	    = L"Y AXIS";	
    m_xMin		    = 0;               
	m_xMax		    = 255;             
	m_yMin		    = 0;             
	m_yMax		    = 255;            
}

CGDIPlusPlotChart::~CGDIPlusPlotChart()
{
	if (m_MemoryGraphics) delete m_MemoryGraphics; m_MemoryGraphics = NULL;

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}


BEGIN_MESSAGE_MAP(CGDIPlusPlotChart, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CGDIPlusPlotChart::DrawCaption(CRect rtBounds) 
{    
	Gdiplus::FontFamily		fontFamily(L"Courier New");
	Gdiplus::Font			font(&fontFamily, CAPTION_SIZE, Gdiplus::FontStyleBold, 
            Gdiplus::UnitPixel);
	Gdiplus::RectF			layoutRect(0.0f, MARGIN, rtBounds.Width(), CAPTION_SIZE);
	Gdiplus::StringFormat	format;
	Gdiplus::SolidBrush		solidBrush(BLACK_RGB);

	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_MemoryGraphics->DrawString(m_caption, -1, &font, layoutRect, &format, &solidBrush);
}

void CGDIPlusPlotChart::DrawXLabel(CRect rtBounds) 
{
	Gdiplus::FontFamily		fontFamily(L"Courier New");
	Gdiplus::Font			font(&fontFamily, LABEL_SIZE, Gdiplus::FontStyleRegular, 
            Gdiplus::UnitPixel);
	Gdiplus::RectF			layoutRect(0.0f, rtBounds.Height() - CAPTION_SIZE - MARGIN, rtBounds.Width(), CAPTION_SIZE);
	Gdiplus::StringFormat	format;
	Gdiplus::SolidBrush		solidBrush(BLACK_RGB);

	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_MemoryGraphics->DrawString(m_xLabel, -1, &font, layoutRect, &format, &solidBrush);
}

void CGDIPlusPlotChart::DrawYLabel(CRect rtBounds) 
{
	Gdiplus::FontFamily		fontFamily(L"Courier New");
	Gdiplus::Font			font(&fontFamily, LABEL_SIZE, Gdiplus::FontStyleRegular, 
            Gdiplus::UnitPixel);
	Gdiplus::RectF			layoutRect(0.0f, 0.0f, LABEL_SIZE + MARGIN, rtBounds.Height());
	Gdiplus::StringFormat	format;
	Gdiplus::SolidBrush		solidBrush(BLACK_RGB);

	format.SetFormatFlags(Gdiplus::StringFormatFlagsDirectionVertical | Gdiplus::StringFormatFlagsDirectionRightToLeft);
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_MemoryGraphics->DrawString(m_yLabel, -1, &font, layoutRect, &format, &solidBrush);
}

void CGDIPlusPlotChart::DrawMark()
{
	for(list<plot_typedef>::iterator it = m_plotlist.begin() ; it != m_plotlist.end() ; ++it)
	{
		if( it->markvisible.empty() ) continue;

		const vector<double>&	datalist	= it->datalist;
		const vector<BOOL>&		markvisible	= it->markvisible;

		for(long i = 0 ; i < (long)markvisible.size() ; ++i)
		{
			if( markvisible[i] != TRUE ) continue;

			double x = PlotToScreenX(i);
			double y = PlotToScreenY(datalist[i]);

			int marksize = 3;
			Gdiplus::Pen	pen(RED_RGB, 2);
			Gdiplus::PointF p1, p2, p3, p4;
			p1.X = x - marksize;
			p1.Y = y - marksize;
			p2.X = x + marksize;
			p2.Y = y + marksize;
			p3.X = x + marksize;
			p3.Y = y - marksize;
			p4.X = x - marksize;
			p4.Y = y + marksize;

			m_MemoryGraphics->DrawLine(&pen, p1, p2);
			m_MemoryGraphics->DrawLine(&pen, p3, p4);
		}
	}
}

void CGDIPlusPlotChart::DrawPlotName()
{
	Gdiplus::FontFamily		fontFamily(L"Courier New");
	Gdiplus::Font			font(&fontFamily, LABEL_SIZE, Gdiplus::FontStyleRegular, 
		Gdiplus::UnitPixel);


	for(list<plot_typedef>::iterator it = m_plotlist.begin() ; it != m_plotlist.end() ; ++it)
	{
		if( it->datalist.empty() ) continue;

		double x = PlotToScreenX(255);
		double y = PlotToScreenY(it->datalist[255]);

		Gdiplus::RectF			layoutRect(x - 30, y - LABEL_SIZE - 5, x, y);
		Gdiplus::SolidBrush		solidBrush(it->color);

		Gdiplus::StringFormat	format;
		format.SetAlignment(Gdiplus::StringAlignmentNear);

		CString strString = it->name;
		m_MemoryGraphics->MeasureString(strString, -1, &font, Gdiplus::PointF(0, 0), &layoutRect);

		layoutRect = Gdiplus::RectF(x - layoutRect.Width, y - layoutRect.Height, x, y);
		m_MemoryGraphics->DrawString(strString, -1, &font, layoutRect, &format, &solidBrush);
	}


}

void CGDIPlusPlotChart::DrawCurve(short index) 
{
    Gdiplus::PointF                 curvePoints[CURVE_POINTS_SIZE];
	std::list<plot_typedef>::iterator		EleIter;
    Gdiplus::FontFamily		        fontFamily(L"Courier New");
	Gdiplus::Font			        font(&fontFamily, SCALE_SIZE, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::StringFormat	        format;
	Gdiplus::SolidBrush		        solidBrush(WHITE_RGB);
	
    format.SetAlignment(Gdiplus::StringAlignmentNear);

	for (EleIter = m_plotlist.begin(); 
            EleIter != m_plotlist.end(); EleIter++) 
	{
		if (index == (*EleIter).index) 
		{
			Gdiplus::Pen pen((*EleIter).color, EleIter->thickness);

			memset(curvePoints, 0, CURVE_POINTS_SIZE);

			const vector<double>& datalist = EleIter->datalist;
			for(int i = 0 ; i < (int)datalist.size() ; ++i )
			{
				float x = PlotToScreenX(i);
				float y = PlotToScreenY(datalist[i]);

				Gdiplus::PointF point(x, y);

				curvePoints[i].X = PlotToScreenX(i);
				curvePoints[i].Y = PlotToScreenY(datalist[i]);
			}
			m_MemoryGraphics->DrawCurve(&pen, curvePoints, EleIter->datalist.size());
            
			break;
		}
	}
}

void CGDIPlusPlotChart::DrawPlotXY() 
{
    unsigned int e;

	for (e = 0; e < m_plotlist.size(); e++) 
	{
		DrawCurve(e);
	}
}

void CGDIPlusPlotChart::DrawGridX() 
{
	Gdiplus::Pen	pen(LIGHT_WHITE_RGB);

	float			y1 = m_QuadrantY, y2 = m_QuadrantY + m_QuadrantHeight;

	for(float x = m_QuadrantX ; x < m_QuadrantX + m_QuadrantWidth ; x+=__plot_to_screen_x * 5)
	{
		m_MemoryGraphics->DrawLine(&pen, x, y1, x, y2);
	}
}

void CGDIPlusPlotChart::DrawGridY() 
{
	Gdiplus::Pen	pen(LIGHT_WHITE_RGB);

	float			x1 = m_QuadrantX, x2 = m_QuadrantX + m_QuadrantWidth;

	for(float y = m_QuadrantY + m_QuadrantHeight ; y >m_QuadrantY ; y-=__plot_to_screen_y * 25)
	{
		m_MemoryGraphics->DrawLine(&pen, x1, y, x2, y);
	}
}

void CGDIPlusPlotChart::DrawQuadrant(CRect rtBounds) 
{
  	Gdiplus::SolidBrush	solidBrush(BLACK_RGB);
	m_MemoryGraphics->FillRectangle(&solidBrush, m_QuadrantX, m_QuadrantY, m_QuadrantWidth, m_QuadrantHeight);

	m_MemoryGraphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);

	DrawGridX();

	DrawGridY();

	m_MemoryGraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

    DrawPlotXY();
}

// CGDIPlusPlotChart 메시지 처리기입니다.

void CGDIPlusPlotChart::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rtBounds;

	GetClientRect(&rtBounds);

	UpdateScreenParameter(rtBounds);

	Gdiplus::Graphics pGraphics(dc.m_hDC);
	Gdiplus::Bitmap pBitmap(rtBounds.Width(), rtBounds.Height());

	m_MemoryGraphics = Gdiplus::Graphics::FromImage(&pBitmap);

	m_MemoryGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	OLE_COLOR pWndBkColor = GetBkColor(dc.m_hDC);
	Gdiplus::Rect pWndRect(0, 0, rtBounds.Width(), rtBounds.Height());
	COLORREF pWndRGB = (pWndBkColor);
	Gdiplus::SolidBrush pWndBg(Gdiplus::Color(255, GetRValue(pWndRGB), GetGValue(pWndRGB), GetBValue(pWndRGB)));
	m_MemoryGraphics->FillRectangle(&pWndBg, pWndRect);

	DrawCaption(rtBounds);
	
	DrawXLabel(rtBounds);

	DrawYLabel(rtBounds);

	DrawQuadrant(rtBounds);

	//m_MemoryGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	DrawMark();
	DrawPlotName();

	pGraphics.DrawImage(&pBitmap, rtBounds.left, rtBounds.top);

	if (m_MemoryGraphics) delete m_MemoryGraphics; m_MemoryGraphics = NULL;
}

CString CGDIPlusPlotChart::GetCaption() 
{
	return m_caption;
}

void CGDIPlusPlotChart::SetCaption(LPCTSTR lpszNewValue) 
{
	m_caption = lpszNewValue;
}

void CGDIPlusPlotChart::SetRange(double xMin, double xMax, double yMin, double yMax) 
{
	m_xMin = xMin; 
	m_xMax = xMax; 
	m_yMin = yMin; 
	m_yMax = yMax;
}

CString CGDIPlusPlotChart::GetXLabel() 
{
	return m_xLabel;
}

void CGDIPlusPlotChart::SetXLabel(LPCTSTR lpszNewValue) 
{
	m_xLabel = lpszNewValue;
}

CString CGDIPlusPlotChart::GetYLabel() 
{
	return m_yLabel;
}

void CGDIPlusPlotChart::SetYLabel(LPCTSTR lpszNewValue) 
{
	m_yLabel = lpszNewValue;
}


void CGDIPlusPlotChart::ClearGraph() 
{
	std::list<plot_typedef>::iterator pIter;

	for (pIter = m_plotlist.begin(); pIter != m_plotlist.end(); pIter++) 
	{
		(*pIter).datalist.clear();
	}
}

void CGDIPlusPlotChart::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
}

long CGDIPlusPlotChart::InsertPlot(long nIndex, LPCTSTR szName, COLORREF rgb, int nThickness)
{
	plot_typedef plot;

	plot.name = szName;
	plot.color.SetFromCOLORREF(rgb);
	plot.thickness = nThickness;

	if( m_plotlist.size() < nIndex )
	{
		plot.index = m_plotlist.size();

		m_plotlist.push_back(plot);

		return (long)m_plotlist.size();
	}
	else
	{
		list<plot_typedef>::iterator it = m_plotlist.begin();

		for(long i = 0 ; i < nIndex ; ++i ) ++it;

		plot.index = nIndex;

		m_plotlist.insert(it, plot);

		return nIndex;
	}
}

BOOL CGDIPlusPlotChart::ErasePlot(long nIndex)
{
	if( m_plotlist.size() <= nIndex ) return FALSE;

	list<plot_typedef>::iterator it = m_plotlist.begin();

	for(long i = 0 ; i < nIndex ; ++i ) ++it;

	m_plotlist.erase(it);

	return TRUE;

}

BOOL CGDIPlusPlotChart::SetData(long nPlotIndex, const vector<double>& vecData)
{
	if( m_plotlist.size() <= nPlotIndex ) return FALSE;

	list<plot_typedef>::iterator it = m_plotlist.begin();

	for(long i = 0 ; i < nPlotIndex ; ++i) ++it;

	it->datalist = vecData;

	return TRUE;
}

BOOL CGDIPlusPlotChart::SetMarkVisible(long nPlotIndex, const vector<BOOL>& vecMarkVisible)
{
	if( m_plotlist.size() <= nPlotIndex ) return FALSE;

	list<plot_typedef>::iterator it = m_plotlist.begin();

	for(long i = 0 ; i < nPlotIndex ; ++i) ++it;

	it->markvisible = vecMarkVisible;

	return TRUE;
}

void CGDIPlusPlotChart::UpdateScreenParameter(const CRect& rtBounds)
{
	m_QuadrantX = LABEL_SIZE;
	m_QuadrantY = CAPTION_SIZE + MARGIN; 

	m_QuadrantWidth = rtBounds.Width() - CAPTION_SIZE - MARGIN;
	m_QuadrantHeight = rtBounds.Height() - CAPTION_SIZE - CAPTION_SIZE - MARGIN * 2;

	__plot_to_screen_x = m_QuadrantWidth	/ (m_xMax - m_xMin);
	__plot_to_screen_y = m_QuadrantHeight	/ (m_yMax - m_yMin);
}

double CGDIPlusPlotChart::PlotToScreenX(double x) const
{
	return m_QuadrantX + (x - m_xMin) * __plot_to_screen_x;
}

double CGDIPlusPlotChart::PlotToScreenY(double y) const
{
	return m_QuadrantY + m_QuadrantHeight - (y - m_yMin) * __plot_to_screen_y;
}

double CGDIPlusPlotChart::ScreenToPlotX(double x) const
{
	return (x - m_QuadrantX) / __plot_to_screen_x + m_xMin;
}

double CGDIPlusPlotChart::ScreenToPlotY(double y) const
{
	return (y - m_QuadrantY) / __plot_to_screen_y + m_yMin;
}

