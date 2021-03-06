﻿#pragma once

#include "iVisionLib.h"

class CImageWindow;

class CResultDrawHelper
{
public:
	static void Update_DATA2ImageWindow(const CPoint *pData,					int nNum, COLORREF color, CImageWindow *pImageWindow, BOOL bCross = FALSE);
	static void Update_DATA2ImageWindow(const IV_dataPoint *pData,				int nNum, COLORREF color, CImageWindow *pImageWindow, BOOL bCross = FALSE);
	static void Update_DATA2ImageWindow(const CRect *pData,						int nNum, COLORREF color, CImageWindow *pImageWindow, BOOL bSmallZoom = FALSE);
	static void Update_DATA2ImageWindow(const IV_dataQuadrangle *pData,			int nNum, COLORREF color, CImageWindow *pImageWindow);
	static void Update_DATA2ImageWindow(const IV_dataImage *pData,				int nNum, COLORREF color, CImageWindow *pImageWindow);
	static void Update_DATA2ImageWindow(const IV_dataBlobInfo *pData,			int nNum, COLORREF color, CImageWindow *pImageWindow);
	static void Update_DATA2ImageWindow(const IV_dataCircle *pData,				int nNum, COLORREF color, CImageWindow *pImageWindow);
	static void Update_DATA2ImageWindow(const IV_dataPolygon *pData,			int nNum, COLORREF color, CImageWindow *pImageWindow);
	static void Update_DATA2ImageWindow(const IV_dataLineEq *pData,				int nNum, COLORREF color, CImageWindow *pImageWindow);
	static void Update_DATA2ImageWindow(const IV_dataLineSeg *pData,			int nNum, COLORREF color, CImageWindow *pImageWindow);
	static void Update_DATA2ImageWindow(const IV_dataRectangle *pData,			int nNum, COLORREF color, CImageWindow *pImageWindow);

	static void Update_DATA2String(IV_dataImage *pData,			int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(CString *pData,				int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(long *pData,					int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(BYTE *pData,					int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(float *pData,				int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(double *pData,				int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(CPoint *pData,				int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(IV_dataPoint *pData,			int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(CRect *pData,				int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(IV_dataQuadrangle *pData,	int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(IV_dataBlobInfo *pData,		int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(IV_dataCircle *pData,		int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(IV_dataPolygon *pData,		int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(IV_dataLineSeg *pData,		int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));
	static void Update_DATA2String(IV_dataRectangle*pData,		int nNum, CString &strOutput, LPCTSTR lpOutLine_StartCharectors = _T(""));

};

   