#include "stdafx.h"
#include "ImageWindowApp.h"
#include "ImageWindow_Impl2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Overlays func.   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CImageWindow_Impl2::uOverlayDrawAll(CDC *pDC, BOOL bDrawOverlay, BOOL bDrawCrossLine)
{
	FastDrawDCReady(pDC);

    if(bDrawOverlay)
    {
	    uOverlayDrawLine		(m_FastDC, m_vecLine);
	    uOverlayDrawRect		(m_FastDC, m_vecRect);
	    uOverlayDrawPoint		(m_FastDC, m_vecPoint);
	    uOverlayDrawEllipse		(m_FastDC, m_vecEllipse);
	    uOverlayDrawArcDoughnut	(m_FastDC, m_vecArcDoughnut);
		uOverlayDrawArcRing		(m_FastDC, m_vecArcRing);
    }

    if(bDrawCrossLine)
    {
        CRect rtImage(0, 0, m_imgShowSelImage.nWidth, m_imgShowSelImage.nHeight);

        float fCenterX = 0.5f * (m_imgShowSelImage.nWidth - 1);
        float fCenterY = 0.5f * (m_imgShowSelImage.nHeight - 1);

		CPoint pt[4];
		pt[0] = ImageToScreen(fCenterX, 0.f);
		pt[1] = ImageToScreen(fCenterX, (float)rtImage.Height() - 1.f);
		pt[2] = ImageToScreen(0.f, fCenterY);
		pt[3] = ImageToScreen((float)rtImage.Width() - 1.f, fCenterY);

		m_FastDC.Draw_Line(pt[0], pt[1], RGB(255, 255, 0));
        m_FastDC.Draw_Line(pt[2], pt[3], RGB(255, 255, 0));
   }

	FastDrawDCExcute();

    if(bDrawOverlay)
    {
	    uOverlayDrawText	(pDC, m_vecText);
    }
}

void CImageWindow_Impl2::uDrawLine(CDC *pdc,CPoint pt1,CPoint pt2, COLORREF crColor)
{
	if( !pdc ) return;

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, crColor);

	CPen* pOldPen = pdc->SelectObject(&pen);

	pdc->MoveTo(pt1);
	pdc->LineTo(pt2);

	pdc->SelectObject(pOldPen);
}

void CImageWindow_Impl2::uDrawRect(CDC *pdc,CRect rect)
{
	CRect o_Rect = ImageToScreenRect(rect);

	pdc->Rectangle(&o_Rect);	

}

void CImageWindow_Impl2::uDrawRectFill(CDC *pdc,CRect rect, COLORREF clr )
{
	CRect o_Rect = ImageToScreenRect(rect);
	CBrush brush = CBrush(clr);

	pdc->FillRect( o_Rect, &brush);	

}

void CImageWindow_Impl2::uDrawPoly(CDC *pdc, const vector<CPoint>& vecPoint)
{
	vector<CPoint> vecPointScreen(vecPoint.size());

	for(long i = 0 ; i < (long)vecPointScreen.size() ; ++i)
	{
		vecPointScreen[i] = ImageToScreen(vecPoint[i]);
	}

	pdc->MoveTo(vecPointScreen[0]);
	pdc->Polygon(&vecPointScreen[0], (int)vecPointScreen.size());
}

void CImageWindow_Impl2::uDrawPolyFill(CDC *pdc, const vector<CPoint>& vecPoint, COLORREF clr)
{
	uDrawPoly(pdc, vecPoint);
}

void CImageWindow_Impl2::uDrawText(CDC *pdc, float fX, float fY, LPCTSTR str)
{
	CPoint ptScreen = ImageToScreen(fX, fY);

	pdc->TextOut(ptScreen.x, ptScreen.y, str);	
}
 
////////////////////////////////////////////////////////////////////////

void CImageWindow_Impl2::uOverlayDrawLine(CFastDrawDC &dc, std::vector<Overlay::Line>& ArsLine)
{
	// 반복자를 사용하면 죽는다.. 왜????
	// 나중에 이유 찾을 것.
	// iVisionCal 에서 죽음..
	for(long n=0; n<(long)ArsLine.size(); n++)
	{
		Overlay::Line &line = ArsLine[n];

		CPoint pt1 = ImageToScreen(line.fX1, line.fY1);
		CPoint pt2 = ImageToScreen(line.fX2, line.fY2);

		m_FastDC.Draw_Line(pt1, pt2, line.crColor);
	}
}

void CImageWindow_Impl2::uOverlayDrawRect(CFastDrawDC &dc, std::vector<Overlay::Rect>& ArsRect)
{	
	std::vector<Overlay::Rect>::iterator itr = ArsRect.begin();
	std::vector<Overlay::Rect>::iterator itrEnd = ArsRect.end();

	CRect o_Rect(0, 0, 0, 0);
	while(itr != itrEnd)
	{
		Overlay::Rect &rt = *itr;

		o_Rect = ImageToScreenRect(CRect((int)rt.fLeft, (int)rt.fTop, (int)rt.fRight, (int)rt.fBottom));

		if(	rt.fRight > rt.fLeft && rt.fBottom > rt.fTop && o_Rect.IsRectEmpty())
		{
			dc.Draw_Point(o_Rect.left, o_Rect.top, rt.crColor);
		}
		else
		{
			dc.Draw_Rectangle(o_Rect, rt.crColor);
		}

		++itr;
	}

}

void CImageWindow_Impl2::uOverlayDrawEllipse(CFastDrawDC &dc, std::vector<Overlay::Ellipse>& ArsEllipse)
{
	std::vector<Overlay::Ellipse>::iterator itr = ArsEllipse.begin();
	std::vector<Overlay::Ellipse>::iterator itrEnd = ArsEllipse.end();

	CRect o_Rect(0, 0, 0, 0);
	while(itr != itrEnd)
	{
		Overlay::Ellipse &ellipse = *itr;

		o_Rect = ImageToScreenRect(ellipse.fLeft, ellipse.fTop, ellipse.fRight, ellipse.fBottom);

		dc.Draw_Ellipse(o_Rect, ellipse.crColor);

		++itr;
	}
}

void CImageWindow_Impl2::uOverlayDrawPoint(CFastDrawDC &dc, std::vector<Overlay::Point>& ArsPt)
{
	std::vector<Overlay::Point>::iterator itr = ArsPt.begin();
	std::vector<Overlay::Point>::iterator itrEnd = ArsPt.end();

	CRect o_Rect(0, 0, 0, 0);
	while(itr != itrEnd)
	{
		CPoint ptScreen = ImageToScreen(itr->fX, itr->fY);

		//if(m_fScaleX >= 3.f || m_fScaleY >= 3.f)
		//{
			o_Rect.left		= ptScreen.x - 1;
			o_Rect.top		= ptScreen.y - 1;
			o_Rect.right	= ptScreen.x + 2;
			o_Rect.bottom	= ptScreen.y + 2;
		//}
		//else
		//{
		//	o_Rect.left		= ptScreen.x - 0;
		//	o_Rect.top		= ptScreen.y - 0;
		//	o_Rect.right	= ptScreen.x + 1;
		//	o_Rect.bottom	= ptScreen.y + 1;
		//}

		dc.Draw_Rectangle(o_Rect, itr->crColor);

		++itr;
	}
}

void CImageWindow_Impl2::uOverlayDrawText(CDC *pDC, std::vector<Overlay::Text>& ArsText)
{
	if(!ArsText.empty())
	{
		pDC->SetBkMode(TRANSPARENT);

		CFont *pOldFont = pDC->SelectObject(m_pDefaultFont);

		for(long n=0; n<(long)ArsText.size(); n++)
		{
			if(ArsText[n].pFont)
			{
				CFont font;
				font.CreateFontIndirect(ArsText[n].pFont);
				pDC->SelectObject(&font);
			}

			// Text 3개 이상시 자꾸 다운됨..
			pDC->SetTextColor(ArsText[n].crColor);
			uDrawText(pDC, ArsText[n].fX, ArsText[n].fY, ArsText[n].str);
			pDC->SelectObject(m_pDefaultFont);
		}

		pDC->SelectObject(pOldFont);
	}
}

void CImageWindow_Impl2::FastDrawDCReady(CDC* pDC)
{
	__CSFastDrawDC.Lock();
	m_FastDC.Draw_Ready(pDC);

}
void CImageWindow_Impl2::FastDrawDCExcute()
{
	m_FastDC.Draw_Excute();
	__CSFastDrawDC.Unlock();
}


void CImageWindow_Impl2::uOverlayDrawArcDoughnut(CFastDrawDC &dc, std::vector<Overlay::ArcDoughnut>& ArsArcDoughnut)
{
	if(!ArsArcDoughnut.empty())
	{
		CRect o_Rect(0, 0, 0, 0);
		for(long n=0; n<(long)ArsArcDoughnut.size(); n++)
		{
			////////////////////////////////////////////////

			CPoint ptCenter = ImageToScreen(ArsArcDoughnut[n].fCenX, ArsArcDoughnut[n].fCenY);
			CSize Radius1	= ImageToScreenDist	(ArsArcDoughnut[n].fRadiusX - ArsArcDoughnut[n].fWidth * 0.5f);
			CSize Radius2	= ImageToScreenDist	(ArsArcDoughnut[n].fRadiusY - ArsArcDoughnut[n].fWidth * 0.5f);
			CSize Radius3	= ImageToScreenDist	(ArsArcDoughnut[n].fRadiusX + ArsArcDoughnut[n].fWidth * 0.5f);
			CSize Radius4	= ImageToScreenDist	(ArsArcDoughnut[n].fRadiusY + ArsArcDoughnut[n].fWidth * 0.5f);

			float fAng1 = ArsArcDoughnut[n].fStartAngle;
			float fAng2 = ArsArcDoughnut[n].fEndAngle;

			CPoint pt[4];
			dc.Draw_Arc(ptCenter, Radius1.cx, Radius2.cy, fAng1, fAng2, ArsArcDoughnut[n].crColor, &pt[0], &pt[1]);
			dc.Draw_Arc(ptCenter, Radius3.cx, Radius4.cy, fAng1, fAng2, ArsArcDoughnut[n].crColor, &pt[2], &pt[3]);
			dc.Draw_Line(pt[0], pt[2], ArsArcDoughnut[n].crColor);
			dc.Draw_Line(pt[1], pt[3], ArsArcDoughnut[n].crColor);

			////////////////////////////////////////////////
		}
	}
}

void CImageWindow_Impl2::uOverlayDrawArcRing(CFastDrawDC &dc, std::vector<Overlay::ArcRing>& ArsArcRing)
{
	if(!ArsArcRing.empty())
	{
		CRect o_Rect(0, 0, 0, 0);
		for(long n=0; n<(long)ArsArcRing.size(); n++)
		{
			////////////////////////////////////////////////

			CPoint ptCenter = ImageToScreen(ArsArcRing[n].fCenX, ArsArcRing[n].fCenY);
			CSize Radius1	= ImageToScreenDist	(ArsArcRing[n].fInnerLength);
			CSize Radius2	= ImageToScreenDist	(ArsArcRing[n].fInnerLength);
			CSize Radius3	= ImageToScreenDist	(ArsArcRing[n].fOuterLength);
			CSize Radius4	= ImageToScreenDist	(ArsArcRing[n].fOuterLength);

			float fAng1 = ArsArcRing[n].fStartAngle;
			float fAng2 = ArsArcRing[n].fEndAngle;

			CPoint pt[4];
			dc.Draw_Arc(ptCenter, Radius1.cx, Radius2.cy, fAng1, fAng2, ArsArcRing[n].crColor, &pt[0], &pt[1]);
			dc.Draw_Arc(ptCenter, Radius3.cx, Radius4.cy, fAng1, fAng2, ArsArcRing[n].crColor, &pt[2], &pt[3]);
			dc.Draw_Line(pt[0], pt[2], ArsArcRing[n].crColor);
			dc.Draw_Line(pt[1], pt[3], ArsArcRing[n].crColor);

			////////////////////////////////////////////////
		}
	}
}
