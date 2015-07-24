#pragma once
#include <afxmt.h>
#include "FastDrawDC.h"
#include "ImageScale.h"
#include "PixelMeter.h"
#include "ROIEditor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <atlcoll.h>
#include "SWindowImage.h"

#include "CoordinateTransformer.h"

#include "ImageWindow_Define.h"

using namespace std;

class CImageWindowEvt;
class CImageWindowMinimap;
class CMouseTrace;
class CROIDrawBase;
class CROIGeometryBase;
class CROIEditBase;

class CImageWindow_Impl : public CDialog
{
	HBITMAP m_hDIB;
	HBITMAP DIBAlloc(CDC* pDC, long width, long height, long bpp, BOOL bTopToBottom);
	void DIBRelease();

	BYTE*	m_pBitmapInfoBuffer;
	VOID* m_pPixelRef;

	long m_nSizeX;
	long m_nSizeY;
	long m_nBpp;

	long m_nStepX;	


	//////////////////////////////////////////////////////////////////////////
	HBITMAP m_hBitmapDC;
	HCURSOR hCursorPrv;
	ULONG_PTR token;
	
	BOOL IsPixelAccessible(const BYTE *pImagePtr, long nX, long nY, long nStepX)
	{
		// 디버그상태로 연결되어 있을 경우에는 Access Violation 이 뜨겠지만
		// 릴리즈에서는 return FALSE 될 것이다. (아마 =_=)

		return !IsBadReadPtr(pImagePtr + nY * nStepX + nX, 1);
	}

public:
	CImageWindow_Impl(CImageWindowEvt *pHandler, CWnd* pParent = NULL);
	~CImageWindow_Impl();

public:
	void SetEventHandler(CImageWindowEvt *pHandler);
	BOOL GetImage(const BYTE **ppbuf, long* width, long* height, long* depth, long* widthBytes);
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

	long ROIAdd(const CRect& rtROI, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable);
	long ROIAdd(const ROIData& Data, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable);
	void ROISetMinSize(long nMinSizeX, long nMinSizeY);
	void ROIRemoveAll();
	BOOL ROIGet(long nID, CRect* prtROI);
	ROIData ROIGet(long nID) const;
	BOOL ROISet(long nID, const ROIData& Data, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable);
	BOOL ROIDel(long nID);
	BOOL ROISel(long nID);
	long ROICount() const;
	long ROIGetCurSel() const;
	

	void ROIShow();
	void ROIHide();	

	void ExtZoomIn			();
	void ExtZoomOut			();
	void ExtZoomFit			(short nZoomFitMode);
	void ExtZoomScaleGet	(double* pScaleH, double* pScaleV);
	void ExtZoomScale		(double fScaleH, double fScaleV);
	void ExtZoomROIGet		(float *pLeft, float *pTop, float *pRight, float *pBottom);
	void ExtZoomROISet		(float fLeft, float fTop, float fRight, float fBottom);
	void ExtMove			(long nImageCenterX, long nImageCenterY);
	void ExtMove			(float fImageCenterX, float fImageCenterY);

	void ExtPosSet			(long nScreenOffsetX, long nScreenOffsetY);
	void ExtPosGet			(long& nScreenOffsetX, long& nScreenOffsetY);

	void SetDisplayImage(const BYTE* buf, long nWidth, long nHeight, long nWidthBytes, long nDepth);
	void SetDisplayWindow(CWnd* pDialog);
	void SetDisplayImage_Palette(const BYTE* buf, long nWidth, long nHeight, long nWidthBytes, long nDepth, OLE_COLOR* pPaletteColor );

	void EnableDisplayHighQuality(BOOL bEnable);
	void EnableMinimap(BOOL bEnable, CSize size);
	
	void SetDefaultZoomFitMode(long nZoomFitMode);

	BOOL SetDisplayRange(BOOL bUse, long fLeft, long fTop, long fRight, long fBottom);

	void EnablePixelMeter	(BOOL bEnable);
	void SetPixelMeterUnit	(float fOnePixelUnit, LPCTSTR lpszUnitName);
	void SetPixelMeterBegin	(const CPoint& ptBegin);
	void SetPixelMeterEnd	(const CPoint& ptEnd);

	void Screenshot(BOOL bShotImage, BOOL bShotROI, CBitmap& o_ddb);

	void SetUseFill(BOOL bUse )	{ m_bUseFill = bUse ;}

private:
	CBitmap m_BitmapScreenshot;

private:
	void UpdateScrollRange();
	void UpdateScrollShow();

	void __UpdateScrollInfoX();
	void __UpdateScrollInfoY();
	BOOL __IsHScrollAvailable();
	BOOL __IsVScrollAvailable();
	
	
	HWND m_hWndSendClickPos;
	int m_nZoomWheel;
	enumZoomFitMode m_nDefaultZoomFitMode;	// 마우스 휠로 ZoomFit했을 경우

	CPoint m_bDragPrevPos;
	CSize __szRButtonDragDistance;

	BOOL	m_bUseFill;

	int	m_iStretchMode;

	// Minimap 관련
	CImageWindowMinimap*	__pMinimap;
	BOOL					__bMinimapMoveMode;
	BOOL					__bMinimapEnabled;

	// Constructor

	//Mapping transform
	void	TransTracker2ImagePos	(CRect rect, CRect *o_Rect);
	void	TransImagePos2TrackerPos(float fLeft, float fTop, float fRight, float fBottom, CRect *o_Rect);
	CPoint	TransBound2ImagePos		(CPoint i_pt);
	CPoint	TransImagePos2Bound		(float fX, float fY);
	CSize	ImageToScreenDist	(float fDist);

	//---------------------------------
	// ImageWindow 동작여부
	//---------------------------------
	BOOL m_bImageWindowEnable;

	//---------------------------------
	// Display Cross Mark
	//---------------------------------
	BOOL m_bDisplayCrossMark;

	//---------------------------------
	// Navigater
	//---------------------------------
	long m_nMouseControlMode;

	//---------------------------------
	// ROI Tracker
	//---------------------------------
	BOOL m_isROIDraw;
	int m_nROIFontHeight;
	CFont m_ROIFont;
	CSize	m_ROIszMin;    // minimum X and Y size during track operation
	int		m_nSelROI;
	int		m_nROI_Dir;
	int m_nNodeSize;
	BOOL	m_bROIEditMode;

	vector<CROIGeometryBase*>	m_vecROIGeo;
	vector<CROIDrawBase*>		m_vecROIDraw;

	CROIEditBase* m_pROIEditor;

	void uDrawROIs(CDC* pDC);
	void uDrawROI(CDC* pDC, const CROIDrawBase* pROIDraw);

	void uDrawROIEditor(CDC* pDC, CROIEditBase* pROIEdit);

	CROIGeometryBase* GetROIGeometry(long nIndex);


	long m_nSelKnob;

	void	uDraw9Node( CDC &dc, const CRect &rt, int nSize );
	void	uDrawCurrentPosition( CDC *pdc );
	void	uDrawPixelMeter( CDC* pdc );
	void	uDrawMinimap( CDC* pdc, const CRect& rt);

	void	uDrawPixelTrace(CDC* pdc);
	void	uDrawPixelDistance(CDC* pdc);

private:	// PIXELMETER
	CPixelMeter  m_PixelMeter;

public:
	BOOL	IsROIEditMode() const;

	int		FindROI(const CPoint& pos);
	int		GetMinimumDistance( CPoint cp, const EditROI& roi );
	int		GetDirection( CPoint cp, CRect &rt, BOOL bSizeFixed, BOOL bPosFixed );
	void	ROICursorChange( int nROI_Dir );
	void	ROIEditMode(int nROI_Dir);
	void	uUpdateROI( int nSelected, CPoint cp );


	__forceinline void  uDrawNode(CDC &dc, int nNodeX, int nNodeY, int nSize)
	{
		uDrawRect(&dc, CRect(nNodeX-nSize, nNodeY-nSize, nNodeX+nSize, nNodeY+nSize));
	}

	//---------------------------------
	//  Overlay Function Begin
	//---------------------------------
	//	int  m_nRadius;			// Circle Overlay를 위해서 추가.
	//	BOOL m_bScaleDepend;	// Circle Overlay를 위해서 추가.

	BOOL m_isOverlayDraw;
	int m_nTextFontHeight;
	CFont m_TextFont;
	CFont *m_pDefaultFont;   
	CRect m_rtBound;
	void uDrawLine(CDC *pdc,CPoint pt1,CPoint pt2, COLORREF crColor);
	void uDrawRect(CDC *pdc,CRect rect);
	void uDrawText(CDC *pdc,float fX, float fY, LPCTSTR str);
	void uDrawRectFill(CDC *pdc,CRect rect, COLORREF clr );

	void uDrawPoly(CDC *pdc,const vector<CPoint>& vecPoint);
	void uDrawPolyFill(CDC *pdc,const vector<CPoint>& vecPoint, COLORREF clr);

	void uOverlayDrawAll(CDC *pDC, BOOL bDrawOverlay, BOOL bDrawCrossLine);
	void uOverlayDrawPoint(CFastDrawDC &dc, std::vector<Overlay::Point>& ArsPt);
	void uOverlayDrawLine(CFastDrawDC &dc, std::vector<Overlay::Line>& ArsLine);
	void uOverlayDrawRect(CFastDrawDC &dc, std::vector<Overlay::Rect>& ArsRect);
	void uOverlayDrawEllipse(CFastDrawDC &dc, std::vector<Overlay::Ellipse>& ArsEllipse);
	void uOverlayDrawArcDoughnut(CFastDrawDC &dc, std::vector<Overlay::ArcDoughnut>& ArsArcDoughnut);
	void uOverlayDrawArcRing(CFastDrawDC &dc, std::vector<Overlay::ArcRing>& ArsArcRing);
	void uOverlayDrawText(CDC *pDC, std::vector<Overlay::Text>& ArsText);

    // Atl Array declare 
	int m_nOverlayLineWidth;
	std::vector<Overlay::Point>			m_vecPoint;
	std::vector<Overlay::Line>			m_vecLine;
	std::vector<Overlay::Rect>			m_vecRect;
	std::vector<Overlay::Text>			m_vecText;
	std::vector<Overlay::Ellipse>		m_vecEllipse;
	std::vector<Overlay::ArcDoughnut>	m_vecArcDoughnut;
	std::vector<Overlay::ArcRing>		m_vecArcRing;


	void CreateFont(int nTextFontHeight);
//	void CreateFont(CFont &font, int nTextFontHeight);

	//==============================
	// 팝업메뉴 확장 관련
	//==============================
private:
	CPoint				__ptContextMenuPos;
	map<long,CString>	__mapExpandMenu;
	map<long, DWORD>	__mapExpandMenuOption;
	void	__AppendMenuExternal(CMenu* pMenu);
	void	__DeleteMenuExternal(CMenu* pMenu);

public:
	void	CmdRouteMenu(CWnd* pWnd,CMenu* pPopupMenu);

	void	AddPopupMenuExternal(long nID, LPCTSTR pstrItem);
	void	ModifyPopupMenuExternal(long nID, short bEnable, short bChecked);
	void	RemovePopupMenuExternal(long nID);

	// 팝업메뉴 UI 관련
private:
	void	__SetCheckPopupMenu(CMenu *pMenu);
	void	__SetCheckShowToolBarPopupMenu(CMenu* pMenu);
	void	__SetCheckShowMiniMapPopupMenu(CMenu* pMenu);
	void	__SetCheckZoomFitModePopupMenu(CMenu* pMenu);
	void	__SetCheckShowPixelMeterPopupMenu(CMenu* pMenu);

	UINT	__GetZoomFitModeID(enumZoomFitMode nZoomFitMode);


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageWindow_Impl)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

private:
	void FastDrawDCReady(CDC* pDC);
	void FastDrawDCExcute();

	CCriticalSection __CSFastDrawDC;
	CFastDrawDC		m_FastDC;

public:	
	SWindow_Image	m_imgShowSelImage;
	CWnd*			m_pChildWindow;
	
	SWindow_Image	m_imgOriginalImage;

	SWindow_Image	m_imgSampledImage;
	
	// 일부 범위만 보기 모드
	BOOL			m_bUseRangeImage;
	CRect			m_rtRangeImage;
	SWindow_Image	m_imgRangeImage;

	COLORREF*		m_pPaletteColor;

	CImageWindowEvt	*m_pEventHandler;

	void GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos);

private: 
	void __UpdateScale();
	void __UpdateDisplayInformation();

private:
	float m_fScaleX;
	float m_fScaleY;
	CPoint m_ptScreenOffset;
	CSize m_szImage;
	CSize m_szScreen;
	CSize m_szScroll;
	CRect m_rtDIB;
	CRect m_rtDC;

public:
	// Client - 툴바영역포함(UI관련), Screen - 툴바영역제외(그리기관련)
	CCoordinateTransformer m_CoordTransform;
	CPoint ScreenToImage(CPoint point);
	CPoint ImageToScreen(CPoint point);
	CPoint ClientToImage(CPoint point);
	CPoint ImageToClient(CPoint point);

	CPoint ImageToScreen(float x, float y);
	CPoint ImageToClient(float x, float y);

	CRect ScreenToImageRect(const CRect& rt);
	CRect ImageToScreenRect(const CRect& rt);
	CRect ImageToScreenRect(float fLeft, float fTop, float fRight, float fBottom);	// Closed Range
	CRect ClientToImageRect(const CRect& rt);
	CRect ImageToClientRect(const CRect& rt);

	long ScreenToImageCX(float cx)	{	return (long)(cx / m_fScaleX);	}
	long ScreenToImageCY(float cy)	{	return (long)(cy / m_fScaleY);	}
	long ImageToScreenCX(float cx)	{	return (long)(cx * m_fScaleX);	}
	long ImageToScreenCY(float cy)	{	return (long)(cy * m_fScaleY);	}

private:	// TRACE
	CCriticalSection __csDrawImage;
	void	__DrawImage(CDC* pDC);

	HBITMAP __MakeDDB(CDC* pDC, const unsigned char *imgdata, long width, long height, long widthBytes, int bpp, BOOL bTopToBottom = TRUE, OLE_COLOR *pPaletteColor = NULL);
	void	__DrawTrace(CDC* pMemDC, CMouseTrace* pMouseTrace);

private:
	void	__TraceBegin(const CPoint& ptImage);
	CRect	__Trace(const CPoint& ptImage);
	void	__TraceEnd();

	void	__TraceExport();

	BOOL	__IsTraceEnable()	{	return m_bEnableMouseTrace;	}

public:
	void	EnableMouseTrace(BOOL bEnable)	{	m_bEnableMouseTrace = bEnable;	}

	void	SetTraceColor(BYTE a, BYTE r, BYTE g, BYTE b);
	void	SetTraceWidth(int nTraceWidth);

	void	GetMouseTrace(CPoint** ppVertice, long* pnVertice);

// ZOOMFIT
private:
	void	__ZoomFitWidth();
	void	__ZoomFitImage();
	void	__ZoomFitBound();
	BOOL	__Zoom(long nDiffIndex, CPoint point);
	BOOL	__Zoom(float fScaleX, float fScaleY, CPoint point);

	float	__GetScaleWidthFit();
	float	__GetScaleHeightFit();
	float	__GetScaleImageFit();

	CSize	__GetScreenSize();
	CSize	__GetImageSize();

	long	__GetScreenOffsetX(float fScreenX, float fImageX)	{	return (long)(fScreenX - fImageX * m_fScaleX);	}
	long	__GetScreenOffsetY(float fScreenY, float fImageY)	{	return (long)(fScreenY - fImageY * m_fScaleY);	}
	
	BOOL __IsImageSizeUnderScreen();
	long __FindZoomScaleIndex(float fScale);

public:
	void __Scroll(long nScrollPosX, long nScrollPosY);

private:
	void __ScrollLimit(long &nScrollPosX, long &nScrollPosY);

private:
	BOOL m_bEnableMouseTrace;

	CMouseTrace* m_pMouseTrace;

	Color m_argbTrace;
	long m_nTraceWidth;

	vector<CPoint> m_PointArrayExport;
//<---

private:
	CToolBar		m_wndToolBar;
	CToolTipCtrl	m_wndToolTip;

	BOOL m_bScrollRangeUpdating;

private:
	BOOL __CreateToolBar();
	void __MoveView(int x, int y, int cx, int cy);
	BOOL __IsScrollRangeUpdating();
	void __SetScrollRangeUpdating(BOOL bUpdating);


	void __ConvertViewToImagePos(long ptx, long pty, float& fX, float& fY);

	BOOL __OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	BOOL __OnMouseMove(UINT nFlags, CPoint point);
	BOOL __OnNavigate(UINT nFlags, CPoint point);
	BOOL __OnDisplayPosition(UINT nFlags, CPoint point);

	BOOL __OnDrag(UINT nFlags, CPoint point);
	BOOL __OnMinimapDrag(UINT nFlags, CPoint point);
	BOOL __OnLButtonDrag(UINT nFlags, CPoint point);
	BOOL __OnRButtonDrag(UINT nFlags, CPoint point);
	
	BOOL __IsMinimapShowing();
	BOOL __IsMinimapEnabled();

	BOOL __UpdateMinimapPostiion();
	CString __MakeImagePositionString(CPoint point);

	BOOL IsImageValid() const;

public:
	void	ShowToolBar(BOOL bShow);
	int		GetToolBarSizeY();


	// Implementation

	// 대화 상자 데이터입니다.
public:
	enum { IDD = IDD_IMAGEWINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

protected:

	// Message maps
	//{{AFX_MSG(CImageWindow_Impl)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnZoomin();
	afx_msg void OnZoomout();
	afx_msg void OnZoom1x();
	afx_msg void OnZoomwidthfit();
	afx_msg void OnZoomimagefit();
	afx_msg void OnZoomboundfit();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Event maps
	BOOL FireEventLButtonDblClk	(UINT nFlags,long ptx, long pty);
	BOOL FireEventLButtonUp		(UINT nFlags,long ptx, long pty);
	BOOL FireEventLButtonDown	(UINT nFlags,long ptx, long pty, BOOL bMinimapHit);
	BOOL FireEventMouseMove		(UINT nFlags,long ptx, long pty);
	BOOL FireEventRButtonUp		(UINT nFlags,long ptx, long pty, BOOL bOnContextMenuCall);
	BOOL FireEventRButtonDown	(UINT nFlags,long ptx, long pty);
	void FireEventROIChanged	(long nROIID);
	void FireEventExtPopupMenu	(long nCmdID, double fx, double fy);
	BOOL FireEventKeyEvent		(MSG* pMsg);
	void FireEventOnScroll		();
	void FireEventOnZoom		();

	virtual void OnOK()		{}
	virtual void OnCancel()	{}

public:
	afx_msg void OnDisplaycurrentpos();
	afx_msg void OnCrossmark();
	afx_msg void OnUpdateCrossmark(CCmdUI *pCmdUI);
	afx_msg void OnZoom2x();
	afx_msg void OnZoom5x();
	afx_msg void OnZoom10x();
	afx_msg void OnMove();
	afx_msg void OnRemovealloverlay();
	afx_msg void OnUpdateRemovealloverlay(CCmdUI *pCmdUI);
	afx_msg void OnShowtoolbar();
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPopupShowminimap();
	afx_msg void OnPopupShowpixelmeter();
	afx_msg void OnExportFile();
	afx_msg void OnExportClipboard();
	afx_msg void OnPopupScreenshot();
	afx_msg void OnUpdatePopupScreenshot(CCmdUI *pCmdUI);
	afx_msg void OnUpdateExportFile(CCmdUI *pCmdUI);
	afx_msg void OnUpdateExportClipboard(CCmdUI *pCmdUI);
};