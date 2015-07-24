
#include "stdafx.h"
#include "resultdrawhelper.h"
#include "..\Tools\ImageWindow\ImageWindow.h"
#include "..\iVMBaseLibrary\IV_dataBlobInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CResultDrawHelper::Update_DATA2ImageWindow(const CPoint *pData, int nNum, COLORREF color, CImageWindow *pImageWindow, BOOL bCross)
{
	if(!pData)
		return;

	if(bCross)
	{
		for(int i = 0; i < nNum; i++)
		{
			CPoint pt1, pt2;
			pt1.x = pData[i].x - 100;
			pt1.y = pData[i].y;
			pt2.x = pData[i].x + 100;
			pt2.y = pData[i].y;
			pImageWindow->OverlayAddLine(&pt1, &pt2, color);

			pt1.x = pData[i].x;
			pt1.y = pData[i].y - 100;
			pt2.x = pData[i].x;
			pt2.y = pData[i].y + 100;
			pImageWindow->OverlayAddLine(&pt1, &pt2, color);
		}
	}
	else
	{
		for(int i = 0; i < nNum; i++)
		{
			pImageWindow->OverlayAddPoint(&pData[i], color);
		}
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataPoint *pData, int nNum, COLORREF color, CImageWindow *pImageWindow, BOOL bCross)
{
	if(!pData)
		return;

	if(bCross)
	{
		for(int i = 0; i < nNum; i++)
		{
			const IV_dataPoint &pt = pData[i];

			pImageWindow->OverlayAddFLine(pt.fx - 2, pt.fy, pt.fx + 3, pt.fy, color);
			pImageWindow->OverlayAddFLine(pt.fx, pt.fy - 2, pt.fx, pt.fy + 3, color);
		}
	}
	else
	{
		for(int i = 0; i < nNum; i++)
		{
			pImageWindow->OverlayAddFPoint(pData[i].fx, pData[i].fy, color);
		}
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const CRect *pData, int nNum, COLORREF color, CImageWindow *pImageWindow, BOOL bSmallZoom)
{
	if(!pData)
		return;

	for(int i = 0; i < nNum; i++)
	{
		pImageWindow->OverlayAddRectangle(&pData[i], color);

		// 사이즈가 5보다 작으면 10픽셀짜리 크로스 라인을 그어준다. -> 200픽셀
		if(bSmallZoom && pData[i].Width() < 5 && pData[i].Height() < 5)
		{
			CPoint pt1, pt2;
			pt1.x = pData[i].CenterPoint().x - 100;
			pt1.y = pData[i].CenterPoint().y;
			pt2.x = pData[i].CenterPoint().x + 100;
			pt2.y = pData[i].CenterPoint().y;
			pImageWindow->OverlayAddLine(&pt1, &pt2, color);

			pt1.x = pData[i].CenterPoint().x;
			pt1.y = pData[i].CenterPoint().y - 100;
			pt2.x = pData[i].CenterPoint().x;
			pt2.y = pData[i].CenterPoint().y + 100;
			pImageWindow->OverlayAddLine(&pt1, &pt2, color);
		}
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataQuadrangle *pData, int nNum, COLORREF color, CImageWindow *pImageWindow)
{
	if(!pData)
		return;

	for(int i = 0 ; i < nNum ; i++)
	{
		const IV_dataQuadrangle &quad = pData[i];

		pImageWindow->OverlayAddFLine(quad.m_fLTx, quad.m_fLTy, quad.m_fRTx, quad.m_fRTy, color);
		pImageWindow->OverlayAddFLine(quad.m_fRTx, quad.m_fRTy, quad.m_fRBx, quad.m_fRBy, color);
		pImageWindow->OverlayAddFLine(quad.m_fRBx, quad.m_fRBy, quad.m_fLBx, quad.m_fLBy, color);
		pImageWindow->OverlayAddFLine(quad.m_fLBx, quad.m_fLBy, quad.m_fLTx, quad.m_fLTy, color);
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataCircle *pData, int nNum, COLORREF color, CImageWindow *pImageWindow)
{
	if(!pData) return;

	for(int i = 0; i < nNum; i++)
	{
		pImageWindow->OverlayAddCircle(pData[i].m_fCenX,
		                               pData[i].m_fCenY,
		                               pData[i].m_fRadius,
		                               pData[i].m_fRadius,
		                               color,
		                               FALSE);
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataBlobInfo *pData, int nNum, COLORREF color, CImageWindow *pImageWindow)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		long l = pData[i].m_rtROI.left;
		long t = pData[i].m_rtROI.top;
		long r = pData[i].m_rtROI.right;
		long b = pData[i].m_rtROI.bottom;

		CRect rt(l, t, r, b);
		pImageWindow->OverlayAddRectangle(&rt, color);

		//	strTemp.Format("%d", pData[i].nArea);
		strTemp.Format(_T("%d"), i);

		CPoint pt(l, t);
		pImageWindow->OverlayAddText(&pt, strTemp, color);
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataPolygon *pData, int nNum, COLORREF color, CImageWindow *pImageWindow)
{
	if(!pData)
		return;

	CRect rtConvert(0, 0, 0, 0);
	for(int i = 0; i < nNum; i++)
	{
		long			count	= pData[i].GetVertexNum();
		IV_dataPoint	*pt		= pData[i].GetPoint();

		if(count > 3)
		{
			// Polygon 은 3점은 되야?

			for(int j = 0; j < count - 1; j++)
			{
				pImageWindow->OverlayAddFLine(pt[j].fx, pt[j].fy, pt[j+1].fx, pt[j+1].fy, color);				
			}

			pImageWindow->OverlayAddFLine(pt[count-1].fx, pt[count-1].fy, pt[0].fx, pt[0].fy, color);
		}
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataLineEq *pData, int nNum, COLORREF color, CImageWindow *pImageWindow)
{
	long nImageSizeX = 0, nImageSizeY = 0;
	pImageWindow->GetImage(NULL, &nImageSizeX, &nImageSizeY, NULL, 0);

	for(long i = 0; i < nNum; i++)
	{
		if(fabs(pData[i].m_fa) < fabs(pData[i].m_fb))
		{
			pImageWindow->OverlayAddFLine(0.f, pData[i].GetPositionY(0.f), (float)(nImageSizeX - 1), pData[i].GetPositionY((float)(nImageSizeX - 1)), color);
		}
		else
		{
			pImageWindow->OverlayAddFLine(pData[i].GetPositionX(0.f), 0.f, pData[i].GetPositionX((float)(nImageSizeY - 1)), (float)(nImageSizeY - 1), color);
		}
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataLineSeg *pData, int nNum, COLORREF color, CImageWindow *pImageWindow)
{
	long nImageSizeX = 0, nImageSizeY = 0;
	pImageWindow->GetImage(NULL, &nImageSizeX, &nImageSizeY, NULL, 0);

	for(long i = 0; i < nNum; i++)
	{
		pImageWindow->OverlayAddFLine(	pData[i].m_sx,
		                                pData[i].m_sy,
		                                pData[i].m_ex,
		                                pData[i].m_ey,
		                                color);
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataRectangle *pData,	int nNum, COLORREF color, CImageWindow *pImageWindow)
{
	if(!pData)
		return;

	for(int i = 0; i < nNum; i++)
	{
		IV_dataQuadrangle rtArbitraryFRect = pData[i].Convert_Quadrangle();
		Update_DATA2ImageWindow(&rtArbitraryFRect, 1, color, pImageWindow);
	}
}

void CResultDrawHelper::Update_DATA2ImageWindow(const IV_dataImage *pData, int /*nNum*/, COLORREF /*color*/, CImageWindow *pImageWindow)
{
	if(!pData)
		return;

	pImageWindow->SetDisplayImage(	pData->GetRawMem(), pData->GetSizeX(), pData->GetSizeY(), pData->GetWidthBytes(), pData->GetBpp());
}

void CResultDrawHelper::Update_DATA2String(IV_dataImage *pData, int /*nNum*/, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	strOutput.Format(_T("%sWidth : %d\r\n%sHeight : %d\r\n%sBit/Pixel(depth) : %d\r\n%sWidthBytes : %d\r\n"),
	                 lpOutLine_StartCharectors, pData->GetSizeX(),
	                 lpOutLine_StartCharectors, pData->GetSizeY(),
	                 lpOutLine_StartCharectors, pData->GetBpp(),
	                 lpOutLine_StartCharectors, pData->GetWidthBytes());
}

void CResultDrawHelper::Update_DATA2String(CString *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\t%s\r\n"), lpOutLine_StartCharectors, i, pData[i].GetBuffer(0));
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(long *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\t%ld\r\n"), lpOutLine_StartCharectors, i, pData[i]);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(BYTE *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\t%ld\r\n"), lpOutLine_StartCharectors, i, pData[i]);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(float *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\t%f\r\n"), lpOutLine_StartCharectors, i, pData[i]);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(double *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\t%lf\r\n"), lpOutLine_StartCharectors, i, pData[i]);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(CPoint *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\tx=%d, y=%d\r\n"), lpOutLine_StartCharectors, i, pData[i].x, pData[i].y);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(IV_dataPoint *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\tx=%4.10f, y=%4.10f\r\n"), lpOutLine_StartCharectors, i, pData[i].fx, pData[i].fy);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(CRect *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		if(pData[i].Width() > 0 && pData[i].Height() > 0)
		{
			strTemp.Format(_T("%s[%ld]\tleft=%d, top=%d, right=%d, bottom=%d\r\n"), lpOutLine_StartCharectors, i, pData[i].left, pData[i].top, pData[i].right, pData[i].bottom);
			strOutput += strTemp;
		}
		else
		{
			strTemp.Format(_T("%s[%ld]\tRect Empty\r\n"), lpOutLine_StartCharectors, i);
			strOutput += strTemp;
		}
	}
}

void CResultDrawHelper::Update_DATA2String(IV_dataQuadrangle *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\tLT(%d,%d), RT(%d,%d), BR(%d,%d), BL(%d,%d)\r\n"),
		               lpOutLine_StartCharectors,
		               i,
		               pData[i].m_fLTx, pData[i].m_fLTy,
		               pData[i].m_fRTx, pData[i].m_fRTy,
		               pData[i].m_fRBx, pData[i].m_fRBy,
		               pData[i].m_fLBx, pData[i].m_fLBy);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(IV_dataBlobInfo *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\tRect(left=%d, top=%d, right=%d, bottom=%d), MassCenter(%.2f, %.2f), Area(%d), fArea(%.2f) Label(%d)\r\n"), lpOutLine_StartCharectors, i, pData[i].m_rtROI.left, pData[i].m_rtROI.top, pData[i].m_rtROI.right, pData[i].m_rtROI.bottom, pData[i].m_fCenX, pData[i].m_fCenY, pData[i].m_nArea, pData[i].m_fArea, pData[i].m_nLabel);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(IV_dataCircle *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\tCenX=%4.10f, CenY=%4.10f, Radius=%4.10f\r\n"), lpOutLine_StartCharectors, i, pData[i].m_fCenX, pData[i].m_fCenY, pData[i].m_fRadius);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(IV_dataPolygon *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp, strTemp2;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%d]\t"), lpOutLine_StartCharectors, i);
		long			count	= pData[i].GetVertexNum();
		IV_dataPoint	*pt		= pData[i].GetPoint();

		for(int j = 0; j < count; j++)
		{
			strTemp2.Format(_T("pt%d(%.1f, %.1f), "), j, pt[j].fx, pt[j].fy);
			strTemp += strTemp2;
		}
		strTemp += _T("\r\n");

		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(IV_dataLineSeg *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\tPoint Start(%4.10f, %4.10f), Point End(%4.10f, RadiusY)\r\n"), lpOutLine_StartCharectors, i, pData[i].m_sx, pData[i].m_sy, pData[i].m_ex, pData[i].m_ey);
		strOutput += strTemp;
	}
}

void CResultDrawHelper::Update_DATA2String(IV_dataRectangle *pData, int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors)
{
	if(!pData)
		return;

	CString strTemp;
	for(int i = 0; i < nNum; i++)
	{
		strTemp.Format(_T("%s[%ld]\tCenX=%4.10f, CenY=%4.10f, Width=%4.10f, Height=%4.10f, Angle=%4.10f\r\n"), lpOutLine_StartCharectors, i, pData[i].m_fCenX, pData[i].m_fCenY, pData[i].m_fWidth, pData[i].m_fHeight, pData[i].m_fAngle);
		strOutput += strTemp;
	}
}
   