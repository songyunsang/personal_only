#include "stdafx.h"

#include "ROIDrawBase.h"
#include "ROIDrawRect.h"
#include "ROIDrawPolygon.h"
#include "ROIDrawRectRounded.h"

#include "ROIEditBase.h"
#include "ROIEditFactory.h"

#include "ImageWindowApp.h"
#include "ImageWindow_Impl2.h"
#include "ImageWindowEvt.h"
#include "ImageWindowMinimap.h"
#include "MouseTrace.h"

#include "../ImageFileFormat/ImageFileFormat.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CImageWindow_Impl2, CDialog)
	//{{AFX_MSG_MAP(CImageWindow_Impl2)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ZOOMIN, OnZoomin)
	ON_COMMAND(ID_ZOOMOUT, OnZoomout)
	ON_COMMAND(ID_ZOOMNORMAL, OnZoom1x)
	ON_COMMAND(ID_ZOOMWIDTHFIT, OnZoomwidthfit)
	ON_COMMAND(ID_ZOOMIMAGEFIT, OnZoomimagefit)
	ON_COMMAND(ID_ZOOMBOUNDFIT, OnZoomboundfit)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_MOVE, OnMove)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_DISPLAYCURRENTPOS, &CImageWindow_Impl2::OnDisplaycurrentpos)
	ON_COMMAND(ID_CROSSMARK, &CImageWindow_Impl2::OnCrossmark)
	ON_UPDATE_COMMAND_UI(ID_CROSSMARK, &CImageWindow_Impl2::OnUpdateCrossmark)
	ON_COMMAND(ID_POPUP_ZOOM2X, &CImageWindow_Impl2::OnZoom2x)
	ON_COMMAND(ID_POPUP_ZOOM5X, &CImageWindow_Impl2::OnZoom5x)
	ON_COMMAND(ID_POPUP_ZOOM10X, &CImageWindow_Impl2::OnZoom10x)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_REMOVEALLOVERLAY, &CImageWindow_Impl2::OnRemovealloverlay)
	ON_UPDATE_COMMAND_UI(ID_REMOVEALLOVERLAY, &CImageWindow_Impl2::OnUpdateRemovealloverlay)
	ON_COMMAND(ID_POPUP_SHOWTOOLBAR, &CImageWindow_Impl2::OnShowtoolbar)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, &CImageWindow_Impl2::OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, &CImageWindow_Impl2::OnToolTipText)
	ON_COMMAND(ID_POPUP_SHOWMINIMAP, &CImageWindow_Impl2::OnPopupShowminimap)
	ON_COMMAND(ID_POPUP_SHOWPIXELMETER, &CImageWindow_Impl2::OnPopupShowpixelmeter)
	ON_COMMAND(ID_EXPORT_FILE, &CImageWindow_Impl2::OnExportFile)
	ON_COMMAND(ID_EXPORT_CLIPBOARD, &CImageWindow_Impl2::OnExportClipboard)
	ON_COMMAND(ID_POPUP_SCREENSHOT, &CImageWindow_Impl2::OnPopupScreenshot)
	ON_UPDATE_COMMAND_UI(ID_POPUP_SCREENSHOT, &CImageWindow_Impl2::OnUpdatePopupScreenshot)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_FILE,	&CImageWindow_Impl2::OnUpdateExportFile)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_CLIPBOARD, &CImageWindow_Impl2::OnUpdateExportClipboard)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map



CImageWindow_Impl2::CImageWindow_Impl2(CImageWindowEvt *pHandler, CWnd *pParent)
	: CDialog(CImageWindow_Impl2::IDD, pParent)
	, m_pEventHandler(pHandler)
	, __pMinimap(new CImageWindowMinimap)
	, __bMinimapMoveMode(FALSE)
	, __bMinimapEnabled(FALSE)
	, m_isROIDraw(FALSE)
	, m_nZoomWheel(3)
	, m_nMouseControlMode(0)
	, m_bDisplayCrossMark(FALSE)
	, m_isOverlayDraw(FALSE)
	, m_nOverlayLineWidth(2)
	, m_bImageWindowEnable(TRUE)
	, m_iStretchMode(COLORONCOLOR)
	, m_bEnableMouseTrace(FALSE)
	, m_nTraceWidth(1)
	, m_bUseRangeImage(FALSE)
	, m_rtRangeImage(0, 0, 0, 0)
	, __szRButtonDragDistance(0, 0)
	, m_PixelMeter(&m_CoordTransform)
	, m_bScrollRangeUpdating(FALSE)
	, m_bUseFill(FALSE)
	, m_bROIEditMode(FALSE)
{
	GdiplusStartupInput gdiInput;
	GdiplusStartup(&token, &gdiInput, NULL);

	m_rtBound.top		= 500;
	m_rtBound.left		= 500;
	m_rtBound.right		= 1500;
	m_rtBound.bottom	= 1500;

	/////////////////////////////////////////
	//image buffer initialze
	m_pPaletteColor		= nullptr;
	m_pDefaultFont		= nullptr;
	m_pChildWindow		= nullptr;

	// 20110711 윤상추가 : Mouse Trace Mode 추가
	m_pMouseTrace = new CMouseTrace;
	m_argbTrace	  = Color(192, 0, 255, 255);
	m_nDefaultZoomFitMode = ZOOMFIT_MODE_IMAGE;

	m_fScaleX			= 1.f;
	m_fScaleY			= 1.f;
	m_ptScreenOffset	= CPoint(0, 0);
	m_szImage			= CSize(0, 0);
	m_szScreen			= CSize(0, 0);
	m_szScroll			= CSize(0, 0);

	m_pROIEditor		= nullptr;
	m_nSelROI			= -1;
	m_nSelKnob			= KNOB_NOSELECT;
}


/////////////////////////////////////////////////////////////////////////////
// CImageWindow_Impl2::~CImageWindow_Impl2 - Destructor

CImageWindow_Impl2::~CImageWindow_Impl2()
{
	safeReleaseVector(m_vecROIDraw);
	safeReleaseVector(m_vecROIGeo);

	safeRelease(m_pROIEditor);

	GdiplusShutdown(token);

	if(m_pPaletteColor)
	{
		delete []m_pPaletteColor;
		m_pPaletteColor = nullptr;
	}

	if(__pMinimap)
	{
		delete __pMinimap;
		__pMinimap = nullptr;
	}

	if(m_pMouseTrace)
	{
		delete m_pMouseTrace;
		m_pMouseTrace = nullptr;
	}

	if(m_imgRangeImage.pbyImage)
	{
		delete []m_imgRangeImage.pbyImage;
		m_imgRangeImage.pbyImage = nullptr;
	}

	delete [] m_imgSampledImage.pbyImage;
}

void CImageWindow_Impl2::SetEventHandler(CImageWindowEvt *pHandler)
{
	m_pEventHandler = pHandler;
}

BOOL CImageWindow_Impl2::FireEventLButtonDblClk(UINT nFlags, long ptx, long pty)
{
	if(m_pEventHandler == NULL)	return FALSE;

	float fX = 0.f, fY = 0.f;

	__ConvertViewToImagePos(ptx, pty, fX, fY);

	return m_pEventHandler->OnLButtonDblClk(nFlags, ptx, pty, fX, fY);
}

BOOL CImageWindow_Impl2::FireEventLButtonUp(UINT nFlags, long ptx, long pty)
{
	if(m_pEventHandler == NULL)	return FALSE;

	float fX = 0.f, fY = 0.f;

	__ConvertViewToImagePos(ptx, pty, fX, fY);

	return m_pEventHandler->OnLButtonUp(nFlags, ptx, pty, fX, fY);
}

BOOL CImageWindow_Impl2::FireEventLButtonDown(UINT nFlags, long ptx, long pty, BOOL bMinimapHit)
{
	if(m_pEventHandler == NULL)	return FALSE;

	float fX = 0.f, fY = 0.f;

	__ConvertViewToImagePos(ptx, pty, fX, fY);

	return m_pEventHandler->OnLButtonDown(nFlags, ptx, pty, fX, fY, bMinimapHit);
}

BOOL CImageWindow_Impl2::FireEventMouseMove(UINT nFlags, long ptx, long pty)
{
	if(m_pEventHandler == NULL)	return FALSE;

	float fX = 0.f, fY = 0.f;

	__ConvertViewToImagePos(ptx, pty, fX, fY);

	return m_pEventHandler->OnMouseMove(nFlags, ptx, pty, fX, fY);
}

BOOL CImageWindow_Impl2::FireEventRButtonUp(UINT nFlags, long ptx, long pty, BOOL bOnContextMenuCall)
{
	if(m_pEventHandler == NULL)	return FALSE;

	float fX = 0.f, fY = 0.f;

	__ConvertViewToImagePos(ptx, pty, fX, fY);

	return m_pEventHandler->OnRButtonUp(nFlags, ptx, pty, fX, fY, bOnContextMenuCall);
}

BOOL CImageWindow_Impl2::FireEventRButtonDown(UINT nFlags, long ptx, long pty)
{
	if(m_pEventHandler == NULL)	return FALSE;

	float fX = 0.f, fY = 0.f;

	__ConvertViewToImagePos(ptx, pty, fX, fY);

	return m_pEventHandler->OnRButtonDown(nFlags, ptx, pty, fX, fY);
}

void CImageWindow_Impl2::FireEventROIChanged(long nROIID)
{
	if(m_pEventHandler == NULL)	return;

	m_pEventHandler->OnROIChanged(nROIID);
}

void CImageWindow_Impl2::FireEventExtPopupMenu(long nCmdID, double fx, double fy)
{
	if(m_pEventHandler == NULL)	return;

	m_pEventHandler->OnExtPopupMenu(nCmdID, fx, fy);
}

BOOL CImageWindow_Impl2::FireEventKeyEvent(MSG *pMsg)
{
	if(WM_KEYFIRST > pMsg->message || pMsg->message > WM_KEYLAST) return FALSE;

	if(m_pEventHandler == NULL) return FALSE;

	return m_pEventHandler->OnKeyEvent(pMsg);
}

void CImageWindow_Impl2::__ConvertViewToImagePos(long ptx, long pty, float &fX, float &fY)
{
	pty -= GetToolBarSizeY();

	//CPoint ptImage = ScreenToImage(CPoint(ptx, pty));	대신 아래 코드 사용
	fX = (ptx - m_ptScreenOffset.x) / m_fScaleX;
	fY = (pty - m_ptScreenOffset.y) / m_fScaleY;
}

BOOL CImageWindow_Impl2::__OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	if( m_szImage.cx <= 0 || m_szImage.cy <= 0 ) return FALSE;

	return __Zoom(zDelta / WHEEL_DELTA, point);
}

BOOL CImageWindow_Impl2::__OnMouseMove(UINT nFlags, CPoint point)
{
	BOOL bInvalidate = FALSE;

	bInvalidate |= __OnNavigate(nFlags, point);
	bInvalidate |= __OnDisplayPosition(nFlags, point);

	return bInvalidate;
}

BOOL CImageWindow_Impl2::__OnNavigate(UINT nFlags, CPoint point)
{
	if( m_nMouseControlMode != M_MODE_NAVIGATION )	return FALSE;
	if( !m_szScreen.cx || !m_szScreen.cy )			return FALSE;

	long nImagePosX = (long)(point.x / (float)m_szScreen.cx * m_szImage.cx);
	long nImagePosY = (long)(point.y / (float)m_szScreen.cy * m_szImage.cy);

	ExtMove(nImagePosX, nImagePosY);

	return TRUE;
}

BOOL CImageWindow_Impl2::__OnDisplayPosition(UINT nFlags, CPoint point)
{
	if( m_nMouseControlMode != M_MODE_DISPLAY_POSITION ) return FALSE;

	return TRUE;
}

BOOL CImageWindow_Impl2::__OnDrag(UINT nFlags, CPoint point)
{
	BOOL bInvalidate = FALSE;

	bInvalidate |= __OnLButtonDrag(nFlags, point);
	bInvalidate |= __OnRButtonDrag(nFlags, point);
	bInvalidate |= __OnMinimapDrag(nFlags, point);

	return bInvalidate;
}

BOOL CImageWindow_Impl2::__OnLButtonDrag(UINT nFlags, CPoint point)
{
	if( (nFlags & MK_LBUTTON ) != MK_LBUTTON ) return FALSE;

	SetPixelMeterEnd(ClientToImage(point));

	return TRUE;
}

BOOL CImageWindow_Impl2::__OnRButtonDrag(UINT nFlags, CPoint point)
{
	if( (nFlags & MK_RBUTTON ) != MK_RBUTTON ) return FALSE;

	long dx(m_bDragPrevPos.x - point.x);
	long dy(m_bDragPrevPos.y - point.y);

	if( dx == 0 && dy == 0 ) return FALSE;

	__Scroll(-(m_ptScreenOffset.x - dx), -(m_ptScreenOffset.y - dy));

	__szRButtonDragDistance += CSize(abs(dx), abs(dy));

	m_bDragPrevPos = point;

	return TRUE;
}

BOOL CImageWindow_Impl2::__OnMinimapDrag(UINT nFlags, CPoint point)
{
	if( m_nMouseControlMode == M_MODE_NAVIGATION ) return FALSE;	// Navigation 기능과 미니맵 드래그는 동일한 기능, 동시 동작 될 이유없음

	if( ( nFlags & MK_LBUTTON ) != MK_LBUTTON ) return FALSE;

	if( !__IsMinimapEnabled() || !__IsMinimapShowing() ) return FALSE;

	CPoint ptDest;
	CPoint ptHitTest(point.x, point.y - GetToolBarSizeY());
	if(__pMinimap->GetPosition(ptHitTest, ptDest) && __bMinimapMoveMode)
	{
		ExtMove(ptDest.x, ptDest.y);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CImageWindow_Impl2::OnDraw - Drawing function
#define RECTWIDTH(x)	(x).Width()
#define RECTHEIGHT(x)	(x).Height()

void CImageWindow_Impl2::OnPaint()
{
	CRect rcBounds(0, 0, 0, 0);
	this->GetClientRect(&rcBounds);

	rcBounds.top = GetToolBarSizeY();

	if(rcBounds.Width() <= 0 || rcBounds.Height() <= 0)	return;

	CRect rtBounds(0, 0, rcBounds.Width(), rcBounds.Height());

	CPaintDC dc(this);

	if(!m_imgShowSelImage.pbyImage)
	{
		//-------------------
		// 까맣게 칠해준다.
		//-------------------
		CBrush bru, *pOldbru;
		bru.CreateSolidBrush(RGB(0, 0, 0));
		pOldbru = dc.SelectObject(&bru);
		dc.Rectangle(rcBounds);
		dc.SelectObject(pOldbru);
		bru.DeleteObject();
	}
	else
	{
		CDC MemDC;
		CBitmap Bitmap;
		MemDC.CreateCompatibleDC(&dc);
		Bitmap.CreateCompatibleBitmap(&dc, rtBounds.Width(), rtBounds.Height());
		CBitmap *pOldBitmap = MemDC.SelectObject(&Bitmap);

		CBrush brush;
		brush.CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 255));
		SetBkColor(MemDC.GetSafeHdc(), RGB(128, 128, 128));
		MemDC.FillRect(rtBounds, &brush);

		__DrawImage(&MemDC);

		uOverlayDrawAll( &MemDC, m_isOverlayDraw, m_bDisplayCrossMark );
		uDrawROIs( &MemDC );
		uDrawROIEditor(&MemDC, m_pROIEditor);

		uDrawCurrentPosition( &MemDC );
		uDrawPixelMeter(&MemDC);
		uDrawMinimap(&MemDC, rtBounds);

		dc.BitBlt(0, rcBounds.top, rcBounds.Width(), rcBounds.Height(), &MemDC, 0, 0, SRCCOPY);
		MemDC.SelectObject(pOldBitmap);

		Bitmap	.DeleteObject();
		MemDC	.DeleteDC();
	}
}

BOOL CImageWindow_Impl2::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= WS_HSCROLL | WS_VSCROLL ;	// Enable word-wrapping

	return CWnd::PreCreateWindow(cs);
}


int CImageWindow_Impl2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	CreateFont(TEXT_FONTSIZE);

	return 0;
}

void CImageWindow_Impl2::GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos)
{
	switch(nBar)
	{
	case SB_VERT:
		if(GetStyle() & WS_VSCROLL)
		{
			// 수직 스크롤러가 존재
			return CWnd::GetScrollRange(nBar, lpMinPos, lpMaxPos);
		}
		break;

	case SB_HORZ:
		if(GetStyle() & WS_HSCROLL)
		{
			// 수평 스크롤러가 존재
			return CWnd::GetScrollRange(nBar, lpMinPos, lpMaxPos);
		}
		break;
	}

	*lpMinPos = 0;
	*lpMaxPos = 0;
}

void CImageWindow_Impl2::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int nNext, nMin, nMax;
	GetScrollRange(SB_VERT, &nMin, &nMax);

	CRect rtWin(0, 0, 0, 0);
	::GetClientRect(m_hWnd, &rtWin);

	int nStep		= (nMax - nMin + rtWin.Height()) / m_imgShowSelImage.nHeight;
	int nPageStep	= (nMax - nMin + rtWin.Height()) / m_imgShowSelImage.nHeight * 10;
	nStep		= max(1, nStep);
	nPageStep	= max(1, nPageStep);

	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
	{
		SCROLLINFO sif;
		sif.cbSize = sizeof(SCROLLINFO);
		sif.fMask = SIF_POS;
		GetScrollInfo(SB_VERT, &sif);
		nNext = sif.nTrackPos;
		if(nNext > nMax) nNext = nMax;
		else if(nNext < nMin) nNext = nMin ;
		::SetScrollPos(m_hWnd, SB_VERT, nNext, TRUE);
	}
	break;
	case SB_LINEDOWN:
		nNext = ::GetScrollPos(m_hWnd, SB_VERT) + nStep;
		if(nNext > nMax) break;
		::SetScrollPos(m_hWnd, SB_VERT, nNext, TRUE);
		break;

	case SB_LINEUP:
		nNext = ::GetScrollPos(m_hWnd, SB_VERT) - nStep;
		if(nNext < nMin) break;
		::SetScrollPos(m_hWnd, SB_VERT, nNext, TRUE);
		break;

	case SB_PAGEDOWN:
		nNext = ::GetScrollPos(m_hWnd, SB_VERT);
		if(nNext + nPageStep <= nMax)
		{
			::SetScrollPos(m_hWnd, SB_VERT, nNext + nPageStep, TRUE);
			nNext += nPageStep;
		}
		else
		{
			nNext = nMax;
			::SetScrollPos(m_hWnd, SB_VERT, nNext, TRUE);
		}
		break;
	case SB_PAGEUP:
		nNext = ::GetScrollPos(m_hWnd, SB_VERT);
		if(nNext - nPageStep >= nMin)
		{
			::SetScrollPos(m_hWnd, SB_VERT, nNext - nPageStep, TRUE);
			nNext -= nPageStep;
		}
		else
		{
			nNext = nMin;
			::SetScrollPos(m_hWnd, SB_VERT, nNext, TRUE);
		}
		break;
	default:
		return;
		break;
	}


	m_ptScreenOffset.y = -::GetScrollPos(m_hWnd, SB_VERT);

	__UpdateMinimapPostiion();
	__UpdateDisplayInformation();

	Invalidate(FALSE);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CImageWindow_Impl2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	int nNext, nMin, nMax;
	GetScrollRange(SB_HORZ, &nMin, &nMax);

	CRect rtWin(0, 0, 0, 0);
	::GetClientRect(m_hWnd, &rtWin);

	int nStep		= (nMax - nMin + rtWin.Width()) / m_imgShowSelImage.nWidth;
	int nPageStep	= (nMax - nMin + rtWin.Width()) / m_imgShowSelImage.nWidth * 10;
	nStep		= max(1, nStep);
	nPageStep	= max(1, nPageStep);


	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
	{
		SCROLLINFO sif;
		sif.cbSize = sizeof(SCROLLINFO);
		sif.fMask = SIF_POS;
		GetScrollInfo(SB_HORZ, &sif);
		nNext = sif.nTrackPos;
		if(nNext > nMax) nNext = nMax;
		else if(nNext < nMin) nNext = nMin ;
		::SetScrollPos(m_hWnd, SB_HORZ, nNext, TRUE);
	}
	break;
	case SB_LINEDOWN:
		nNext = ::GetScrollPos(m_hWnd, SB_HORZ);
		if(nNext + nStep <= nMax)
		{
			::SetScrollPos(m_hWnd, SB_HORZ, nNext + nStep, TRUE);
			nNext += nStep;
		}
		break;
	case SB_LINEUP:
		nNext = ::GetScrollPos(m_hWnd, SB_HORZ);
		if(nNext - nStep >= nMin)
		{
			::SetScrollPos(m_hWnd, SB_HORZ, nNext - nStep, TRUE);
			nNext -= nStep;
		}
		else
		{
			nNext = 0;
			::SetScrollPos(m_hWnd, SB_HORZ, nNext, TRUE);
		}
		break;
	case SB_PAGEDOWN:
		nNext = ::GetScrollPos(m_hWnd, SB_HORZ);
		if(nNext + nPageStep <= nMax)
		{
			::SetScrollPos(m_hWnd, SB_HORZ, nNext + nPageStep, TRUE);
			nNext += nPageStep;
		}
		else
		{
			nNext = nMax;
			::SetScrollPos(m_hWnd, SB_HORZ, nNext, TRUE);
		}
		break;
	case SB_PAGEUP:
		nNext = ::GetScrollPos(m_hWnd, SB_HORZ);
		if(nNext - nPageStep >= nMin)
		{
			::SetScrollPos(m_hWnd, SB_HORZ, nNext - nPageStep, TRUE);
			nNext -= nPageStep;
		}
		else
		{
			nNext = nMin;
			::SetScrollPos(m_hWnd, SB_HORZ, nNext, TRUE);
		}
		break;
	default:
		return;
		break;
	}

	m_ptScreenOffset.x = -::GetScrollPos(m_hWnd, SB_HORZ);

	__UpdateMinimapPostiion();
	__UpdateDisplayInformation();

	Invalidate(FALSE);

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CImageWindow_Impl2::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
	if(!m_bImageWindowEnable)
	{
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}

	//	BOOL bMyCursor = FALSE;

	//=================================
	// 스크롤바에 있으면 일반 마우스
	//=================================
	if(nHitTest == HTHSCROLL || nHitTest == HTVSCROLL)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	}
	else
	{
		switch(m_nMouseControlMode)
		{
		case M_MODE_NOTHING:
		default:
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			break;
		case M_MODE_DRAG:
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			SetCursor(::AfxGetApp()->LoadCursor(IDC_IW_HANDPICK));
		}
		break;
		case M_MODE_NAVIGATION:
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			break;
		case M_MODE_DISPLAY_POSITION:
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			SetCursor(::AfxGetApp()->LoadCursor(IDC_IW_CROSSPOINT));
		}
		break;
		}

		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CImageWindow_Impl2::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	if(!m_bImageWindowEnable)
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	SetFocus();

	CRect	rect(0, 0, 0, 0);
	CPoint	ptDestMinimap;

	//-------------------
	// Minimap Hit Test
	//-------------------
	if(__IsMinimapEnabled() && __IsMinimapShowing())
	{
		CPoint ptHitTest(point.x, point.y - GetToolBarSizeY());

		__pMinimap->SetCapture(ptHitTest);

		if(__pMinimap->GetPosition(ptHitTest, ptDestMinimap))
		{
			__bMinimapMoveMode = TRUE;
		}
	}

	if(FireEventLButtonDown(nFlags, point.x, point.y, __bMinimapMoveMode))
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	CPoint  ptImage		= ClientToImage( point );

	__TraceBegin(ptImage);

	SetPixelMeterBegin(ptImage);

	//-------------------
	// ROI Tracker는 항상 처리한다.
	//-------------------
	if(m_isROIDraw)
	{
		CPoint pos_scr = point - CPoint(0, GetToolBarSizeY());
		CPoint pos_img = ScreenToImage(pos_scr);

		if( m_nSelROI == -1 )
		{
			long nROI = FindROI(pos_img);

			if( nROI != -1 )
			{
				m_pROIEditor = CROIEditFactory::Create(GetROIGeometry(nROI), &m_CoordTransform);				
				m_pROIEditor->SetBound(CRect(0, 0, m_szImage.cx, m_szImage.cy));
				m_pROIEditor->OnEditBegin(nFlags, pos_scr);
				m_nSelROI = nROI;
				m_nSelKnob = KNOB_MOVE;
			}
		}
		else
		{
			long nKnob = m_pROIEditor->FindKnob(pos_scr);	

			if( nKnob == KNOB_NOSELECT)
			{
				long nROI = FindROI(pos_img);

				if( nROI == -1 )
				{
					safeRelease(m_pROIEditor);

					m_nSelROI = -1;
				}
				else
				{
					delete m_pROIEditor;

					m_pROIEditor = CROIEditFactory::Create(GetROIGeometry(nROI), &m_CoordTransform);				
					m_pROIEditor->SetBound(CRect(0, 0, m_szImage.cx, m_szImage.cy));
					m_pROIEditor->OnEditBegin(nFlags, pos_scr);
					m_nSelROI = nROI;
					m_nSelKnob = KNOB_MOVE;
				}
			}
			else
			{
				m_nSelKnob = nKnob;
				m_pROIEditor->OnEditBegin(nFlags, pos_scr);
			}
		}
	}

	//-------------------
	// Minimap 처리
	//-------------------
	if(__bMinimapMoveMode)
	{
		ExtMove(ptDestMinimap.x, ptDestMinimap.y);
	}

	//	Refresh();
	Invalidate(FALSE);
	CWnd::OnLButtonDown(nFlags, point);
}

void CImageWindow_Impl2::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	__bMinimapMoveMode = FALSE;

	if(!m_bImageWindowEnable)
	{
		CWnd::OnLButtonUp(nFlags, point);
		return;
	}

	__TraceEnd();

	__pMinimap->ReleaseCapture(point);

	if(FireEventLButtonUp(nFlags, point.x, point.y))
	{
		CWnd::OnLButtonUp(nFlags, point);
		return;
	}

	//-------------------
	// ROI Tracker는 항상 처리한다.
	//-------------------
	if(m_isROIDraw)
	{
		if(m_nSelROI >= 0)
		{
			if( m_nSelKnob != KNOB_NOSELECT )
			{
				m_pROIEditor->OnEditEnd(nFlags, point);
				FireEventROIChanged(m_nSelROI);
				m_nSelKnob = -1;
			}			
		}
	}

	switch(m_nMouseControlMode)
	{
	case M_MODE_NOTHING:
	default:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		break;
	case M_MODE_DRAG:
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		SetCursor(::AfxGetApp()->LoadCursor(IDC_IW_HANDPICK));
	}
	break;
	case M_MODE_NAVIGATION:
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		break;
	case M_MODE_DISPLAY_POSITION:
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		SetCursor(::AfxGetApp()->LoadCursor(IDC_IW_CROSSPOINT));
	}
	break;
	}


	Invalidate(FALSE);

	CWnd::OnLButtonUp(nFlags, point);
}

void CImageWindow_Impl2::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bImageWindowEnable)
	{
		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	if(FireEventMouseMove(nFlags, point.x, point.y))
	{
		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	//-------------------
	// ROI Tracker는 항상 처리한다.
	//-------------------

	if(m_isROIDraw)
	{
		CPoint pos_scr = point - CPoint(0, GetToolBarSizeY());
		CPoint pos_img = ScreenToImage(pos_scr);

		if( m_nSelROI >= 0 )
		{
			if((nFlags & MK_LBUTTON) == MK_LBUTTON)
			{
				hCursorPrv = m_pROIEditor->UpdateCursor(m_nSelKnob);

				m_pROIEditor->OnEditing(nFlags, pos_scr);
			}
			else
			{
				int nKnobOver = m_pROIEditor->FindKnob(pos_scr);

				if( nKnobOver != KNOB_NOSELECT )
				{
					hCursorPrv = m_pROIEditor->UpdateCursor(nKnobOver);
				}
				else
				{
					int nOverROI = FindROI(pos_img);

					if( nOverROI != -1 )
					{
						hCursorPrv = SetCursor(LoadCursor(NULL, IDC_SIZEALL));
					}
					else
					{
						hCursorPrv = SetCursor(LoadCursor(NULL, IDC_ARROW));
					}

				}			
			}
		}
		else
		{
			int nOverROI = FindROI(pos_img);

			if( nOverROI != -1 )
			{
				hCursorPrv = SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			}
			else
			{
				SetCursor(hCursorPrv);
			}
		}
	}

	BOOL bRefresh = FALSE;
		
	bRefresh |= __OnMouseMove	(nFlags, point);
	bRefresh |= __OnDrag		(nFlags, point);

	if( __IsTraceEnable() && ((nFlags & MK_LBUTTON) == MK_LBUTTON) )
	{
		CPoint  pos_img = ClientToImage( point );

		CRect rtInvalidate = __Trace(pos_img);

		if( !rtInvalidate.IsRectNull() && m_nMouseControlMode == M_MODE_DISPLAY_POSITION )
		{
			rtInvalidate.InflateRect(0, 40, 100, 0);	// For Draw Current Position
		}

		InvalidateRect(rtInvalidate, FALSE);

		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	if(bRefresh || m_bDisplayCrossMark)
	{
		__UpdateMinimapPostiion();

		Invalidate(FALSE);
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CImageWindow_Impl2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(!m_bImageWindowEnable)
	{
		return CWnd::OnMouseWheel(nFlags, zDelta, pt);
	}

	ScreenToClient(&pt);

	__OnMouseWheel(nFlags, zDelta, pt);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CImageWindow_Impl2::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if((nFlags & MK_SHIFT) == MK_SHIFT)
	{
		__OnMouseWheel(nFlags, 240, point);

		CWnd::OnLButtonDown(nFlags, point);

		return;
	}
	else if((nFlags & MK_CONTROL) == MK_CONTROL)
	{
		__OnMouseWheel(nFlags, -240, point);

		CWnd::OnLButtonDown(nFlags, point);

		return;
	}

	if(FireEventLButtonDblClk(nFlags, point.x, point.y))
	{
		CWnd::OnLButtonDblClk(nFlags, point);
		return;
	}

	if(m_nMouseControlMode != M_MODE_NAVIGATION)
	{
		m_nMouseControlMode = M_MODE_NAVIGATION;
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
	}
	else
	{
		m_nMouseControlMode = M_MODE_NOTHING;
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}

	if(m_fScaleX < 1 || m_fScaleY < 1)
	{
		OnZoom5x();
	}

	Invalidate(FALSE);

	CWnd::OnLButtonDblClk(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
// CImageWindow_Impl2 message handlers


//////////////////////////////////////////////////////////////////////////////
// Image Alloc , Set/Getimage and save/load
//////////////////////////////////////////////////////////////////////////////
BOOL CImageWindow_Impl2::GetImage(const BYTE **ppbuf, long *width, long *height, long *depth, long *widthBytes)
{
	if(width	) *width	= m_imgShowSelImage.nWidth;
	if(height	) *height	= m_imgShowSelImage.nHeight;
	if(depth	) *depth	= m_imgShowSelImage.nBpp;
	if(ppbuf	) *ppbuf	= m_imgShowSelImage.pbyImage;
	if(widthBytes)	*widthBytes	= m_imgShowSelImage.nWidthBytes;

	return m_imgShowSelImage.pbyImage != nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Overlay Draw Interface function
//////////////////////////////////////////////////////////////////////////////////

void CImageWindow_Impl2::OverlayShow(BOOL bShow)
{
	m_isOverlayDraw = bShow;

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlaySetLineWidth(short nLineWidth)
{
	m_nOverlayLineWidth = nLineWidth;

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddPoint(const CPoint *pPt, OLE_COLOR crColor)
{
	Overlay::Point sobj;
	sobj.fX = (float)pPt->x;
	sobj.fY = (float)pPt->y;
	sobj.crColor = (COLORREF) crColor;

	m_vecPoint.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddPoints(const CPoint *pPt, long nCount, OLE_COLOR crColor)
{
	Overlay::Point sobj;
	sobj.crColor = (COLORREF) crColor;

	m_vecPoint.reserve(m_vecPoint.size() + nCount);

	for(long idx = 0 ; idx < nCount ; idx++)
	{
		sobj.fX = (float)pPt->x;
		sobj.fY = (float)pPt->y;

		m_vecPoint.push_back(sobj);
	}

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddLine(const CPoint *pPt1, const CPoint *pPt2, OLE_COLOR crColor)
{
	Overlay::Line sobj;
	sobj.fX1 = (float)pPt1->x;
	sobj.fX2 = (float)pPt2->x;
	sobj.fY1 = (float)pPt1->y;
	sobj.fY2 = (float)pPt2->y;

	sobj.crColor = (COLORREF) crColor;
	sobj.nPenStyle = PS_SOLID;

	m_vecLine.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddRectangle(const CRect *pRect, OLE_COLOR crColor, float /*rot*/)
{
	if(pRect == NULL)
		return;

	const CRect &rt = *pRect;

	Overlay::Rect sobj;
	sobj.fLeft		= float(rt.left);
	sobj.fTop		= float(rt.top);
	sobj.fRight		= float(rt.right);		// 20111226 윤상수정, ROI가 한칸씩 밀려들어가는 현상 수정 //sobj.fRight	= float(rt.right - 1);
	sobj.fBottom	= float(rt.bottom);		// 20111226 윤상수정, ROI가 한칸씩 밀려들어가는 현상 수정 //sobj.fBottom	= float(rt.bottom - 1);

	sobj.crColor	= (COLORREF) crColor;
	sobj.nPenStyle	= PS_SOLID;
	//	sobj.rot		= rot;

	m_vecRect.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddRectangle(const CPoint *pLT, const CPoint *pRT, const CPoint *pRB, const CPoint *pLB, OLE_COLOR crColor, float /*rot*/)
{
	if(pLT == NULL || pRT == NULL || pRB == NULL || pLB == NULL)
		return;

	const CPoint &ptLT = *pLT;
	const CPoint &ptRT = *pRT;
	const CPoint &ptRB = *pRB;
	const CPoint &ptLB = *pLB;

	Overlay::Line sobjL;
	sobjL.fX1	= float(ptLT.x);
	sobjL.fX2	= float(ptLB.x);
	sobjL.fY1	= float(ptLT.y);
	sobjL.fY2	= float(ptLB.y);

	Overlay::Line sobjT;
	sobjT.fX1	= float(ptLT.x);
	sobjT.fX2	= float(ptRT.x);
	sobjT.fY1	= float(ptLT.y);
	sobjT.fY2	= float(ptRT.y);

	Overlay::Line sobjR;
	sobjR.fX1	= float(ptRT.x);
	sobjR.fX2	= float(ptRB.x);
	sobjR.fY1	= float(ptRT.y);
	sobjR.fY2	= float(ptRB.y);

	Overlay::Line sobjB;
	sobjB.fX1	= float(ptRB.x);
	sobjB.fX2	= float(ptLB.x);
	sobjB.fY1	= float(ptRB.y);
	sobjB.fY2	= float(ptLB.y);

	sobjL.crColor	= (COLORREF) crColor;
	sobjL.nPenStyle	= PS_SOLID;
	sobjT.crColor	= (COLORREF) crColor;
	sobjT.nPenStyle	= PS_SOLID;
	sobjR.crColor	= (COLORREF) crColor;
	sobjR.nPenStyle	= PS_SOLID;
	sobjB.crColor	= (COLORREF) crColor;
	sobjB.nPenStyle	= PS_SOLID;	

	m_vecLine.push_back(sobjL);
	m_vecLine.push_back(sobjT);
	m_vecLine.push_back(sobjR);
	m_vecLine.push_back(sobjB);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddRectangles(const CRect *pRect, long nCount, OLE_COLOR crColor, float /*rot*/)
{
	if(pRect == NULL)
		return;

	m_vecRect.reserve(m_vecRect.size() + nCount);

	Overlay::Rect sobj;
	sobj.crColor	= (COLORREF) crColor;
	sobj.nPenStyle	= PS_SOLID;

	for(long idx = 0 ; idx < nCount ; idx++)
	{
		const CRect &rt = pRect[idx];

		sobj.fLeft		= float(rt.left);
		sobj.fTop		= float(rt.top);
		sobj.fRight		= float(rt.right - 1);
		sobj.fBottom	= float(rt.bottom - 1);

		m_vecRect.push_back(sobj);
	}

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddCircle(float fCenX, float fCenY, float fRadiusX, float fRadiusY, OLE_COLOR crColor, BOOL /*bScaleDepend*/)
{
	Overlay::Ellipse sobj;

	sobj.fLeft		= fCenX - fRadiusX;
	sobj.fTop		= fCenY - fRadiusY;
	sobj.fRight		= fCenX + fRadiusX;
	sobj.fBottom	= fCenY + fRadiusY;
	sobj.nPenStyle	= PS_SOLID;
	sobj.crColor	= (COLORREF) crColor;

	m_vecEllipse.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddFText(float fX, float fY, LPCTSTR str, OLE_COLOR crColor)
{
	Overlay::Text sobj;

	sobj.fX			= fX;
	sobj.fY			= fY;
	sobj.pFont		= nullptr;
	sobj.crColor	= (COLORREF) crColor;
	sobj.str		= str;

	m_vecText.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddText(const CPoint *pPt, LPCTSTR str, OLE_COLOR crColor)
{
	// 표시할 텍스트가 많아지면 GDI 리소스가 고갈되는 문제가 있음
	// 따라서, 폰트는 1 개로 지정한다.
	// 맨 처음 원소의 것을 사용.

	Overlay::Text sobj;

	sobj.fX			= (float)pPt->x;
	sobj.fY			= (float)pPt->y;
	sobj.pFont		= nullptr;
	sobj.crColor	= (COLORREF) crColor;
	sobj.str		= str;

	m_vecText.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddTextEx(const CPoint *pPt, LPCTSTR str, LOGFONT *pFont, OLE_COLOR crColor)
{
	// 표시할 텍스트가 많아지면 GDI 리소스가 고갈되는 문제가 있음
	// 따라서, 폰트는 1 개로 지정한다.
	// 맨 처음 원소의 것을 사용.

	Overlay::Text sobj;

	sobj.fX			= (float)pPt->x;
	sobj.fY			= (float)pPt->y;
	sobj.pFont		= pFont;
	sobj.crColor	= (COLORREF) crColor;
	sobj.str		= str;

	m_vecText.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayRemoveLine()
{
	m_vecLine.clear();

	Invalidate(FALSE);

}

void CImageWindow_Impl2::OverlayRemoveRectangle()
{
	m_vecRect.clear();

	Invalidate(FALSE);

}

void CImageWindow_Impl2::OverlayRemovePoint()
{
	m_vecPoint.clear();

	Invalidate(FALSE);

}

void CImageWindow_Impl2::OverlayRemoveCircle()
{
	m_vecEllipse.clear();

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayRemoveText()
{
	m_vecText.clear();

	Invalidate(FALSE);
}


void CImageWindow_Impl2::CreateFont(int nTextFontHeight)
{
	TEXTMETRIC tm;
	CClientDC dc(this);
	dc.GetTextMetrics(&tm);
	m_nTextFontHeight = nTextFontHeight + tm.tmExternalLeading;

	m_TextFont.CreateFont(m_nTextFontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
	                      ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	                      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	                      DEFAULT_PITCH | FF_SWISS, _T("Arial"));

	m_pDefaultFont = (CFont *)dc.SelectObject(&m_TextFont);
}

////////////////////////////////////////////////////////////////////////////////
// Overlay Draw Interface function end
//////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
//Coordinate transform
void CImageWindow_Impl2::TransTracker2ImagePos(CRect rect, CRect *o_Rect)
{
	CPoint lt = ClientToImage(rect.TopLeft());
	CPoint rb = ClientToImage(rect.BottomRight());

	o_Rect->top		= lt.y;
	o_Rect->left	= lt.x;
	o_Rect->right	= rb.x;
	o_Rect->bottom	= rb.y;
}

void CImageWindow_Impl2::TransImagePos2TrackerPos(float fLeft, float fTop, float fRight, float fBottom, CRect *o_Rect)
{
	CPoint lt = ImageToClient(CPoint((long)fLeft, (long)fTop));
	CPoint rb = ImageToClient(CPoint((long)fRight, (long)fBottom));

	o_Rect->top		= lt.y;
	o_Rect->left	= lt.x;
	o_Rect->right	= rb.x;
	o_Rect->bottom	= rb.y;
}

CPoint CImageWindow_Impl2::TransBound2ImagePos(CPoint i_pt)
{
	i_pt.y -= GetToolBarSizeY();
	return ScreenToImage(i_pt);
}

CPoint CImageWindow_Impl2::TransImagePos2Bound(float fX, float fY)
{
	CPoint ptScreen = ImageToScreen(CPoint((long)fX, (long)fY));
	ptScreen.y += GetToolBarSizeY();

	return ptScreen;
}

CSize CImageWindow_Impl2::ImageToScreenDist(float fDist)
{
	return CSize(ImageToScreenCX(fDist), ImageToScreenCY(fDist));
}

CRect CImageWindow_Impl2::ScreenToImageRect(const CRect &rt)
{
	return CRect(ScreenToImage(rt.TopLeft()), ScreenToImage(rt.BottomRight()));
}

CRect CImageWindow_Impl2::ImageToScreenRect(const CRect &rt)
{
	return CRect(ImageToScreen(rt.TopLeft()), ImageToScreen(rt.BottomRight()));
}

CRect CImageWindow_Impl2::ImageToScreenRect(float fLeft, float fTop, float fRight, float fBottom)
{
	CPoint ptLeftTop(ImageToScreen(fLeft, fTop));
	CPoint ptRightBottom(ImageToScreen(fRight, fBottom));

	ptRightBottom.Offset(1, 1);

	return CRect(ptLeftTop, ptRightBottom);
}

CRect CImageWindow_Impl2::ClientToImageRect(const CRect &rt)
{
	return CRect(ClientToImage(rt.TopLeft()), ClientToImage(rt.BottomRight()));
}
CRect CImageWindow_Impl2::ImageToClientRect(const CRect &rt)
{
	return CRect(ImageToClient(rt.TopLeft()), ImageToClient(rt.BottomRight()));
}




////////////////////////////////////////////////////////////////
// Context Menu
////////////////////////////////////////////////////////////////

void CImageWindow_Impl2::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CImageWindow_Impl2::OnContextMenu(CWnd *pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	if(!m_bImageWindowEnable)
	{
		return;
	}

	//--------------------------------------------------------------------------------------//
	// Popup menu 는 Roi 영역 밖에서만 사용 할수 있게 한다.                                 //
	// 여기서 point 는 스크린 기준이고
	// OnRButtonDown(UINT nFlags, CPoint point)  //여기서 point 는 클라이언트 기준이다!
	//  클라이언트 포인터를 스크린 포인터로 변경 : ClientToScreen(&point);
	//--------------------------------------------------------------------------------------//
	CPoint ptPos = point;
	ScreenToClient(&ptPos);

	__ptContextMenuPos = ClientToImage( ptPos );
	if(FireEventRButtonUp(NULL, ptPos.x, ptPos.y, TRUE))
	{
		return;
	}

	//===============================================================================================
	// 20090916 송윤상 추가
	// 이미지 윈도우 UI 변경 -
	// Panning버튼을 오른쪽 버튼으로 변경하면서 팝업메뉴처리 수정
	// 오른쪽 버튼을 눌렀다 뗀 거리가 4(pixel)이상이면 팝업메뉴를 띄우지 않는다
	//===============================================================================================

	CSize size = __szRButtonDragDistance;

	if( ((long)sqrt((double)(size.cx * size.cx + size.cy * size.cy))) > 4 ) return;

	CMenu muTemp;
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		muTemp.LoadMenu(IDR_MENU);
	}

	CMenu *pMenu = muTemp.GetSubMenu(0);

	CmdRouteMenu(pWnd, pMenu);

	__AppendMenuExternal(pMenu);

	__SetCheckPopupMenu(pMenu);

	pMenu->TrackPopupMenu(TPM_LEFTBUTTON | TPM_LEFTALIGN, point.x, point.y, pWnd);

	Invalidate(TRUE);
}

void CImageWindow_Impl2::CmdRouteMenu(CWnd *pWnd, CMenu *pPopupMenu)
{
	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	state.m_pParentMenu = pPopupMenu;
	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();

	for (state.m_nIndex = 0 ; state.m_nIndex < state.m_nIndexMax ; state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);

		// menu separator or invalid cmd - ignore it

		if (state.m_nID == 0) continue;

		if (state.m_nID == (UINT) - 1)
		{
			// possibly a popup menu, route to child menu if so

			CMenu *pSub = pPopupMenu->GetSubMenu(state.m_nIndex);
			if(pSub) CmdRouteMenu(pWnd, pSub);
		}
		else
		{
			// normal menu item, Auto disable if command is

			// _not_ a system command.

			state.m_pSubMenu = nullptr;
			state.DoUpdate(pWnd, FALSE);
		}
	}
}

void CImageWindow_Impl2::OnZoomin()
{
	__Zoom(1, CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));
}

void CImageWindow_Impl2::OnZoomout()
{
	__Zoom(-1, CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));
}

void CImageWindow_Impl2::OnZoom1x()
{
	//=================================
	// ZoomScale 변경
	//=================================

	m_fScaleX = 1.f;
	m_fScaleY = 1.f;

	UpdateScrollRange();

	CPoint ptImageCenter = ScreenToImage(CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));
	ExtMove(ptImageCenter.x, ptImageCenter.y);

	m_nZoomWheel = __FindZoomScaleIndex(min(m_fScaleX, m_fScaleY));

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OnZoomwidthfit()
{
	m_nDefaultZoomFitMode = ZOOMFIT_MODE_WIDTH;

	__ZoomFitWidth();
	__ZoomFitWidth();
}

void CImageWindow_Impl2::OnZoomimagefit()
{
	m_nDefaultZoomFitMode = ZOOMFIT_MODE_IMAGE;

	__ZoomFitImage();
}

void CImageWindow_Impl2::OnZoomboundfit()
{
	m_nDefaultZoomFitMode = ZOOMFIT_MODE_BOUND;

	__ZoomFitBound();
}

void CImageWindow_Impl2::OnMove()
{
	if(m_nMouseControlMode != M_MODE_NAVIGATION)
	{
		m_nMouseControlMode = M_MODE_NAVIGATION;
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
	}
	else
	{
		m_nMouseControlMode = M_MODE_NOTHING;
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}

//Zoom In/Out 외부설정 함수 추가 2005.11.6 주병권
void CImageWindow_Impl2::ExtZoomIn()
{
	OnZoomin();
}

void CImageWindow_Impl2::ExtZoomOut()
{
	OnZoomout();
}

//---> 원섭 2005-07-04 [Overlay항목들을 모두 삭제한다.]
void CImageWindow_Impl2::OverlayRemoveAll()
{
	m_vecPoint		.clear();
	m_vecLine		.clear();
	m_vecRect		.clear();
	m_vecEllipse	.clear();
	m_vecArcDoughnut.clear();
	m_vecArcRing	.clear();

	m_vecText.clear();

	Invalidate(FALSE);
}

void CImageWindow_Impl2::ExtMove(long nImageCenterX, long nImageCenterY)
{
	ExtMove((float)nImageCenterX, (float)nImageCenterY);
}

void CImageWindow_Impl2::ExtMove(float fImageCenterX, float fImageCenterY)
{
	__Scroll(	-__GetScreenOffsetX(m_szScreen.cx / 2.f, fImageCenterX),
	            -__GetScreenOffsetY(m_szScreen.cy / 2.f, fImageCenterY));

}

void CImageWindow_Impl2::SetDisplayImage(const BYTE *buf, long nWidth, long nHeight, long nWidthBytes, long nDepth)
{
	SWindow_Image previmg = m_imgOriginalImage;

	m_imgOriginalImage.pbyImage		= buf;
	m_imgOriginalImage.nWidth		= nWidth;
	m_imgOriginalImage.nHeight		= nHeight;
	m_imgOriginalImage.nWidthBytes	= nWidthBytes;
	m_imgOriginalImage.nBpp			= nDepth;

	m_imgShowSelImage				= m_imgOriginalImage;

	// 일부만 보는 중이라면(이미지가 같아야 하고, 영역이 설정되어 있어야 한다.)
	if( m_bUseRangeImage &&
	    m_imgOriginalImage.nWidth == nWidth &&
	    m_imgOriginalImage.nHeight == nHeight &&
	    0 <= m_rtRangeImage.Width() &&
	    0 <= m_rtRangeImage.Height() )
	{
		// 이미지가 0보다 작거나 같다면 그릴게 없다.
		SetDisplayRange(TRUE, m_rtRangeImage.left, m_rtRangeImage.top, m_rtRangeImage.right, m_rtRangeImage.bottom);
	}

	if(m_pPaletteColor)
	{
		delete []m_pPaletteColor;
		m_pPaletteColor = nullptr;
	}


	if(nWidth == 0 || nHeight == 0 || nDepth == 0 || buf == NULL)
	{
		__UpdateScale();
		return;
	}

	if(__IsMinimapEnabled())
		__pMinimap->SetImage(m_imgShowSelImage);

	if(previmg.nWidth != nWidth || previmg.nHeight != nHeight)
	{
		__UpdateScale();
		UpdateScrollRange();
		Invalidate(TRUE);
	}
	else
		Invalidate(FALSE);

}

void CImageWindow_Impl2::UpdateScrollRange()
{
	if( __IsScrollRangeUpdating() ) return;

	__SetScrollRangeUpdating(TRUE);

	m_CoordTransform.SetScale(m_fScaleX, m_fScaleY);

	UpdateScrollShow();

	CRect rtClient;
	GetClientRect(&rtClient);

	rtClient.bottom -= GetToolBarSizeY();

	m_szScreen = rtClient.Size();

	__UpdateScrollInfoX();
	__UpdateScrollInfoY();

	__SetScrollRangeUpdating(FALSE);
}

void CImageWindow_Impl2::UpdateScrollShow()
{
	BOOL bShowH = __IsHScrollAvailable();
	BOOL bShowV = __IsVScrollAvailable();

	if(bShowH != bShowV)
	{
		ShowScrollBar(SB_HORZ, bShowH);
		ShowScrollBar(SB_VERT, bShowV);
	}
	else
	{
		ShowScrollBar(SB_BOTH, bShowV);
	}
}

void CImageWindow_Impl2::__UpdateScrollInfoX()
{
	m_szScroll.cx = max( (long)(m_szImage.cx * m_fScaleX + 0.5f - m_szScreen.cx), 0);

	if(!m_imgShowSelImage.pbyImage) return ;

	ASSERT( ( m_szScroll.cx != 0 ) ==  __IsHScrollAvailable() );

	if( m_szScroll.cx == 0 ) return;

	long nRangeX = m_szScroll.cx;

	SCROLLINFO si = {sizeof(SCROLLBARINFO), SIF_PAGE, 0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nPage = ScreenToImageCX((float)m_szScreen.cx);
	si.nMax = nRangeX + si.nPage;
	si.nMin = 0;
	si.nPos = -m_ptScreenOffset.x;
	si.nTrackPos = 0;

	SetScrollInfo(SB_HORZ, &si, TRUE);
}

void CImageWindow_Impl2::__UpdateScrollInfoY()
{
	m_szScroll.cy = max( (long)(m_szImage.cy * m_fScaleY - m_szScreen.cy), 0);

	if(m_imgShowSelImage.pbyImage != NULL)
		ASSERT( ( m_szScroll.cy != 0 ) == __IsVScrollAvailable() );

	if( m_szScroll.cy == 0 ) return;

	long nRangeY = m_szScroll.cy;

	SCROLLINFO si = {sizeof(SCROLLBARINFO), SIF_PAGE, 0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nPage = ScreenToImageCY((float)m_szScreen.cy);
	si.nMax = nRangeY + si.nPage;
	si.nMin = 0;
	si.nPos = -m_ptScreenOffset.y;
	si.nTrackPos = 0;

	SetScrollInfo(SB_VERT, &si, TRUE);
}

BOOL CImageWindow_Impl2::__IsHScrollAvailable()
{
	if(!m_imgShowSelImage.pbyImage) return FALSE;

	return __GetScaleWidthFit() < m_fScaleX;
}

BOOL CImageWindow_Impl2::__IsVScrollAvailable()
{
	if(!m_imgShowSelImage.pbyImage) return FALSE;

	return __GetScaleHeightFit() < m_fScaleY;
}

void CImageWindow_Impl2::ROIShow()
{
	m_isROIDraw = TRUE;
	Invalidate(FALSE);
}

void CImageWindow_Impl2::ROIHide()
{
	m_isROIDraw = FALSE;
	Invalidate(FALSE);
}

long CImageWindow_Impl2::ROIAdd(const CRect& rtROI, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable)
{
	ROIDrawPara para;

	para.nLineWidth		= nLineWidth;
	para.crColor		= (COLORREF) crColor;
	para.isMovable		= isMovalble;
	para.isResizable	= isResizable;
	para.strName		= pstrName;

	CROIGeometryBase*	pROIGeo		= new CROIGeometryRect	(rtROI);
	CROIDrawBase*		pROIDraw	= new CROIDrawRect		(pROIGeo, para, &m_CoordTransform);

	m_vecROIGeo	.push_back(pROIGeo);
	m_vecROIDraw.push_back(pROIDraw);

	Invalidate(FALSE);

	return (long)m_vecROIDraw.size() - 1;
}

long CImageWindow_Impl2::ROIAdd(const ROIData& Data, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable)
{
	ROIDrawPara para;

	para.nLineWidth		= nLineWidth;
	para.crColor		= (COLORREF) crColor;
	para.isMovable		= isMovalble;
	para.isResizable	= isResizable;
	para.strName		= pstrName;

	CROIGeometryBase*	pROIGeo		= nullptr;
	CROIDrawBase*		pROIDraw	= nullptr;
	switch(Data.nType)
	{
	case RECTANGLE:
		pROIGeo		= new CROIGeometryRect			(Data.rt);
		pROIDraw	= new CROIDrawRect				(pROIGeo, para, &m_CoordTransform);
		break;
	case RECTANGLEROUNDED:
		pROIGeo		= new CROIGeometryRectRounded	(Data);
		pROIDraw	= new CROIDrawRectRounded		(pROIGeo, para, &m_CoordTransform);
		break;

	case TRIANGLE:
		pROIGeo		= new CROIGeometryPolygon		(Data.vecPoint);
		pROIDraw	= new CROIDrawPolygon			(pROIGeo, para, &m_CoordTransform);
		break;
	}

	if( pROIGeo && pROIDraw )
	{
		m_vecROIGeo	.push_back(pROIGeo);
		m_vecROIDraw.push_back(pROIDraw);

		Invalidate(FALSE);

		return (long)m_vecROIDraw.size() - 1;
	}

	return -1;
}

void CImageWindow_Impl2::ROISetMinSize(long nMinSizeX, long nMinSizeY)
{
	if(nMinSizeX > 0 && nMinSizeX < m_imgShowSelImage.nWidth )	m_ROIszMin.cx = nMinSizeX;
	if(nMinSizeY > 0 && nMinSizeY < m_imgShowSelImage.nHeight )	m_ROIszMin.cy = nMinSizeY;
}

void CImageWindow_Impl2::ROIRemoveAll()
{
	m_nSelROI = -1;
	m_nSelKnob = -1;

	safeReleaseVector(m_vecROIDraw);
	safeReleaseVector(m_vecROIGeo);

	safeRelease(m_pROIEditor);

	Invalidate(FALSE);
}

BOOL CImageWindow_Impl2::ROIGet(long nID, CRect *prtROI)
{
	if( OutOfRange(m_vecROIGeo, nID) ) return FALSE;

	const CROIGeometryBase* pROIGeo = m_vecROIGeo[nID];

	ROIData data;
	pROIGeo->ToExport(data);
	
	prtROI->CopyRect(&data.rt);

	return TRUE;
}

ROIData CImageWindow_Impl2::ROIGet(long nID) const
{
	if( OutOfRange(m_vecROIGeo, nID) )
	{
		ROIData empty;

		empty.nType = -1;

		return empty;
	}

	const CROIGeometryBase* pROIGeo = m_vecROIGeo[nID];

	ROIData data;
	pROIGeo->ToExport(data);

	return data;
}

BOOL CImageWindow_Impl2::ROISel(long nID)
{
	CROIGeometryBase* pROIGeometry = GetROIGeometry(nID);

	if( pROIGeometry == NULL ) return FALSE;

	if( m_nSelROI == nID ) return TRUE;

	safeRelease(m_pROIEditor);

	m_pROIEditor = CROIEditFactory::Create(pROIGeometry, &m_CoordTransform);
	m_pROIEditor->SetBound(CRect(0, 0, m_szImage.cx, m_szImage.cy));
	m_nSelROI = nID;

	Invalidate(FALSE);

	return TRUE;
}

long CImageWindow_Impl2::ROICount() const
{
	return (long)m_vecROIGeo.size();
}

void CImageWindow_Impl2::OverlayAddLineEx(const CPoint *pPt1, const CPoint *pPt2, OLE_COLOR crColor, long nPenStyle)
{
	// TODO: Add your dispatch handler code here
	Overlay::Line sobj;
	sobj.fX1 = (float)pPt1->x;
	sobj.fX2 = (float)pPt2->x;
	sobj.fY1 = (float)pPt1->y;
	sobj.fY2 = (float)pPt2->y;

	sobj.crColor = (COLORREF) crColor;
	sobj.nPenStyle = nPenStyle;

	m_vecLine.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddRectangleEx(const CRect *pRect, OLE_COLOR crColor, long nPenStyle)
{
	Overlay::Rect sobj;

	const CRect &rt = *pRect;

	sobj.fLeft		= float(rt.left);
	sobj.fTop		= float(rt.top);
	sobj.fRight		= float(rt.right - 1);
	sobj.fBottom	= float(rt.bottom - 1);
	sobj.crColor	= (COLORREF) crColor;
	sobj.nPenStyle	= nPenStyle;
	//	sobj.rot		= 0.f;

	m_vecRect.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::ExtZoomScaleGet(double *pScaleH, double *pScaleV)
{
	if(pScaleH) *pScaleH = m_fScaleX;
	if(pScaleV) *pScaleV = m_fScaleY;
}

// =========================================================
/** @fn		void CImageWindow_Impl2::ExtZoomScale(double fScaleH, double fScaleV)
@brief		임의의 스케일로 변환한다.
@param		fScaleH : 수평측 스케일 값
@param		fScaleV : 수직측 스케일 값
@return		:
@date		: 2005-12-16 오후 2:00:33
@author		: 한원섭
@warning	: 스케일값은 둘다 0이 될수 없다.
@sa			: OnZoomfit(), OnZoomWidthfit(), OnZoomin(), OnZoomout, ExtZoomIn(), ExtZoomOut()
@note		: fScaleH(V) 값은 다음의 값을 추가로 가진다
-1 : Width(Height) Fit (수평측으로 한번에 다 보이는 스케일을 가진다.
0 : 다른 fScale 값과 동일(fScaleV가 0이면 fScaleV = fScaleH 값이 된다.
따라서 둘다 0이 될 수는 없다.

Width Fit을 할려면	: fScaleH = -1, fScaleV =  0 을 넣어주면 수행되고
Height Fit을 할려면	: fScaleH =  0, fScaleV = -1 을 넣어주면 수행되고
Bound Fit을 할려면	: fScaleH = -1, fScaleV = -1 을 넣어주면 수행된다.
*/ // ======================================================
void CImageWindow_Impl2::ExtZoomScale(double fScaleH, double fScaleV)
{
	if( fScaleH == 0 && fScaleV == 0)				return;
	if( m_szScreen.cx <= 0 || m_szScreen.cy <= 0 )	return;

	__Zoom((float)fScaleH, (float)fScaleV, CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));
}

void CImageWindow_Impl2::ExtZoomROIGet(float *pLeft, float *pTop, float *pRight, float *pBottom)
{
	CRect rtView(0, 0, 0, 0);
	GetClientRect(rtView);

	float fX1 = 0.f, fY1 = 0.f, fX2 = 0.f, fY2 = 0.f;
	__ConvertViewToImagePos(rtView.left,	rtView.top,		fX1, fY1);
	__ConvertViewToImagePos(rtView.right,	rtView.bottom,	fX2, fY2);

	if(pLeft)	*pLeft		= fX1;
	if(pTop)	*pTop		= fY1;
	if(pRight)	*pRight		= fX2;
	if(pBottom)	*pBottom	= fY2;
}

void CImageWindow_Impl2::ExtZoomROISet(float fLeft, float fTop, float fRight, float fBottom)
{
	float fROI_SizeX = (fRight	- fLeft);
	float fROI_SizeY = (fBottom	- fTop);

	float fScaleH = m_szScreen.cx / fROI_SizeX;
	float fScaleV = m_szScreen.cy / fROI_SizeY;

	if( fScaleH == 0 && fScaleV == 0) return;

	__Zoom((float)fScaleH, (float)fScaleV, CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));
	ExtMove((long)((fLeft + fRight) / 2), (long)((fTop + fBottom) / 2));

	UpdateScrollRange();

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( m_wndToolBar.GetSafeHwnd() )
	{
		long nToolBarSizeY = GetToolBarSizeY();

		__MoveView(0, nToolBarSizeY, cx, cy - nToolBarSizeY);
	}
}

void CImageWindow_Impl2::ExtZoomFit(short nZoomFitMode)
{
	switch(nZoomFitMode)
	{
	case ZOOMFIT_MODE_WIDTH:
		__ZoomFitWidth();
		break;
	case ZOOMFIT_MODE_IMAGE:
		__ZoomFitImage();
		break;
	case ZOOMFIT_MODE_BOUND:
		__ZoomFitBound();
		break;
	default :
		break;
	}

	m_nZoomWheel = __FindZoomScaleIndex(min(m_fScaleX, m_fScaleY));
}

void CImageWindow_Impl2::OverlayAddFPoint(float fX, float fY, OLE_COLOR crColor)
{
	// TODO: Add your dispatch handler code here
	Overlay::Point sobj;
	sobj.fX = fX;
	sobj.fY = fY;
	sobj.crColor = (COLORREF) crColor;

	m_vecPoint.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddFLine(float fX1, float fY1, float fX2, float fY2, OLE_COLOR crColor)
{
	// TODO: Add your dispatch handler code here
	Overlay::Line sobj;
	sobj.fX1 = fX1;
	sobj.fX2 = fX2;
	sobj.fY1 = fY1;
	sobj.fY2 = fY2;

	sobj.crColor = (COLORREF) crColor;
	sobj.nPenStyle = PS_SOLID;

	m_vecLine.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddFRectangle(float fLeft, float fTop, float fRight, float fBottom, OLE_COLOR crColor)
{
	// TODO: Add your dispatch handler code here
	Overlay::Rect sobj;

	sobj.fLeft		= fLeft;
	sobj.fTop		= fTop;
	sobj.fRight		= fRight;
	sobj.fBottom	= fBottom;
	sobj.crColor	= (COLORREF) crColor;
	sobj.nPenStyle	= PS_SOLID;
	//	sobj.rot		= 0.f;

	m_vecRect.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddArcDoughnut(float fCenX, float fCenY, float fStartAngle, float fEndAngle, float fRadiusX, float fRadiusY, float fWidth, OLE_COLOR crColor)
{
	Overlay::ArcDoughnut sobj;

	sobj.fCenX			= fCenX;
	sobj.fCenY			= fCenY;
	sobj.fStartAngle	= fStartAngle;
	sobj.fEndAngle		= fEndAngle;
	sobj.fRadiusX		= fRadiusX;
	sobj.fRadiusY		= fRadiusY;
	sobj.fWidth			= fWidth;
	sobj.crColor		= (COLORREF)crColor;
	sobj.nPenStyle		= PS_SOLID;

	m_vecArcDoughnut.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OverlayAddArcRing(float fCenX, float fCenY, float fStartAngle, float fEndAngle, float fInnerLength, float fOuterLength, OLE_COLOR crColor)
{
	Overlay::ArcRing sobj;

	sobj.fCenX			= fCenX;
	sobj.fCenY			= fCenY;
	sobj.fStartAngle	= fStartAngle;
	sobj.fEndAngle		= fEndAngle;
	sobj.fInnerLength	= fInnerLength;
	sobj.fOuterLength	= fOuterLength;
	sobj.crColor		= (COLORREF)crColor;
	sobj.nPenStyle		= PS_SOLID;

	m_vecArcRing.push_back(sobj);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::SetDisplayWindow(CWnd *pDialog)
{
	// 이미 Dialog 가 붙어 있다면 제거한다

	if(m_pChildWindow)
	{
		m_pChildWindow->SetParent(nullptr);
		m_pChildWindow->ShowWindow(SW_HIDE);
		m_bImageWindowEnable = TRUE;
	}

	// Dialog 를 붙이되 ImageWindow 기능은 Disable 시킨다

	m_pChildWindow = pDialog;

	if(m_pChildWindow)
	{
		m_pChildWindow->SetParent(this);
		m_pChildWindow->ShowWindow(SW_SHOW);

		SetScrollRange(SB_VERT, 0, 0, TRUE);
		SetScrollRange(SB_HORZ, 0, 0, TRUE);

		CRect rtView(0, 0, 0, 0);
		GetClientRect(rtView);
		m_pChildWindow->MoveWindow(rtView);
		m_bImageWindowEnable = FALSE;
	}
	else
	{
		UpdateScrollRange();
	}
}

void CImageWindow_Impl2::SetDisplayImage_Palette(const BYTE *buf, long nWidth, long nHeight, long nWidthBytes, long nDepth, OLE_COLOR *pPaletteColor)
{
	SWindow_Image previmg = m_imgOriginalImage;

	m_imgOriginalImage.pbyImage		= buf;
	m_imgOriginalImage.nWidth		= nWidth;
	m_imgOriginalImage.nHeight		= nHeight;
	m_imgOriginalImage.nWidthBytes	= nWidthBytes;
	m_imgOriginalImage.nBpp			= nDepth;

	m_imgShowSelImage				= m_imgOriginalImage;

	//SWindow_Image previmg = m_imgShowSelImage;
	//m_imgShowSelImage.pbyImage		= buf;
	//m_imgShowSelImage.nWidth		= nWidth;
	//m_imgShowSelImage.nHeight		= nHeight;
	//m_imgShowSelImage.nWidthBytes	= nWidthBytes;
	//m_imgShowSelImage.nBpp			= nDepth;

	//m_Scale.SetRange(FALSE, 0, 0, 0, 0, 0, 0);

	// 일부만 보는 중이라면
	if(m_bUseRangeImage)
	{
		// 이미지 사이즈가 같다면 계속 일부만 보게 한다.
		if(m_imgOriginalImage.nWidth == nWidth || m_imgOriginalImage.nHeight == nHeight)
		{
			// 이미지가 0보다 작거나 같다면 그릴게 없다.
			if(0 < m_rtRangeImage.Width() && 0 < m_rtRangeImage.Height())
			{
				SetDisplayRange(TRUE, m_rtRangeImage.left, m_rtRangeImage.top, m_rtRangeImage.right, m_rtRangeImage.bottom);
			}
		}
	}
	else
	{
		m_imgShowSelImage = m_imgOriginalImage;
	}

	if(m_pPaletteColor)
	{
		delete []m_pPaletteColor;
		m_pPaletteColor = nullptr;
	}

	long nColorNum = (long)pow(2.f, nDepth);
	m_pPaletteColor = new COLORREF[nColorNum];

	for(long n = 0; n < nColorNum; n++)
	{
		m_pPaletteColor[n] = pPaletteColor[n];
	}

	if(nWidth == 0 || nHeight == 0 || nDepth == 0 || buf == NULL)
	{
		__UpdateScale();
		return;
	}

	if(previmg.nWidth != nWidth || previmg.nHeight != nHeight)
	{
		__UpdateScale();
		UpdateScrollRange();
		Invalidate(TRUE);
	}
	else
		Invalidate(FALSE);
}

void CImageWindow_Impl2::EnableDisplayHighQuality(BOOL bEnable)
{
	m_iStretchMode = bEnable ? HALFTONE : COLORONCOLOR;
}

void CImageWindow_Impl2::OnDisplaycurrentpos()
{
	if(m_nMouseControlMode != M_MODE_DISPLAY_POSITION)
	{
		m_nMouseControlMode = M_MODE_DISPLAY_POSITION;
	}
	else
	{
		m_nMouseControlMode = M_MODE_NOTHING;
	}

	Invalidate(FALSE);
}

void CImageWindow_Impl2::uDrawCurrentPosition( CDC *pdc )
{
	if( m_nMouseControlMode != M_MODE_DISPLAY_POSITION ) return;

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CString str = __MakeImagePositionString(ClientToImage(point));

	pdc->SetTextColor(RGB(0, 50, 0));
	pdc->TextOut(point.x, point.y - 40 - GetToolBarSizeY(), str);
}

void CImageWindow_Impl2::uDrawPixelMeter( CDC *pdc )
{
	if(m_nMouseControlMode == M_MODE_DISPLAY_POSITION) return; // 현재 포지션 출력하면 화면이 보기 싫다

	if( !pdc ) return;

	m_PixelMeter.Draw(pdc);

}

void CImageWindow_Impl2::uDrawMinimap(CDC *pdc, const CRect &rt)
{
	if( !pdc ) return;
	if(!__IsMinimapEnabled() || !__IsMinimapShowing()) return;

	__pMinimap->DrawAlphaBlendDraw(pdc, rt);
}

void CImageWindow_Impl2::OnCrossmark()
{
	//static BYTE ppp[] = {
	//	0x01, 0x01, 0x01, 0x11, 0x11, 0x11, 0x21, 0x21, 0x21, 0x31, 0x31, 0x31, 0x41, 0x41, 0x41, 0x51, 0x51, 0x51, 0x61, 0x61, 0x61, 0x71, 0x71, 0x71,
	//	0x01, 0x01, 0x01, 0x11, 0x11, 0x11, 0x21, 0x21, 0x21, 0x31, 0x31, 0x31, 0x41, 0x41, 0x41, 0x51, 0x51, 0x51, 0x61, 0x61, 0x61, 0x71, 0x71, 0x71,
	//	0x01, 0x01, 0x01, 0x11, 0x11, 0x11, 0x21, 0x21, 0x21, 0x31, 0x31, 0x31, 0x41, 0x41, 0x41, 0x51, 0x51, 0x51, 0x61, 0x61, 0x61, 0x71, 0x71, 0x71,
	//	0x01, 0x01, 0x01, 0x11, 0x11, 0x11, 0x21, 0x21, 0x21, 0x31, 0x31, 0x31, 0x41, 0x41, 0x41, 0x51, 0x51, 0x51, 0x61, 0x61, 0x61, 0x71, 0x71, 0x71,
	//	0x01, 0x01, 0x01, 0x11, 0x11, 0x11, 0x21, 0x21, 0x21, 0x31, 0x31, 0x31, 0x41, 0x41, 0x41, 0x51, 0x51, 0x51, 0x61, 0x61, 0x61, 0x71, 0x71, 0x71};

	//SetDisplayImage(ppp, 5, 5, 24, 24);

	m_bDisplayCrossMark = !m_bDisplayCrossMark;
	Invalidate(FALSE);
}

void CImageWindow_Impl2::OnUpdateCrossmark(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bDisplayCrossMark);
}

void CImageWindow_Impl2::OnRemovealloverlay()
{
	OverlayRemoveAll();
}

void CImageWindow_Impl2::OnUpdateRemovealloverlay(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (m_vecPoint.size() + m_vecLine.size() + m_vecRect.size() + m_vecEllipse.size() + m_vecArcDoughnut.size() + m_vecArcRing.size() + m_vecText.size()) != 0);
}

void CImageWindow_Impl2::OnZoom2x()
{
	m_fScaleX = 2.f;
	m_fScaleY = 2.f;

	UpdateScrollRange();

	CPoint ptImageCenter = ScreenToImage(CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));
	ExtMove(ptImageCenter.x, ptImageCenter.y);

	m_nZoomWheel = __FindZoomScaleIndex(min(m_fScaleX, m_fScaleY));

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OnZoom5x()
{
	m_fScaleX = 5.f;
	m_fScaleY = 5.f;

	UpdateScrollRange();

	CPoint ptImageCenter = ScreenToImage(CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));
	ExtMove(ptImageCenter.x, ptImageCenter.y);

	m_nZoomWheel = __FindZoomScaleIndex(min(m_fScaleX, m_fScaleY));

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OnZoom10x()
{
	m_fScaleX = 10.f;
	m_fScaleY = 10.f;

	UpdateScrollRange();

	CPoint ptImageCenter = ScreenToImage(CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));
	ExtMove(ptImageCenter.x, ptImageCenter.y);

	m_nZoomWheel = __FindZoomScaleIndex(min(m_fScaleX, m_fScaleY));

	Invalidate(FALSE);
}

void CImageWindow_Impl2::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	SetFocus();

	if(FireEventRButtonDown(nFlags, point.x, point.y))
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;
	}

	__szRButtonDragDistance.SetSize(0, 0);

	m_nMouseControlMode = M_MODE_DRAG;
	m_bDragPrevPos = point;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetCursor(AfxGetApp()->LoadCursor(IDC_IW_HANDPICK));

	Invalidate(FALSE);

	CWnd::OnRButtonDown(nFlags, point);
}

void CImageWindow_Impl2::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if(FireEventRButtonUp(nFlags, point.x, point.y, FALSE))
	{
		CWnd::OnRButtonUp(nFlags, point);
		return;
	}

	m_nMouseControlMode = M_MODE_NOTHING;

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

	CWnd::OnRButtonDown(nFlags, point);
}

void CImageWindow_Impl2::EnableMinimap(BOOL bEnable, CSize size)
{
	__bMinimapEnabled = bEnable;

	__pMinimap->SetMinimapSize(size);
	__pMinimap->SetImage(m_imgShowSelImage);
	__pMinimap->SetCurrentViewPos(CRect(ScreenToImage(CPoint(0, 0)), ScreenToImage(CPoint(m_szScreen.cx, m_szScreen.cy))));
	Invalidate(FALSE);
}

void CImageWindow_Impl2::SetDefaultZoomFitMode(long nZoomFitMode)
{
	switch(nZoomFitMode)
	{
	case 1:
		m_nDefaultZoomFitMode = ZOOMFIT_MODE_WIDTH;
		break;
	case 2:
		m_nDefaultZoomFitMode = ZOOMFIT_MODE_IMAGE;
		break;
	case 3:
		m_nDefaultZoomFitMode = ZOOMFIT_MODE_BOUND;
		break;
	default:
		m_nDefaultZoomFitMode = ZOOMFIT_MODE_BOUND;
		break;
	}
}

void CImageWindow_Impl2::AddPopupMenuExternal(long nID, LPCTSTR pstrItem)
{
	if(__mapExpandMenu.find(nID) != __mapExpandMenu.end())
	{
		ASSERT(0);
		return;
	}

	__mapExpandMenu[nID]		= pstrItem;
	__mapExpandMenuOption[nID]	= MAKELONG(1, 0);
}

void CImageWindow_Impl2::RemovePopupMenuExternal(long nID)
{
	if( __mapExpandMenu.find(nID) == __mapExpandMenu.end() )
	{
		ASSERT(0);
		return;
	}

	__mapExpandMenu.erase(__mapExpandMenu.find(nID));
	__mapExpandMenuOption.erase(__mapExpandMenuOption.find(nID));
}

void CImageWindow_Impl2::ModifyPopupMenuExternal(long nID, short bEnable, short bChecked)
{
	if( __mapExpandMenu.find(nID) == __mapExpandMenu.end() )
	{
		ASSERT(0);
		return;
	}

	__mapExpandMenuOption[nID] = MAKELONG(bEnable, bChecked);
}

void CImageWindow_Impl2::__AppendMenuExternal(CMenu *pMenu)
{
	if(__mapExpandMenu.empty()) return;
	pMenu->AppendMenu(MF_SEPARATOR);

	map<long, CString>::iterator iter = __mapExpandMenu.begin();
	while( iter != __mapExpandMenu.end() )
	{
		if(iter->second != _T(""))
		{
			DWORD dwOption = __mapExpandMenuOption[iter->first];
			BOOL bEnable = LOWORD(dwOption);
			BOOL bCheck = HIWORD(dwOption);
			long nID = EXTMENU_BASEID + (long)(iter->first);
			pMenu->AppendMenu(MF_STRING, nID, iter->second);
			pMenu->CheckMenuItem(nID, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
			pMenu->EnableMenuItem(nID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_DISABLED));
		}
		else
		{
			pMenu->AppendMenu(MF_SEPARATOR);
		}
		++iter;
	}
}

void CImageWindow_Impl2::__DeleteMenuExternal(CMenu *pMenu)
{
	if(__mapExpandMenu.empty())
		return;

	map<long, CString>::iterator iter = __mapExpandMenu.begin();
	while( iter != __mapExpandMenu.end() )
	{
		pMenu->DeleteMenu(iter->first, MF_BYCOMMAND);
		++iter;
	}

	pMenu->DeleteMenu(pMenu->GetMenuItemCount() - 1, MF_BYPOSITION);
}

void CImageWindow_Impl2::__SetCheckPopupMenu(CMenu *pMenu)
{
	__SetCheckShowToolBarPopupMenu(pMenu);
	__SetCheckShowMiniMapPopupMenu(pMenu);
	__SetCheckZoomFitModePopupMenu(pMenu);
	__SetCheckShowPixelMeterPopupMenu(pMenu);
}

void CImageWindow_Impl2::__SetCheckShowToolBarPopupMenu(CMenu *pMenu)
{
	BOOL bCheck = m_wndToolBar.IsVisible();

	pMenu->CheckMenuItem(ID_POPUP_SHOWTOOLBAR, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
}

void CImageWindow_Impl2::__SetCheckShowMiniMapPopupMenu(CMenu *pMenu)
{
	BOOL bCheck = __IsMinimapEnabled();

	pMenu->CheckMenuItem(ID_POPUP_SHOWMINIMAP, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
}

void CImageWindow_Impl2::__SetCheckZoomFitModePopupMenu(CMenu *pMenu)
{
	BOOL bCheck = TRUE;

	UINT nID = __GetZoomFitModeID(m_nDefaultZoomFitMode);

	pMenu->CheckMenuItem(nID, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
}

void CImageWindow_Impl2::__SetCheckShowPixelMeterPopupMenu(CMenu *pMenu)
{
	BOOL bCheck = m_PixelMeter.IsEnable();

	pMenu->CheckMenuItem(ID_POPUP_SHOWPIXELMETER, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
}

UINT CImageWindow_Impl2::__GetZoomFitModeID(enumZoomFitMode nZoomFitMode)
{
	switch(nZoomFitMode)
	{
	case ZOOMFIT_MODE_WIDTH:
		return ID_ZOOMWIDTHFIT;
	case ZOOMFIT_MODE_IMAGE:
		return ID_ZOOMIMAGEFIT;
	case ZOOMFIT_MODE_BOUND:
		return ID_ZOOMBOUNDFIT;
	}

	return (UINT) - 1;
}

BOOL CImageWindow_Impl2::OnCommand(WPARAM wParam, LPARAM lParam)
{
	long nCmdID = (long)wParam - EXTMENU_BASEID;
	map<long, CString>::iterator	iter = __mapExpandMenu.find(nCmdID);
	if(	iter != __mapExpandMenu.end() )
	{
		FireEventExtPopupMenu(nCmdID, __ptContextMenuPos.x, __ptContextMenuPos.y);
	}

	return CWnd::OnCommand(wParam, lParam);
}

void CImageWindow_Impl2::__UpdateScale()
{
	m_szScreen	= __GetScreenSize();
	m_szImage	= __GetImageSize();

	ExtZoomFit((short)m_nDefaultZoomFitMode);
}

void CImageWindow_Impl2::__UpdateDisplayInformation()
{
	m_CoordTransform.SetOffsetPost(m_ptScreenOffset);
	m_CoordTransform.SetScale(m_fScaleX, m_fScaleY);

	int nScreenStartX = 0;
	int nScreenStartY = 0;

	int nScreenSizeX = 0;
	int nScreenSizeY = 0;

	int nImageStartX = 0;
	int nImageStartY = 0;

	int nImageSizeX	= ScreenToImageCX((float)m_szScreen.cx);
	int nImageSizeY = ScreenToImageCY((float)m_szScreen.cy);

	int nImageToScreenPosX = m_ptScreenOffset.x;
	int nImageToScreenPosY = m_ptScreenOffset.y;

	BOOL bZoomX = nImageSizeX < m_szImage.cx;	// X축 Zoom 상태
	BOOL bZoomY = nImageSizeY < m_szImage.cy;	// Y축 Zoom 상태

	if( bZoomX )
	{
		nScreenSizeX	= m_szScreen.cx;
		nImageStartX	= ScreenToImageCX((float) - nImageToScreenPosX);
		nImageSizeX		= ScreenToImageCX((float)nScreenSizeX) + 2;
		nScreenStartX	= ImageToScreenCX((float)nImageStartX) + nImageToScreenPosX;
		nScreenSizeX	= ImageToScreenCX((float)nImageSizeX);
	}
	else
	{
		nScreenStartX	= nImageToScreenPosX;
		nScreenSizeX	= ImageToScreenCX((float)m_szImage.cx);
		nImageStartX	= 0;
		nImageSizeX		= m_szImage.cx;
	}

	if( bZoomY )
	{
		nScreenSizeY	= m_szScreen.cy;
		nImageStartY	= ScreenToImageCY((float) - nImageToScreenPosY);
		nImageSizeY		= ScreenToImageCY((float)nScreenSizeY) + 2;
		nScreenStartY	= ImageToScreenCY((float)nImageStartY) + nImageToScreenPosY;
		nScreenSizeY	= ImageToScreenCY((float)nImageSizeY);

	}
	else
	{
		nScreenStartY	= nImageToScreenPosY;
		nScreenSizeY	= ImageToScreenCY((float)m_szImage.cy);
		nImageStartY	= 0;
		nImageSizeY		= m_szImage.cy;
	}

	m_rtDC	= CRect(nScreenStartX, nScreenStartY, nScreenStartX + nScreenSizeX, nScreenStartY + nScreenSizeY);
	m_rtDIB	= CRect(nImageStartX, nImageStartY, nImageStartX + nImageSizeX, nImageStartY + nImageSizeY);
}

CPoint CImageWindow_Impl2::ScreenToImage(CPoint point)
{
	m_CoordTransform.ReverseTransform(point);

	return point;
}

CPoint CImageWindow_Impl2::ImageToScreen(CPoint point)
{
	m_CoordTransform.Transform(point);

	return point;
}

CPoint CImageWindow_Impl2::ClientToImage(CPoint point)
{
	point.y -= GetToolBarSizeY();

	return ScreenToImage(point);
}

CPoint CImageWindow_Impl2::ImageToClient(CPoint point)
{
	CPoint ptScreen = ImageToScreen(point);

	ptScreen.y += GetToolBarSizeY();

	return ptScreen;
}

CPoint CImageWindow_Impl2::ImageToScreen(float x, float y)
{	
	CPoint pt;
	if(m_bUseRangeImage)
	{
		pt.x = (long)((x - m_rtRangeImage.left) * m_fScaleX + 0.5f);
		pt.y = (long)((y - m_rtRangeImage.top) * m_fScaleY + 0.5f);
	}
	else
	{
		pt.SetPoint((long)((x + 0.5f) * m_fScaleX + 0.5f), (long)((y + 0.5f) * m_fScaleY + 0.5f));
	}

	pt += m_ptScreenOffset;

	return pt;
}

CPoint CImageWindow_Impl2::ImageToClient(float x, float y)
{
	CPoint ptScreen = ImageToScreen(x, y);

	ptScreen.y += GetToolBarSizeY();

	return ptScreen;
}

void CImageWindow_Impl2::__DrawImage(CDC *pDC)
{
	if( !pDC) return;

	HBITMAP hBitmap = __MakeDDB(pDC,
	                            m_imgShowSelImage.pbyImage,
	                            m_imgShowSelImage.nWidth,
	                            m_imgShowSelImage.nHeight,
	                            m_imgShowSelImage.nWidthBytes,
	                            m_imgShowSelImage.nBpp,
	                            TRUE,
	                            m_pPaletteColor);


	CRect rtDC(m_rtDC);
	CRect rtDIB(m_rtDIB);

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	HBITMAP hOldBitmap = (HBITMAP)MemDC.SelectObject(hBitmap);

	__DrawTrace(&MemDC, m_pMouseTrace);

	int nOldStretchMode = pDC->SetStretchBltMode(m_iStretchMode);

	pDC->StretchBlt(rtDC.left, rtDC.top, rtDC.Width(), rtDC.Height(), &MemDC, rtDIB.left, rtDIB.top, rtDIB.Width(), rtDIB.Height(), SRCCOPY);

	pDC->SetStretchBltMode(nOldStretchMode);

	MemDC.SelectObject(hOldBitmap);
	MemDC.DeleteDC();
	DeleteObject(hBitmap);
}

HBITMAP CImageWindow_Impl2::__MakeDDB(CDC *pDC, const unsigned char *imgdata, long width, long height, long widthBytes, int bpp, BOOL bTopToBottom, OLE_COLOR *pPaletteColor)
{
	char buf[ sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 ];					// 비트맵 헤더정보와 팔레트 정보를 셋팅할 버퍼
	BITMAPINFOHEADER *dibhdr = (BITMAPINFOHEADER *)buf;							// 비트맵 인포헤더
	COLORREF *rgb = (COLORREF *)( buf + sizeof(BITMAPINFOHEADER) );				// 팔레트

	if(bpp == 8)
	{
		if(pPaletteColor)
		{
			for(int i = 0; i < 256; i++)
			{
				long nR = GetBValue(pPaletteColor[i]);
				long nG = GetGValue(pPaletteColor[i]);
				long nB = GetRValue(pPaletteColor[i]);

				rgb[i] = RGB(nR, nG, nB);						// 그레이 팔레트 만들기 */
			}
		}
		else
		{
			for(int i = 0; i < 256; i++) rgb[i] = RGB( i, i, i );							// 그레이 팔레트 만들기 */
		}
	}

	dibhdr->biSize = sizeof( BITMAPINFOHEADER );								// 사이즈
	dibhdr->biWidth = bpp ? widthBytes / (bpp / 8) : 0;							// 가로길이
	dibhdr->biHeight = bTopToBottom ? -height : height;							// 세로길이
	dibhdr->biPlanes = 1;														// 플레인 수
	dibhdr->biBitCount = (WORD)bpp;												// 비트 퍼 픽셀
	dibhdr->biCompression = BI_RGB;												// 압축 여부
	dibhdr->biSizeImage = widthBytes * height;									// 이미지 사이즈
	dibhdr->biXPelsPerMeter = 0;
	dibhdr->biYPelsPerMeter = 0;
	dibhdr->biClrUsed = 0;
	dibhdr->biClrImportant = 0;

	HBITMAP hBitmap = CreateDIBitmap(pDC->m_hDC, dibhdr, CBM_INIT, (void *)imgdata, (BITMAPINFO *)dibhdr, DIB_RGB_COLORS);

	return hBitmap;
}

void CImageWindow_Impl2::__DrawTrace(CDC *pMemDC, CMouseTrace *pMouseTrace)
{
	if( !__IsTraceEnable() ) return;

	if( !pMemDC || !pMouseTrace ) return;

	pMouseTrace->Lock();

	vector<Point> &PointArray = pMouseTrace->m_PointArray;

	if( PointArray.empty() ) return;

	long		nPenWidth	= pMouseTrace->m_nPenWidth;
	Color		argbTrace	= pMouseTrace->m_argb;

	if( PointArray.size() == 1 )
	{
		// 임시코드 - nPenWidth에 맞게 점 찍기
		long nSizeX = nPenWidth;
		long nSizeY = nPenWidth;
		long nBeginX = - nSizeX / 2;
		long nBeginY = - nSizeY / 2;

		long nEndX = nSizeX + nBeginX;
		long nEndY = nSizeY + nBeginY;

		long i = 0;

		for(long y = nBeginY ; y < nEndY ; ++y )
		{
			for(long x = nBeginX ; x < nEndX ; ++x, ++i )
			{
				Point offset(x, y);

				pMemDC->SetPixel(PointArray[0].X + x, PointArray[0].Y + y, argbTrace.ToCOLORREF());
			}
		}
	}
	else
	{
		Graphics gdi(pMemDC->GetSafeHdc());

		Pen mypen(argbTrace, (Gdiplus::REAL)m_nTraceWidth);

		mypen.SetLineCap(LineCapSquare, LineCapSquare, DashCapFlat );
		mypen.SetLineJoin(LineJoinRound );

		gdi.DrawLines(&mypen, &PointArray[0], (INT)PointArray.size());
	}

	pMouseTrace->Unlock();
}

void CImageWindow_Impl2::__TraceBegin(const CPoint &ptImage)
{
	if( !__IsTraceEnable() || !m_pMouseTrace) return;

	//m_pMouseTrace->Init(m_rgbTrace, m_nTraceWidth);
	m_pMouseTrace->Init(m_argbTrace, m_nTraceWidth);
	m_pMouseTrace->WayPoint(ptImage.x, ptImage.y);
}

CRect CImageWindow_Impl2::__Trace(const CPoint &ptImage)
{
	if( !__IsTraceEnable() || !m_pMouseTrace ) return CRect(0, 0, 0, 0);

	m_pMouseTrace->WayPoint(ptImage.x, ptImage.y);

	CRect rtBound = m_pMouseTrace->Boundary();

	if( !rtBound.IsRectEmpty() )
	{
		CRect rtInvalidate(ImageToClient(rtBound.TopLeft()), ImageToClient(rtBound.BottomRight()));

		rtInvalidate.NormalizeRect();

		return rtInvalidate;
	}
	else
	{
		//Invalidate(FALSE);
	}

	return CRect(0, 0, 0, 0);
}

void CImageWindow_Impl2::__TraceEnd()
{
	if( !__IsTraceEnable() || !m_pMouseTrace ) return;

	m_pMouseTrace->Lock();

	__TraceExport();

	m_pMouseTrace->m_PointArray.clear();

	m_pMouseTrace->Unlock();
}

void CImageWindow_Impl2::__TraceExport()
{
	vector<Point> &PointArray = m_pMouseTrace->m_PointArray;

	m_PointArrayExport.resize(PointArray.size());
	for(long i = 0 ; i < (long)PointArray.size() ; ++i )
	{
		m_PointArrayExport[i].x = PointArray[i].X;
		m_PointArrayExport[i].y = PointArray[i].Y;
	}
}

void CImageWindow_Impl2::SetTraceColor(BYTE a, BYTE r, BYTE g, BYTE b)
{
	m_argbTrace = Color(a, r, g, b);
}

void CImageWindow_Impl2::SetTraceWidth(int nTraceWidth)
{
	m_nTraceWidth = nTraceWidth;
}

void CImageWindow_Impl2::GetMouseTrace(CPoint **ppVertice, long *pnVertice)
{
	if( !ppVertice || !pnVertice || !__IsTraceEnable() ) return;

	*ppVertice = &m_PointArrayExport[0];
	*pnVertice = (long)m_PointArrayExport.size();
}

float	CImageWindow_Impl2::__GetScaleWidthFit()
{
	int cxScreen = m_szScreen.cx;
	int cxImage = m_szImage.cx;

	return (float)cxScreen / (float)cxImage;
}

float	CImageWindow_Impl2::__GetScaleHeightFit()
{
	int cyScreen = m_szScreen.cy;
	int cyImage = m_szImage.cy;

	return (float)cyScreen / (float)cyImage;
}

float	CImageWindow_Impl2::__GetScaleImageFit()
{
	int cxImage = m_szImage.cx;
	int cyImage = m_szImage.cy;

	int cxScreen = m_szScreen.cx;
	int cyScreen = m_szScreen.cy;

	float fScaleX = (float)cxScreen / (float)cxImage;
	float fScaleY = (float)cyScreen / (float)cyImage;

	return min(fScaleX, fScaleY);
}

CSize CImageWindow_Impl2::__GetScreenSize()
{
	CRect rtClient;
	GetClientRect(&rtClient);

	rtClient.bottom -= GetToolBarSizeY();

	return rtClient.Size();
}

CSize CImageWindow_Impl2::__GetImageSize()
{
	return CSize(m_imgShowSelImage.nWidth, m_imgShowSelImage.nHeight);
}

BOOL CImageWindow_Impl2::__IsImageSizeUnderScreen()
{
	return m_szScreen.cx > m_szImage.cx && m_szScreen.cy > m_szImage.cy;
}

long CImageWindow_Impl2::__FindZoomScaleIndex(float fScale)
{
	long nZoomScaleIndex = m_nZoomWheel;

	for(long i = 1 ; i < G_ZOOMROLLNUM ; ++i )
	{
		if( fScale < G_ZOOMROLLING[i])
		{
			nZoomScaleIndex = i - 1;
			break;
		}
	}

	return nZoomScaleIndex;
}

void CImageWindow_Impl2::__Scroll(long nScrollPosX, long nScrollPosY)
{
	__ScrollLimit(nScrollPosX, nScrollPosY);

	if( m_szScroll.cx > 0 ) SetScrollPos(SB_HORZ, nScrollPosX, TRUE);
	if( m_szScroll.cy > 0 ) SetScrollPos(SB_VERT, nScrollPosY, TRUE);

	m_ptScreenOffset.x = -nScrollPosX;
	m_ptScreenOffset.y = -nScrollPosY;

	__UpdateMinimapPostiion();
	__UpdateDisplayInformation();
}

void CImageWindow_Impl2::__ScrollLimit(long &nScrollPosX, long &nScrollPosY)
{
	CSize szScrollRange = m_szScroll;

	if( szScrollRange.cx == 0 )
	{
		nScrollPosX = -__GetScreenOffsetX(m_szScreen.cx / 2.f, m_szImage.cx / 2.f);
	}
	else
	{
		nScrollPosX = max( nScrollPosX, 0);
		nScrollPosX = min( nScrollPosX, szScrollRange.cx + 1);
	}


	if( szScrollRange.cy == 0 )
	{
		nScrollPosY = -__GetScreenOffsetY(m_szScreen.cy / 2.f, m_szImage.cy / 2.f);
	}
	else
	{
		nScrollPosY = max( nScrollPosY, 0);
		nScrollPosY = min( nScrollPosY, szScrollRange.cy + 1);
	}
}

void CImageWindow_Impl2::__ZoomFitWidth()
{
	CRect rtWindow;
	GetWindowRect(&rtWindow);

	rtWindow.DeflateRect(1, 1, 1, 1);
	rtWindow.top += GetToolBarSizeY();
	rtWindow.right -= (__IsVScrollAvailable() ? GetSystemMetrics(SM_CXVSCROLL) : 0);

	m_szScreen = rtWindow.Size();

	m_fScaleX = __GetScaleWidthFit();
	m_fScaleY = __GetScaleWidthFit();

	UpdateScrollRange();

	ExtMove(m_szImage.cx / 2.f, m_szImage.cy / 2.f);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::__ZoomFitImage()
{
	CRect rtWindow;
	GetWindowRect(&rtWindow);

	rtWindow.DeflateRect(1, 1, 1, 1);
	rtWindow.top += GetToolBarSizeY();

	m_szScreen = rtWindow.Size();

	m_fScaleX = __GetScaleImageFit();
	m_fScaleY = __GetScaleImageFit();

	UpdateScrollRange();

	ExtMove(m_szImage.cx / 2.f, m_szImage.cy / 2.f);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::__ZoomFitBound()
{
	CRect rtWindow;
	GetWindowRect(&rtWindow);

	rtWindow.DeflateRect(1, 1, 1, 1);
	rtWindow.top += GetToolBarSizeY();

	m_szScreen = rtWindow.Size();

	m_fScaleX = __GetScaleWidthFit();
	m_fScaleY = __GetScaleHeightFit();

	UpdateScrollRange();

	ExtMove(m_szImage.cx / 2.f, m_szImage.cy / 2.f);

	Invalidate(FALSE);
}

#define MAKEWIDTHBYTES(W___, BC___, ALIGN__)	(( ( ( (W___) * ((BC___) / 8) ) + ((ALIGN__) - 1)) / (ALIGN__)) * (ALIGN__))
BOOL CImageWindow_Impl2::SetDisplayRange(BOOL bUse, long nLeft, long nTop, long nRight, long nBottom)
{
	if(!bUse || nRight <= nLeft || nBottom <= nTop)
	{
		m_imgShowSelImage = m_imgOriginalImage;
		m_ptScreenOffset = CPoint(0, 0);
		m_bUseRangeImage = FALSE;	m_CoordTransform.SetOffsetPre(CPoint(0,0));
		m_szImage = CSize(m_imgOriginalImage.nWidth, m_imgOriginalImage.nHeight);

		// 이미지가 0보다 작거나 같다면 그릴게 없다.
		if(!m_imgOriginalImage.nWidth || !m_imgOriginalImage.nHeight) return FALSE;

		if(m_imgRangeImage.pbyImage)
		{
			delete []m_imgRangeImage.pbyImage;
			m_imgRangeImage.pbyImage = nullptr;
		}
		return FALSE;
	}
	else
	{
		CRect rtNormalROI(nLeft, nTop, nRight, nBottom);
		rtNormalROI &= CRect(0, 0, m_imgOriginalImage.nWidth, m_imgOriginalImage.nHeight);

		int nWidth			= (long)(rtNormalROI.right  - rtNormalROI.left);
		int nHeight			= (long)(rtNormalROI.bottom - rtNormalROI.top);
		int nWidthBytes		= MAKEWIDTHBYTES(nWidth, m_imgOriginalImage.nBpp, 4);
		int nImageSize		= nWidthBytes * nHeight;

		// 이미지가 0보다 작거나 같다면 그릴게 없다.
		if(!nWidth || !nHeight || !nWidthBytes || !nImageSize) return FALSE;

		int nSrcLeft		= (rtNormalROI.left > 0) ? rtNormalROI.left : 0;
		int nSrcTop			= (rtNormalROI.top > 0) ? rtNormalROI.top : 0;
		int nCopyWidthBytes	= ((nRight < m_imgOriginalImage.nWidth)  ? rtNormalROI.right - nSrcLeft : m_imgOriginalImage.nWidth - nSrcLeft) * (m_imgOriginalImage.nBpp / 8);
		int nCopyHeight		= (nBottom < m_imgOriginalImage.nHeight) ? rtNormalROI.bottom - nSrcTop : m_imgOriginalImage.nHeight - nSrcTop;
		int nDestLeft		= (rtNormalROI.left > 0) ? 0 : -rtNormalROI.left;
		int nDestTop		= (rtNormalROI.top > 0) ? 0 : -rtNormalROI.top;

		if(m_imgRangeImage.pbyImage)
		{
			delete []m_imgRangeImage.pbyImage;
			m_imgRangeImage.pbyImage = nullptr;
		}

		// Memory Alloc and Copy
		BYTE *pDestPtr = new BYTE[nImageSize];
		memset(pDestPtr, 0, nImageSize);

		const BYTE *pStartSrcPtr	= m_imgOriginalImage.pbyImage + (nSrcTop * m_imgOriginalImage.nWidthBytes + (nSrcLeft * m_imgOriginalImage.nBpp / 8));
		BYTE *pStartDestPtr			= pDestPtr + (nDestTop * nWidthBytes + (nDestLeft * m_imgOriginalImage.nBpp / 8));
		for(long y = 0; y < nCopyHeight; y++)
		{
			memcpy(pStartDestPtr, pStartSrcPtr, nCopyWidthBytes);
			pStartDestPtr += nWidthBytes;
			pStartSrcPtr += m_imgOriginalImage.nWidthBytes;
		}

		// Image Info Setting
		m_imgRangeImage.pbyImage = pDestPtr;
		m_imgRangeImage.nWidth = nWidth;
		m_imgRangeImage.nHeight = nHeight;
		m_imgRangeImage.nBpp = m_imgOriginalImage.nBpp;
		m_imgRangeImage.nImageSize = nImageSize;
		m_imgRangeImage.nWidthBytes = nWidthBytes;

		m_imgShowSelImage = m_imgRangeImage;
		m_bUseRangeImage = TRUE; m_CoordTransform.SetOffsetPre(rtNormalROI.TopLeft());
		m_rtRangeImage = rtNormalROI;//SetRect(nLeft, nTop, nRight, nBottom);

		m_szImage = CSize(nWidth, nHeight);
		m_ptScreenOffset = CPoint(0, 0);
	}

	UpdateScrollRange();
	__UpdateDisplayInformation();

	Invalidate(TRUE);

	return TRUE;
}

void CImageWindow_Impl2::EnablePixelMeter(BOOL bEnable)
{
	m_PixelMeter.Enable(bEnable);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::SetPixelMeterUnit(float fOnePixelUnit, LPCTSTR lpszUnitName)
{
	m_PixelMeter.SetUnit(fOnePixelUnit, lpszUnitName);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::SetPixelMeterBegin(const CPoint &ptBegin)
{
	if( m_PixelMeter.IsEnable() ) m_PixelMeter.SetBegin(ptBegin);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::SetPixelMeterEnd(const CPoint &ptEnd)
{
	if( m_PixelMeter.IsEnable() ) m_PixelMeter.SetEnd(ptEnd);

	Invalidate(FALSE);
}

void CImageWindow_Impl2::Screenshot(BOOL bShotImage, BOOL bShotROI, CBitmap& o_ddb)
{
	CRect rcBounds(0, 0, 0, 0);
	this->GetClientRect(&rcBounds);

	rcBounds.top = GetToolBarSizeY();

	if(rcBounds.Width() <= 0 || rcBounds.Height() <= 0)	return;

	CRect rtBounds(0, 0, rcBounds.Width(), rcBounds.Height());

	CDC MemDC;
	MemDC.CreateCompatibleDC(GetDC());	if( MemDC.GetSafeHdc() == NULL ) return;

	HANDLE handle1 = m_BitmapScreenshot.GetSafeHandle();

	if( handle1 == NULL )
	{
		m_BitmapScreenshot.CreateCompatibleBitmap(GetDC(), rtBounds.Width(), rtBounds.Height());	if( m_BitmapScreenshot.GetSafeHandle() == NULL ) return;
		m_BitmapScreenshot.SetBitmapDimension(rtBounds.Width(), rtBounds.Height());
	}
	else
	{
		CSize dimension = m_BitmapScreenshot.GetBitmapDimension();

		if( dimension != CSize(rtBounds.Width(), rtBounds.Height()) )
		{
			m_BitmapScreenshot.DeleteObject();
			m_BitmapScreenshot.CreateCompatibleBitmap(GetDC(), rtBounds.Width(), rtBounds.Height());	if( m_BitmapScreenshot.GetSafeHandle() == NULL ) return;
			m_BitmapScreenshot.SetBitmapDimension(rtBounds.Width(), rtBounds.Height());
		}
	}

	CBitmap *pOldBitmap = MemDC.SelectObject(&m_BitmapScreenshot);

	CBrush brush;
	brush.CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 255));	if( brush.GetSafeHandle() == NULL ) return;
	SetBkColor(MemDC.GetSafeHdc(), RGB(128, 128, 128));
	MemDC.FillRect(rtBounds, &brush);

	if( bShotImage )
	{
		__DrawImage(&MemDC);
	}

	if( bShotROI )
	{
		uOverlayDrawAll( &MemDC, m_isOverlayDraw, m_bDisplayCrossMark );
		uDrawROIs( &MemDC );
		uDrawROIEditor( &MemDC , m_pROIEditor);
		uDrawCurrentPosition( &MemDC );
		uDrawPixelMeter(&MemDC);
		uDrawMinimap(&MemDC, rtBounds);		
	}

	CRect rtDC = m_rtDC;
	CDC MemDCDst;
	CBitmap& BitmapDst = o_ddb;
	MemDCDst.CreateCompatibleDC(GetDC());	if( MemDCDst.GetSafeHdc() == NULL ) return;

	HANDLE handle = BitmapDst.GetSafeHandle();

	if( handle == NULL )
	{
		BitmapDst.CreateCompatibleBitmap(GetDC(), rtDC.Width(), rtDC.Height());	if( BitmapDst.GetSafeHandle() == NULL ) return;
		BitmapDst.SetBitmapDimension(rtDC.Width(), rtDC.Height());
	}
	else
	{
		CSize dimension = BitmapDst.GetBitmapDimension();

		if( dimension != CSize(rtDC.Width(), rtDC.Height()) )
		{
			BitmapDst.DeleteObject();
			BitmapDst.CreateCompatibleBitmap(GetDC(), rtDC.Width(), rtDC.Height());	if( BitmapDst.GetSafeHandle() == NULL ) return;
			BitmapDst.SetBitmapDimension(rtDC.Width(), rtDC.Height());
		}
	}

	CBitmap *pOldBitmapDst = MemDCDst.SelectObject(&BitmapDst);

	MemDCDst.BitBlt(0, 0, rtDC.Width(), rtDC.Height(), &MemDC, rtDC.left, rtDC.top, SRCCOPY);

	MemDCDst.SelectObject(pOldBitmapDst);
	MemDCDst.DeleteDC();

	MemDC.SelectObject(pOldBitmap);
	MemDC	.DeleteDC();
}

void CImageWindow_Impl2::OnShowtoolbar()
{
	ShowToolBar(m_wndToolBar.IsVisible() ? FALSE : TRUE);

	__UpdateScale();

	Invalidate(FALSE);
}

BOOL CImageWindow_Impl2::OnToolTipText(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// if there is a top level routing frame then let it handle the message
	if (GetRoutingFrame() != NULL) return FALSE;

	// to be thorough we will need to handle UNICODE versions of the message also !!
	TOOLTIPTEXTA *pTTTA = (TOOLTIPTEXTA *)pNMHDR;
	TOOLTIPTEXTW *pTTTW = (TOOLTIPTEXTW *)pNMHDR;
	UINT_PTR nID = (UINT)pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
	    pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID((HWND)nID);
	}

	if (nID != 0) // will be zero on a separator
	{
		CString strTipText;

		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		strTipText.LoadString((UINT) nID);

#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			_wcstombsz(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#endif

		*pResult = 0;

		// bring the tooltip window above other popup windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		               SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);

		return TRUE;
	}

	return FALSE;
}

BOOL CImageWindow_Impl2::__CreateToolBar()
{
	DWORD style = WS_CHILD | WS_VISIBLE | CBRS_TOOLTIPS | (CBRS_ALIGN_TOP & CBRS_ALIGN_ANY);

	if( !m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, style) )	return FALSE;
	if( !m_wndToolTip.Create(this) )						return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !m_wndToolBar.LoadToolBar(IDR_TOOLBAR_IMAGEWINDOW) )	return FALSE;

	CImageList imageList;
	CBitmap bitmap;

	//Create and set toolbar image list.
	bitmap.LoadBitmap(IDB_TOOLBAR);
	imageList.Create(24, 24, ILC_COLORDDB | ILC_MASK, 13, 1);
	imageList.Add(&bitmap, RGB(255, 0, 255));
	m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();
	bitmap.Detach();

	imageList.Detach();
	bitmap.Detach();

	return TRUE;
}

BOOL CImageWindow_Impl2::__IsScrollRangeUpdating()
{
	return m_bScrollRangeUpdating;
}

void CImageWindow_Impl2::__SetScrollRangeUpdating(BOOL bUpdating)
{
	m_bScrollRangeUpdating = bUpdating;
}

void CImageWindow_Impl2::__MoveView(int x, int y, int cx, int cy)
{
	UpdateScrollRange();
	__UpdateDisplayInformation();
	__UpdateMinimapPostiion();

	Invalidate(FALSE);

	if(m_pChildWindow)
	{
		CRect rtView(0, 0, 0, 0);
		GetClientRect(rtView);

		m_pChildWindow->MoveWindow(x, y, cx, cy);
		m_pChildWindow->Invalidate(FALSE);
	}

}

BOOL CImageWindow_Impl2::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( !__CreateToolBar() ) return FALSE;

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_wndToolBar.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CImageWindow_Impl2::ShowToolBar(BOOL bShow)
{
	m_wndToolBar.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

int CImageWindow_Impl2::GetToolBarSizeY()
{
	if( !m_wndToolBar.IsVisible() ) return 0;

	CRect rtToolBar(0, 0, 0, 0);
	m_wndToolBar.GetClientRect(&rtToolBar);

	return rtToolBar.Height();
}

BOOL CImageWindow_Impl2::__IsMinimapShowing()
{
	return __IsHScrollAvailable() || __IsVScrollAvailable();
}

BOOL CImageWindow_Impl2::__UpdateMinimapPostiion()
{
	if( !__IsMinimapEnabled() ) return FALSE;

	__pMinimap->SetCurrentViewPos(CRect(ScreenToImage(CPoint(0, 0)), ScreenToImage(CPoint(m_szScreen.cx, m_szScreen.cy))));

	return TRUE;
}

BOOL CImageWindow_Impl2::__IsMinimapEnabled()
{
	return __bMinimapEnabled;
}

CString CImageWindow_Impl2::__MakeImagePositionString(CPoint point)
{
	CRect rtImageBoundary(0, 0, m_imgShowSelImage.nWidth, m_imgShowSelImage.nHeight);

	CString str;
	if( !rtImageBoundary.PtInRect(point) || m_imgShowSelImage.nBpp != 8 )
	{
		str.Format(_T("[%d,%d]"), point.x, point.y);
	}
	else if( !IsPixelAccessible(m_imgShowSelImage.pbyImage, point.x, point.y, m_imgShowSelImage.nWidthBytes) )
	{
		str.Format(_T("[%d,%d]"), point.x, point.y);
	}
	else
	{
		const BYTE *pPtr = m_imgShowSelImage.pbyImage;
		str.Format(_T("[%d,%d] %d"), point.x, point.y, pPtr[m_imgShowSelImage.nWidthBytes * point.y + point.x]);
	}

	return str;
}

BOOL CImageWindow_Impl2::__Zoom(long nDiffIndex, CPoint point)
{
	long nCurrIndex = m_nZoomWheel;
	long nNextIndex = m_nZoomWheel + nDiffIndex;

	nNextIndex = max(nNextIndex, 0);
	nNextIndex = min(nNextIndex, G_ZOOMROLLNUM - 1);

	if( nCurrIndex == nNextIndex )
	{
		return FALSE;
	}

	float fNextScaleX = G_ZOOMROLLING[nNextIndex];
	float fNextScaleY = G_ZOOMROLLING[nNextIndex];

	float fNextSizeX = m_szImage.cx * fNextScaleX;
	float fNextSizeY = m_szImage.cy * fNextScaleY;

	float fFitSizeX = (float)m_szScreen.cx;
	float fFitSizeY = (float)m_szScreen.cy;

	float fDivSizeX = fabs(fNextSizeX - fFitSizeX);
	float fDivSizeY = fabs(fNextSizeY - fFitSizeY);

	BOOL fail_to_zoom_x = fDivSizeX < 1.f;	// size차이가 fitsize와 1px이하이면 fail
	BOOL fail_to_zoom_y = fDivSizeY < 1.f;

	if( fail_to_zoom_x || fail_to_zoom_y )
	{
		nNextIndex += nDiffIndex;

		nNextIndex = max(nNextIndex, 0);
		nNextIndex = min(nNextIndex, G_ZOOMROLLNUM - 1);

		if( nCurrIndex == nNextIndex )
		{
			return FALSE;
		}

		fNextScaleX = G_ZOOMROLLING[nNextIndex];
		fNextScaleY = G_ZOOMROLLING[nNextIndex];
	}

	m_nZoomWheel = nNextIndex;

	if( !__Zoom(fNextScaleX, fNextScaleY, point) )
	{
		return FALSE;
	}

	return TRUE;
}
BOOL CImageWindow_Impl2::__Zoom(float fScaleX, float fScaleY, CPoint point)
{
	CPoint ptCurrentCenter = ScreenToImage(CPoint(m_szScreen.cx / 2, m_szScreen.cy / 2));

	if( !__IsImageSizeUnderScreen() )
	{
		float fFitScaleX = __GetScaleWidthFit();
		float fFitScaleY = __GetScaleHeightFit();

		if( fScaleX < fFitScaleX && fScaleY < fFitScaleY )
		{
			ExtZoomFit((short)m_nDefaultZoomFitMode);

			return TRUE;
		}
	}
	else
	{
		if( fScaleX < 1.f && fScaleY < 1.f )
		{
			m_nZoomWheel = __FindZoomScaleIndex(1.f);

			OnZoom1x();
			return TRUE;
		}
	}

	if(fScaleX == -1.f && fScaleY == -1.f)
	{
		// 현재 여기로 들어오는 경우 없음 : Scale최소값을 위쪽에서 제한을 걸어놓음
		ExtZoomFit((short)m_nDefaultZoomFitMode);
	}
	else
	{
		if( m_fScaleX == fScaleX && m_fScaleY == fScaleY ) return FALSE;

		CPoint ptImagePrev = ScreenToImage(point);

		m_fScaleX = fScaleX;
		m_fScaleY = fScaleY;

		m_CoordTransform.SetScale(m_fScaleX, m_fScaleY);

		CPoint ptImageCurr = ScreenToImage(point);

		CPoint ptImageOffset = ptImageCurr - ptImagePrev;

		UpdateScrollRange();

		__Scroll(	-(m_ptScreenOffset.x + ImageToScreenCX((float)ptImageOffset.x)),
		            -(m_ptScreenOffset.y + ImageToScreenCY((float)ptImageOffset.y)));

		Invalidate(FALSE);
	}

	return TRUE;
}

BOOL CImageWindow_Impl2::PreTranslateMessage(MSG *pMsg)
{
	if( FireEventKeyEvent(pMsg) ) return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CImageWindow_Impl2::OnPopupShowminimap()
{
	EnableMinimap(!__IsMinimapEnabled(), CSize(100, 100));
}

void CImageWindow_Impl2::OnPopupShowpixelmeter()
{
	EnablePixelMeter(!m_PixelMeter.IsEnable());
}


void CImageWindow_Impl2::OnExportFile()
{
	CFileDialog dlg(FALSE, _T("*.bmp"), NULL, OFN_HIDEREADONLY, _T("BMP Image File (*.bmp)|*.bmp|PNG Image File (*.png)|*.png|JPEG Image File (*.jpg)|*.jpg||"));

	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	CString strExt = dlg.GetFileExt();
	strExt.MakeUpper();

	long nImageSize = m_imgShowSelImage.nWidthBytes * m_imgShowSelImage.nHeight;

	if(nImageSize > 0)
	{
		BYTE *pSaveData = new BYTE[nImageSize];
		memcpy(pSaveData, m_imgShowSelImage.pbyImage, nImageSize);

		if(strExt == _T("BMP"))
		{
			CImageFileFormat::ImageSaveBMP(dlg.GetPathName(), pSaveData, m_imgShowSelImage.nWidth, m_imgShowSelImage.nHeight, m_imgShowSelImage.nBpp, m_imgShowSelImage.nWidthBytes, CRect(0, 0, m_imgShowSelImage.nWidth, m_imgShowSelImage.nHeight), TRUE);
		}
		else if(strExt == _T("PNG"))
		{
			CImageFileFormat::ImageSavePNG(dlg.GetPathName(), pSaveData, m_imgShowSelImage.nWidth, m_imgShowSelImage.nHeight, m_imgShowSelImage.nBpp, m_imgShowSelImage.nWidthBytes);
		}
		else if(strExt == _T("JPG") || strExt == _T("JPEG"))
		{
			CImageFileFormat::ImageSaveJPG(dlg.GetPathName(), pSaveData, m_imgShowSelImage.nWidth, m_imgShowSelImage.nHeight, m_imgShowSelImage.nBpp, m_imgShowSelImage.nWidthBytes, CRect(0, 0, m_imgShowSelImage.nWidth, m_imgShowSelImage.nHeight), 99);
		}

		delete []pSaveData;
	}
}


void CImageWindow_Impl2::OnExportClipboard()
{
	CDC*		pDC		= GetDC();
	const BYTE* pPixel	= m_imgOriginalImage.pbyImage;
	long		nStepX	= m_imgOriginalImage.nWidthBytes;
	long		nSizeX	= m_imgOriginalImage.nWidth;
	long		nSizeY	= m_imgOriginalImage.nHeight;
	long		nBpp	= m_imgOriginalImage.nBpp;

	if(pPixel == NULL) return;

	char buf[ sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256 ];		// 비트맵 헤더정보와 팔레트 정보를 셋팅할 버퍼
	BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER*)buf;				// 비트맵 인포헤더
	COLORREF* rgb = (COLORREF*)( buf + sizeof(BITMAPINFOHEADER) );	// 팔레트

	if(nBpp == 8)
	{
		// 그레이 팔레트 만들기
		for(int i = 0; i < 256; ++i) rgb[i] = RGB( i,i,i );
	}

	dibhdr->biSize = sizeof( BITMAPINFOHEADER );	// 사이즈 
	dibhdr->biWidth = nStepX / (nBpp/8);			// 가로길이 
	dibhdr->biHeight = -nSizeY;						// 세로길이 
	dibhdr->biPlanes = 1;							// 플레인 수 
	dibhdr->biBitCount = (WORD)nBpp;				// 비트 퍼 픽셀 
	dibhdr->biCompression = BI_RGB;					// 압축 여부 
	dibhdr->biSizeImage = nStepX *  nSizeY;			// 이미지 사이즈 
	dibhdr->biXPelsPerMeter = 0;												
	dibhdr->biYPelsPerMeter = 0;
	dibhdr->biClrUsed = 0;
	dibhdr->biClrImportant = 0; 

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, nSizeX, nSizeY);

	CBitmap *pOldBitmap = MemDC.SelectObject(&bitmap);

	// 팔레트를 얻어 DC에 선택
	::SetStretchBltMode(MemDC.m_hDC, COLORONCOLOR);

	::StretchDIBits(MemDC.m_hDC, 	// hDC
		0,							// DestX
		0,							// DestY
		nSizeX,						// nDestWidth
		nSizeY,						// nDestHeight
		0,							// SrcX
		0,							// SrcY
		nSizeX,						// wSrcWidth
		nSizeY,						// wSrcHeight
		pPixel,						// lpBits
		(LPBITMAPINFO)dibhdr,		// lpBitsInfo
		DIB_RGB_COLORS,				// wUsage
		SRCCOPY);					// dwROP

	MemDC.SelectObject(pOldBitmap);

	::OpenClipboard(GetSafeHwnd());
	::EmptyClipboard();
	::SetClipboardData(CF_BITMAP, bitmap.GetSafeHandle());
	::CloseClipboard();
	bitmap.Detach();}


void CImageWindow_Impl2::OnPopupScreenshot()
{
	CBitmap Bitmap;
	Screenshot(TRUE, TRUE, Bitmap);

	::OpenClipboard(GetSafeHwnd());
	::EmptyClipboard();
	::SetClipboardData(CF_BITMAP, Bitmap.GetSafeHandle());
	::CloseClipboard();

	Bitmap	.DeleteObject();
}

BOOL CImageWindow_Impl2::IsImageValid() const
{
	return (m_imgShowSelImage.pbyImage && m_imgShowSelImage.nWidth > 0 && m_imgShowSelImage.nHeight > 0);
}

void CImageWindow_Impl2::OnUpdatePopupScreenshot(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsImageValid());
}

void CImageWindow_Impl2::OnUpdateExportFile(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsImageValid());
}

void CImageWindow_Impl2::OnUpdateExportClipboard(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsImageValid());
}
