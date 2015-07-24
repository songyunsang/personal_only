// xGerberViewCtrl.cpp : CImageWindow ActiveX 컨트롤 클래스의 구현입니다.

#include "stdafx.h"
#include "ImageWindowApp.h"
#include "ImageWindow.h"
#include "ImageWindowEvt.h"
#include "ImageWindow_Impl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 메시지 맵입니다.
BEGIN_MESSAGE_MAP(CImageWindow, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CImageWindow::CImageWindow(CImageWindowEvt *pHandler)
: m_pImpl(new CImageWindow_Impl(pHandler))
{
}

CImageWindow::~CImageWindow()
{
	if(m_pImpl)
	{
		delete m_pImpl;
		m_pImpl = nullptr;
	}
}

void CImageWindow::SetEventHandler(CImageWindowEvt *pHandler)
{
	m_pImpl->SetEventHandler(pHandler);
}

int CImageWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !__Create() ) return -1;

	m_pImpl->SetParent(this);	// __Create함수에서 this를 넣으면 ASSERT! 따로 부모를 정해주자

	return 0;
}

void CImageWindow::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(m_pImpl->GetSafeHwnd()) 
	{
		m_pImpl->MoveWindow(0, 0, cx, cy);
	}
}

void CImageWindow::OnDestroy()
{
	CWnd::OnDestroy();

	m_pImpl->DestroyWindow();
}

BOOL CImageWindow::Create(CWnd *pwndParent, const CRect rtPosition)
{
	ASSERT(pwndParent);

	CRect rectDummy(0, 0, 0, 0);
	if(rtPosition.IsRectEmpty() || rtPosition.IsRectNull())
	{
		pwndParent->GetClientRect(&rectDummy);
	}
	else
	{
		rectDummy = rtPosition;
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWnd tmpWnd;
	tmpWnd.Attach(pwndParent->GetSafeHwnd());

	if(!CWnd::Create(NULL, NULL, WS_VISIBLE, rectDummy, &tmpWnd, 0, NULL))
	{
		tmpWnd.Detach();
		return FALSE;
	}

	tmpWnd.Detach();

	return TRUE;
}

BOOL CImageWindow::Create(CDialog *pwndParent, UINT nFrameControlID)
{
	ASSERT(pwndParent);
	CRect rectDummy(0, 0, 0, 0);
	if(nFrameControlID > 0)
	{
		pwndParent->GetDlgItem(nFrameControlID)->GetWindowRect(&rectDummy);
		pwndParent->ScreenToClient(rectDummy);
	}
	else
	{
		pwndParent->GetClientRect(&rectDummy);
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWnd tmpWnd;
	tmpWnd.Attach(pwndParent->GetSafeHwnd());

	if(!CWnd::Create(NULL, NULL, WS_VISIBLE, rectDummy, pwndParent, 0, NULL))
	{
		tmpWnd.Detach();
		return FALSE;
	}

	tmpWnd.Detach();

	return TRUE;
}

BOOL CImageWindow::GetImage(const BYTE **ppbuf, long* width, long* height, long* depth, long* widthBytes)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->GetImage(ppbuf, width, height, depth, widthBytes);
}

void CImageWindow::OverlayShow(BOOL bShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayShow(bShow);
}

void CImageWindow::OverlaySetLineWidth(short nLineWidth)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlaySetLineWidth(nLineWidth);
}

void CImageWindow::OverlayAddPoint(const CPoint *pPt, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddPoint(pPt, crColor);
}

void CImageWindow::OverlayAddPoints(const CPoint *pPt, long nCount, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddPoints(pPt, nCount, crColor);
}

void CImageWindow::OverlayAddLine(const CPoint *pPt1, const CPoint *pPt2, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddLine(pPt1, pPt2, crColor);
}

void CImageWindow::OverlayAddRectangle(const CRect *pRect, OLE_COLOR crColor, float rot)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddRectangle(pRect, crColor, rot);
}

void CImageWindow::OverlayAddRectangle(const CPoint *pLT, const CPoint *pRT, const CPoint *pRB, const CPoint *pLB, OLE_COLOR crColor, float rot)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddRectangle(pLT, pRT, pRB, pLB, crColor, rot);
}

void CImageWindow::OverlayAddRectangles(const CRect *pRect, long nCount, OLE_COLOR crColor, float rot)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddRectangles(pRect, nCount, crColor, rot);
}

void CImageWindow::OverlayAddFText(float fX, float fY, LPCTSTR str, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddFText(fX, fY, str, crColor);
}

void CImageWindow::OverlayAddText(const CPoint *pPt, LPCTSTR str, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddText(pPt, str, crColor);
}

void CImageWindow::OverlayAddTextEx(const CPoint *pPt, LPCTSTR str, LOGFONT *pFont, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddTextEx(pPt, str, pFont, crColor);
}

void CImageWindow::OverlayRemovePoint()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayRemovePoint();
}

void CImageWindow::OverlayRemoveLine()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayRemoveLine();
}

void CImageWindow::OverlayRemoveRectangle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayRemoveRectangle();
}

void CImageWindow::OverlayRemoveText()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayRemoveText();
}

void CImageWindow::OverlayRemoveAll()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayRemoveAll();
}

void CImageWindow::SetROIFill( BOOL bFill )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetUseFill( bFill );
}

void CImageWindow::ExtZoomIn()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtZoomIn();
}

void CImageWindow::ExtZoomOut()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtZoomOut();
}

void CImageWindow::ExtMove(long nImageCenterX, long nImageCenterY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtMove(nImageCenterX, nImageCenterY);
	m_pImpl->Invalidate(FALSE);
}

void CImageWindow::ExtPosSet(long nScreenOffsetX, long nScreenOffsetY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->__Scroll(-nScreenOffsetX, -nScreenOffsetY);
	m_pImpl->Invalidate(FALSE);
}

void CImageWindow::ExtPosGet(long& nScreenOffsetX, long& nScreenOffsetY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtPosGet(nScreenOffsetX, nScreenOffsetY);
}

void CImageWindow::SetDisplayImage(const BYTE* buf, long nWidth, long nHeight, long nWidthBytes, long nDepth)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetDisplayImage(buf, nWidth, nHeight, nWidthBytes, nDepth);
}

long CImageWindow::ROIAdd(const CRect& rtROI, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROIAdd(rtROI, crColor, pstrName, nLineWidth, isMovalble, isResizable);
}

long CImageWindow::ROIAdd(const ROIData& Data, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROIAdd(Data, crColor, pstrName, nLineWidth, isMovalble, isResizable);
}

void CImageWindow::ROISetMinSize(long nMinSizeX, long nMinSizeY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ROISetMinSize(nMinSizeX, nMinSizeY);
}

void CImageWindow::ROIRemoveAll()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ROIRemoveAll();
}

BOOL CImageWindow::ROIGet(long nID, CRect* prtROI)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROIGet(nID, prtROI);
}
BOOL CImageWindow::ROISet(long nID, const ROIData& Data, OLE_COLOR crColor, LPCTSTR pstrName, short nLineWidth, BOOL isMovalble, BOOL isResizable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROISet(nID, Data, crColor, pstrName, nLineWidth, isMovalble, isResizable);
}

BOOL CImageWindow::ROIDel(long nID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROIDel(nID);
}

ROIData CImageWindow::ROIGet(long nID) const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROIGet(nID);
}

BOOL CImageWindow::ROISel(long nID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROISel(nID);
}

long CImageWindow::ROICount() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROICount();
}

long CImageWindow::ROIGetCurSel() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->ROIGetCurSel();
}


void CImageWindow::ROIShow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ROIShow();
}

void CImageWindow::ROIHide()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ROIHide();
}

BOOL CImageWindow::IsROIEditMode() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->IsROIEditMode();
}

void CImageWindow::OverlayAddLineEx(const CPoint *pPt1, const CPoint *pPt2, OLE_COLOR crColor, long nPenStyle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddLineEx(pPt1, pPt2, crColor, nPenStyle);
}

void CImageWindow::OverlayAddRectangleEx(const CRect *pRect, OLE_COLOR crColor, long nPenStyle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddRectangleEx(pRect, crColor, nPenStyle);
}

void CImageWindow::ExtZoomScaleGet(double* pScaleH, double* pScaleV)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtZoomScaleGet(pScaleH, pScaleV);
}

void CImageWindow::ExtZoomScale(double fScaleH, double fScaleV)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtZoomScale(fScaleH, fScaleV);
}

void CImageWindow::ExtZoomROIGet(float *pLeft, float *pTop, float *pRight, float *pBottom)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtZoomROIGet(pLeft, pTop, pRight, pBottom);
}

void CImageWindow::ExtZoomROISet(float fLeft, float fTop, float fRight, float fBottom)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtZoomROISet(fLeft, fTop, fRight, fBottom);
}

void CImageWindow::OverlayAddCircle(float fCenX, float fCenY, float fRadiusX, float fRadiusY, OLE_COLOR crColor, BOOL bScaleDepend)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddCircle(fCenX, fCenY, fRadiusX, fRadiusY, crColor, bScaleDepend);
}

void CImageWindow::OverlayRemoveCircle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayRemoveCircle();
}

void CImageWindow::ExtZoomFit(short nZoomFitMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ExtZoomFit(nZoomFitMode);
}

void CImageWindow::OverlayAddFPoint(float fX, float fY, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddFPoint(fX, fY, crColor);
}

void CImageWindow::OverlayAddFLine(float fX1, float fY1, float fX2, float fY2, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddFLine(fX1, fY1, fX2, fY2, crColor);
}

void CImageWindow::OverlayAddFRectangle(float fLeft, float fTop, float fRight, float fBottom, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddFRectangle(fLeft, fTop, fRight, fBottom, crColor);
}

void CImageWindow::OverlayAddArcDoughnut(float fCenX, float fCenY, float fStartAngle, float fEndAngle, float fRadiusX, float fRadiusY, float fWidth, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddArcDoughnut(fCenX, fCenY, fStartAngle, fEndAngle, fRadiusX, fRadiusY, fWidth, crColor);
}

void CImageWindow::OverlayAddArcRing(float fCenX, float fCenY, float fStartAngle, float fEndAngle, float fInnerLength, float fOuterLength, OLE_COLOR crColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->OverlayAddArcRing(fCenX, fCenY, fStartAngle, fEndAngle, fInnerLength, fOuterLength, crColor);
}

void CImageWindow::SetDisplayWindow(CWnd* pDialog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetDisplayWindow(pDialog);
}

void CImageWindow::SetDisplayImage_Palette(const BYTE* buf, long nWidth, long nHeight, long nWidthBytes, long nDepth, OLE_COLOR* pPaletteColor )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetDisplayImage_Palette(buf, nWidth, nHeight, nWidthBytes, nDepth, pPaletteColor);
}
void CImageWindow::EnableDisplayHighQuality(BOOL bEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->EnableDisplayHighQuality(bEnable);
}

void CImageWindow::EnableMinimap(BOOL bEnable, CSize size)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->EnableMinimap(bEnable, size);
}

void CImageWindow::AddPopupMenuExternal(long nID, LPCTSTR pstrItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->AddPopupMenuExternal(nID, pstrItem);
}

void CImageWindow::ModifyPopupMenuExternal(long nID, short bEnable, short bChecked)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ModifyPopupMenuExternal(nID, bEnable, bChecked);
}

void CImageWindow::RemovePopupMenuExternal(long nID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->RemovePopupMenuExternal(nID);
}

void CImageWindow::EnableMouseTrace(BOOL bEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->EnableMouseTrace(bEnable);
}

void CImageWindow::SetTraceAttribute(long nTraceWidth, BYTE r, BYTE g, BYTE b, BYTE a)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetTraceWidth(nTraceWidth);
	m_pImpl->SetTraceColor(a, r, g, b);
}

void CImageWindow::GetMouseTrace(CPoint** ppVertice, long* pnVertice)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->GetMouseTrace(ppVertice, pnVertice);
}

void CImageWindow::SetDefaultZoomFitMode(long nZoomFitMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetDefaultZoomFitMode(nZoomFitMode);
}

BOOL CImageWindow::SetDisplayRange(BOOL bUse, long nLeft, long nTop, long nRight, long nBottom)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pImpl->SetDisplayRange(bUse, nLeft, nTop, nRight, nBottom);
}

void CImageWindow::EnablePixelMeter(BOOL bEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->EnablePixelMeter(bEnable);
}

void CImageWindow::SetPixelMeterUnit(float fOnePixelUnit, LPCTSTR lpszUnitName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetPixelMeterUnit(fOnePixelUnit, lpszUnitName);
}

void CImageWindow::SetPixelMeterBegin(const CPoint& ptBegin)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetPixelMeterBegin(ptBegin);
}

void CImageWindow::SetPixelMeterEnd(const CPoint& ptEnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->SetPixelMeterEnd(ptEnd);
}

void CImageWindow::ShowToolBar(BOOL bShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->ShowToolBar(bShow);
}

void CImageWindow::Screenshot(BOOL bShotImage, BOOL bShotROI, CBitmap& o_ddb)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pImpl->Screenshot(bShotImage, bShotROI, o_ddb);
}

HANDLE CImageWindow::DDBToDIB(CDC* pDC, CBitmap &Bitmap, CPalette *pPal)
{
	if( !pDC ) return NULL;

	BITMAP   Bmp;
	BITMAPINFO  BmpInfo;
	CPalette  Palette;
	CPalette*  pOldPalette = NULL;
	//기본 팔레트 생성
	Palette.CreateStockObject(DEFAULT_PALETTE);
	//비트맵 정보를 얻는다.
	Bitmap.GetBitmap(&Bmp);
	//////////////////////////////////////////////////////////////////////////
	// 1. 비트맵 해더를 초기화 한다.
	BmpInfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
	BmpInfo.bmiHeader.biWidth   = Bmp.bmWidth;
	BmpInfo.bmiHeader.biHeight   = Bmp.bmHeight;
	BmpInfo.bmiHeader.biPlanes   = 1;
	BmpInfo.bmiHeader.biBitCount  = Bmp.bmPlanes * Bmp.bmBitsPixel;
	BmpInfo.bmiHeader.biCompression  = BI_RGB;
	BmpInfo.bmiHeader.biSizeImage  = 0;
	BmpInfo.bmiHeader.biXPelsPerMeter = 0;
	BmpInfo.bmiHeader.biYPelsPerMeter = 0;
	BmpInfo.bmiHeader.biClrUsed   = 0;
	BmpInfo.bmiHeader.biClrImportant = 0;
	//비트맵 헤더와 컬러 테이블의 크기를 계산한다.
	int nColors = (1 << BmpInfo.bmiHeader.biBitCount);
	if (nColors > 256){
		nColors = 0;
	}
	DWORD dwImageLength = BmpInfo.bmiHeader.biSize + nColors * sizeof(RGBQUAD);
	//저장할 윈도우의 팔레트를 기본 팔레트로 설정한다.
	pOldPalette = pDC->SelectPalette(&Palette, FALSE);
	pDC->RealizePalette();
	//DIB가 저장될 메모리(비트맵 정보 헤더와 컬러 테이블)을 할당한다.
	HANDLE hDIB = ::GlobalAlloc(GPTR, dwImageLength);
	if (hDIB == NULL)
	{
		pDC->SelectPalette(pOldPalette, FALSE);
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//2. 비트맵 이미지의 크기를 계산한다.
	::GetDIBits(pDC->GetSafeHdc(),
		(HBITMAP)Bitmap.GetSafeHandle(),
		0L,
		BmpInfo.bmiHeader.biHeight,
		NULL,  //이 값이 NULL이면 이미지 크기를 계산.
		(LPBITMAPINFO)&BmpInfo.bmiHeader,
		DIB_RGB_COLORS);
	if (BmpInfo.bmiHeader.biSizeImage == 0)
	{
		pDC->SelectPalette(pOldPalette, FALSE);
		GlobalFree(hDIB);
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	//3. DIB 이미지를 구함.
	dwImageLength += BmpInfo.bmiHeader.biSizeImage;
	HANDLE hRealloc = ::GlobalReAlloc(hDIB, dwImageLength, GMEM_MOVEABLE);
	if (hRealloc != NULL)
	{
		hDIB = hRealloc;
	}
	else
	{
		pDC->SelectPalette(pOldPalette, FALSE);
		GlobalFree(hDIB);
		return NULL;
	}
	//DDB에서 DIB로 변환된 이미지를 구함.
	if (!::GetDIBits(pDC->GetSafeHdc(),
		(HBITMAP)Bitmap.GetSafeHandle(),
		0L,    // Start scan line
		(DWORD)BmpInfo.bmiHeader.biHeight,  //# of scan lines
		(LPBYTE)hDIB + (BmpInfo.bmiHeader.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)&BmpInfo.bmiHeader,  //address of bitmapinfo
		(DWORD)DIB_RGB_COLORS))
	{
		pDC->SelectPalette(pOldPalette, FALSE);
		GlobalFree(hDIB);
		return NULL;
	}
	//DIB 헤더 작성
	memcpy(hDIB, &BmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
	pDC->SelectPalette(pOldPalette, FALSE);
	Palette.DeleteObject();

	return hDIB;
}
BOOL CImageWindow::WriteDIB(LPCTSTR pszFileName, HANDLE hDIB)
{
	BITMAPFILEHEADER  BitmapHeader;
	LPBITMAPINFOHEADER  lpDIB = (LPBITMAPINFOHEADER)hDIB;
	if (!hDIB)
	{
		return FALSE;
	}
	CFile file;
	if (!file.Open(pszFileName, CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}
	int nColors = 1 << lpDIB->biBitCount;
	//////////////////////////////////////////////////////////////////////////
	//비트맵 파일 헤더 정보 설정
	//항상 "BM"이 됨.
	BitmapHeader.bfType   = 0x4d42; // 0x42 = "B" 0x4d = "M"
	BitmapHeader.bfSize   = (DWORD)::GlobalSize(hDIB) + sizeof(BITMAPFILEHEADER);
	BitmapHeader.bfReserved1 = 0;
	BitmapHeader.bfReserved2 = 0;
	BitmapHeader.bfOffBits  = (DWORD)(sizeof(BitmapHeader) +
		lpDIB->biSize +
		nColors * sizeof(RGBQUAD));
	//////////////////////////////////////////////////////////////////////////
	//비트맵 파일 헤더와 DIB 헤더 및 DIB 데이터를 파일로 저장
	//비트맵 파일 헤더
	file.Write((BYTE*)&BitmapHeader, sizeof(BITMAPFILEHEADER));
	//DIB 헤더 및 데이터
	file.Write((BYTE*)hDIB, (UINT)::GlobalSize(hDIB));

	return TRUE;
}

void CImageWindow::Screenshot(BOOL bShotImage, BOOL bShotROI, LPCTSTR szFilePath)
{
	m_pImpl->Screenshot(bShotImage, bShotROI, __ddbScreenshot);

	HANDLE hDIB = DDBToDIB(m_pImpl->GetDC(), __ddbScreenshot, NULL);

	if( !hDIB ) return;

	WriteDIB(szFilePath, hDIB);

	::GlobalFree(hDIB);
}

BOOL CImageWindow::__Create()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !m_pImpl->Create(CImageWindow_Impl::IDD, GetParent()) ) return FALSE;

	m_pImpl->ShowWindow(SW_SHOW);

	CRect rtClient(0, 0, 0, 0);
	GetClientRect(&rtClient);

	m_pImpl->MoveWindow(rtClient);

	return TRUE;
}
