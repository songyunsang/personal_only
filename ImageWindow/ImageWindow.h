#pragma once

#ifdef __IMAGEWINDOW_WRAPPER_CLASS_EXPORT__
#define __IMAGEWINDOW_WRAPPER_CLASS__	__declspec(dllexport)
#else
#define __IMAGEWINDOW_WRAPPER_CLASS__	__declspec(dllimport)
#endif

class CImageWindow_Impl;
class CImageWindow_Impl2;
class CImageWindowEvt;

struct ROIData;

class __IMAGEWINDOW_WRAPPER_CLASS__ CImageWindow : public CWnd
{
public:
	CImageWindow(CImageWindowEvt *pHandler);
	~CImageWindow();

public:
	void SetEventHandler(CImageWindowEvt *pHandler);
	BOOL Create(CWnd *pwndParent, const CRect rtPosition = CRect(0, 0, 0, 0));
	BOOL Create(CDialog *pwndParent, UINT nFrameControlID = 0);

	BOOL GetImage(const BYTE **ppbuf, long* width, long* height, long* depth, long* widthBytes);

	void SetROIFill( BOOL bFill );
	void OverlayShow(BOOL bShow);
	void OverlaySetLineWidth(short nLineWidth);
	void OverlayAddPoint(const CPoint *pPt, OLE_COLOR crColor);
	void OverlayAddPoints(const CPoint *pPt, long nCount, OLE_COLOR crColor);
	void OverlayAddLine(const CPoint *pPt1, const CPoint *pPt2, OLE_COLOR crColor);
	void OverlayAddRectangle(const CRect *pRect, OLE_COLOR crColor, float rot = 0.f);
	void OverlayAddRectangle(const CPoint *pLT, const CPoint *pRT, const CPoint *pRB, const CPoint *pLB, OLE_COLOR crColor, float rot = 0.f);
	void OverlayAddRectangles(const CRect *pRect, long nCount, OLE_COLOR crColor, float rot = 0.f);
	void OverlayAddFText	(float fX, float fY, LPCTSTR str, OLE_COLOR crColor);
	void OverlayAddText		(const CPoint *pPt, LPCTSTR str, OLE_COLOR crColor);
	void OverlayAddTextEx	(const CPoint *pPt, LPCTSTR str, LOGFONT *pFont, OLE_COLOR crColor);
	void OverlayRemovePoint();
	void OverlayRemoveLine();
	void OverlayRemoveRectangle();
	void OverlayRemoveText();
	void OverlayRemoveAll();
	void OverlayAddLineEx(const CPoint *pPt1, const CPoint *pPt2, OLE_COLOR crColor, long nPenStyle);
	void OverlayAddRectangleEx(const CRect *pRect, OLE_COLOR crColor, long nPenStyle);
	void OverlayAddCircle(float fCenX, float fCenY, float fRadiusX, float fRadiusY, OLE_COLOR crColor, BOOL bScaleDepend);
	void OverlayRemoveCircle();
	void OverlayAddFPoint(float fX, float fY, OLE_COLOR crColor);
	void OverlayAddFLine(float fX1, float fY1, float fX2, float fY2, OLE_COLOR crColor);
	void OverlayAddFRectangle(float fLeft, float fTop, float fRight, float fBottom, OLE_COLOR crColor);
	void OverlayAddArcDoughnut(float fCenX, float fCenY, float fStartAngle, float fEndAngle, float fRadiusX, float fRadiusY, float fWidth, OLE_COLOR crColor);
	void OverlayAddArcRing(float fCenX, float fCenY, float fStartAngle, float fEndAngle, float fInnerLength, float fOuterLength, OLE_COLOR crColor);

	void ExtZoomIn			();
	void ExtZoomOut			();
	void ExtZoomFit			(short nZoomFitMode);
	void ExtZoomScaleGet	(double* pScaleH, double* pScaleV);
	void ExtZoomScale		(double fScaleH, double fScaleV);
	void ExtZoomROIGet		(float *pLeft, float *pTop, float *pRight, float *pBottom);
	void ExtZoomROISet		(float fLeft, float fTop, float fRight, float fBottom);
	void ExtMove			(long nImageCenterX, long nImageCenterY);

	void ExtPosSet			(long nScreenOffsetX, long nScreenOffsetY);
	void ExtPosGet			(long& nScreenOffsetX, long& nScreenOffsetY);

	long ROIAdd(const CRect& rtROI, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable);
	long ROIAdd(const ROIData& Data, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable);
	void ROISetMinSize(long nMinSizeX, long nMinSizeY);
	void ROIRemoveAll();
	BOOL ROIGet(long nID, CRect *prtROI);
	BOOL ROISet(long nID, const ROIData& Data, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable);
	BOOL ROIDel(long nID);
	ROIData ROIGet(long nID) const;
	BOOL ROISel(long nID);
	long ROICount() const;
	long ROIGetCurSel() const;

	void ROIShow();
	void ROIHide();	

	BOOL IsROIEditMode() const;

	void SetDisplayImage(const BYTE* buf, long nWidth, long nHeight, long nWidthBytes, long nDepth);
	void SetDisplayWindow(CWnd* pDialog);
	void SetDisplayImage_Palette(const BYTE * buf, long nWidth, long nHeight, long nWidthBytes, long nDepth, OLE_COLOR* pPaletteColor );

	void EnableDisplayHighQuality(BOOL bEnable);
	void EnableMinimap(BOOL bEnable, CSize size);

	void AddPopupMenuExternal(long nID, LPCTSTR pstrItem);
	void ModifyPopupMenuExternal(long nID, short bEnable, short bChecked);
	void RemovePopupMenuExternal(long nID);

	void EnableMouseTrace(BOOL bEnable);
	void SetTraceAttribute(long nTraceWidth, BYTE r, BYTE g, BYTE b, BYTE a = 255);
	void GetMouseTrace(CPoint** ppVertice, long* pnVertice);

	void SetDefaultZoomFitMode(long nZoomFitMode);		//  1. Width 2. Image 3. Bound

	BOOL SetDisplayRange(BOOL bUse, long nLeft=0, long nTop=0, long nRight=0, long nBottom=0);

	void EnablePixelMeter	(BOOL bEnable);
	void SetPixelMeterUnit	(float fOnePixelUnit=1.f, LPCTSTR lpszUnitName=NULL);
	void SetPixelMeterBegin	(const CPoint& ptBegin);
	void SetPixelMeterEnd	(const CPoint& ptEnd);

	void ShowToolBar(BOOL bShow);
	
	BOOL WriteDIB(LPCTSTR pszFileName, HANDLE hDIB);
	HANDLE DDBToDIB(CDC* pDC, CBitmap &Bitmap, CPalette *pPal);

	void Screenshot(BOOL bShotImage, BOOL bShotROI, CBitmap& o_ddb);
	void Screenshot(BOOL bShotImage, BOOL bShotROI, LPCTSTR szFilePath);


private:
	BOOL __Create();

	//}}AFX_DISPATCH

private:
	// Message maps
	//{{AFX_MSG(CImageWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CImageWindow_Impl *m_pImpl;
	CBitmap __ddbScreenshot;
};