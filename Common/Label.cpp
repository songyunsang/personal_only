#include "stdafx.h"
#include "Label.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CLabel, CStatic)
	//{{AFX_MSG_MAP(CLabel)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CLabel::CLabel()
	: m_nTimerID1(0)
	, m_nTimerID2(0)
	, m_nFontSize(0)
	, m_nFillFullStep(0)
	, m_nFillCurrentStep(0)
	, m_crHiColor(0)
	, m_crLoColor(0)
	, m_bTimer(FALSE)
	, m_bState(FALSE)
	, m_bECState(FALSE)
	, m_bTransparent(FALSE)
	, m_Link(LinkNone)
	, m_FlashType(None)
	, m_ECType(Disable)
	, m_bFont3d(FALSE)
	, m_bNotifyParent(FALSE)
	, m_bToolTips(FALSE)
	, m_bRotation(FALSE)
	, m_fillmode(Normal)
	, m_nFillDirection(Horizontal)
	, m_3dType(Raised)
	, m_hCursor(NULL)
	, m_hBackBrush(NULL)
{
	m_crText		= GetSysColor(COLOR_WINDOWTEXT);

	m_cr3DHiliteColor	=	RGB(255, 255, 255);
	m_hwndBrush			= ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));
}

CLabel::~CLabel()
{
	// Clean up
	m_font.DeleteObject();
	::DeleteObject(m_hwndBrush);

	// Stop Checking complaining
	if (m_hBackBrush)
	{
		::DeleteObject(m_hBackBrush);
	}
}

void CLabel::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc, TRUE);
	GetParent()->UpdateWindow();
}

void CLabel::ReconstructFont()
{
	m_font.DeleteObject();
	BOOL bCreated = m_font.CreateFontIndirect(&m_lf);

	ASSERT(bCreated);
}

void CLabel::OnDestroy()
{
	if(NULL != m_nTimerID1)
	{
		KillTimer(m_nTimerID1);
		m_nTimerID1 = 0;
	}
	if(NULL != m_nTimerID2)
	{
		KillTimer(m_nTimerID2);
		m_nTimerID2 = 0;
	}

	CStatic::OnDestroy();
}
void CLabel::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DWORD dwFlags = 0;

	CRect rc(0, 0, 0, 0);
	GetClientRect(rc);
	CString strText;
	GetWindowText(strText);
	CBitmap bmp;


	///////////////////////////////////////////////////////
	//
	// Set up for double buffering...
	//
	CDC *pDCMem;
	CBitmap	*pOldBitmap = NULL;

	if (!m_bTransparent)
	{
		pDCMem = new CDC;
		pDCMem->CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
		pOldBitmap = pDCMem->SelectObject(&bmp);
	}
	else
	{
		pDCMem = &dc;
	}

	UINT nMode = pDCMem->SetBkMode(TRANSPARENT);


	COLORREF crText = pDCMem->SetTextColor(m_crText);
	CFont *pOldFont = pDCMem->SelectObject(&m_font);


	// Fill in backgound if not transparent
	if (!m_bTransparent)
	{
		if (m_fillmode == Normal)
		{
			CBrush br;

			if (m_hBackBrush != NULL)
				br.Attach(m_hBackBrush);
			else
				br.Attach(m_hwndBrush);

			pDCMem->FillRect(rc, &br);

			br.Detach();
		}
		else if ( m_fillmode == Gradient ) // Gradient Fill
			DrawGradientFill(pDCMem, &rc, m_crLoColor, m_crHiColor, 100);

		else if ( m_fillmode == ByDegrees )
			FillByDegrees(pDCMem, &rc, m_crLoColor, m_crHiColor);
	}


	// If the text is flashing turn the text color on
	// then to the color of the window background.

	LOGBRUSH lb;
	ZeroMemory(&lb, sizeof(lb));

	// Stop Checking complaining
	if (m_hBackBrush)
		::GetObject(m_hBackBrush, sizeof(lb), &lb);


	// Something to do with flashing
	if (!m_bState && m_FlashType == Text)
	{
		lb.lbColor = RGB(255, 255, 255) - m_crText;
		pDCMem->SetTextColor(lb.lbColor);
	}
	if (!m_bState && m_FlashType == Background)
	{
		lb.lbColor = RGB(255, 255, 255) - m_crLoColor;
		pDCMem->SetBkColor(lb.lbColor);
	}

	if(!m_bECState && m_ECType == Enable)
	{
		CFont cf;
		LOGFONT lf;

		cf.CreatePointFont(m_nFontSize * 10, m_lf.lfFaceName);
		cf.GetLogFont(&lf);

		m_lf.lfHeight = lf.lfHeight;
		m_lf.lfWidth  = lf.lfWidth;

		ReconstructFont();
	}
	else if(m_ECType == Enable)
	{
		CFont cf;
		LOGFONT lf;

		cf.CreatePointFont( (m_nFontSize + 1) * 10, m_lf.lfFaceName);
		cf.GetLogFont(&lf);

		m_lf.lfHeight = lf.lfHeight;
		m_lf.lfWidth  = lf.lfWidth;

		ReconstructFont();
	}

	DWORD style = GetStyle();

	switch (style & SS_TYPEMASK)
	{
	case SS_RIGHT:
		dwFlags = DT_RIGHT | DT_WORDBREAK;
		break;

	case SS_CENTER:
		dwFlags = SS_CENTER | DT_WORDBREAK;
		break;

	case SS_LEFTNOWORDWRAP:
		dwFlags = DT_LEFT;
		break;

	default: // treat other types as left
	case SS_LEFT:
		dwFlags = DT_LEFT | DT_WORDBREAK;
		break;
	}


	// Added to expand tabs...
	if(strText.Find(_T('\t')) != -1)
		dwFlags |= DT_EXPANDTABS;

	// If the text centered make an assumtion that
	// the will want to center verticly as well
	if (style & SS_CENTERIMAGE)
	{
		dwFlags = DT_CENTER;

		// Apply
		if (strText.Find(_T("\r\n")) == -1)
		{
			dwFlags |= DT_VCENTER;

			// And because DT_VCENTER only works with single lines
			dwFlags |= DT_SINGLELINE;
		}

	}

	//
	// 3333   DDDDD
	//     3  D    D
	//   33   D     D    E F X
	//     3  D    D
	// 3333   DDDDD
	//
	//
	if (m_bRotation)
	{
		int nAlign = pDCMem->SetTextAlign (TA_BASELINE);

		CPoint pt;
		GetViewportOrgEx (pDCMem->m_hDC, &pt) ;
		SetViewportOrgEx (pDCMem->m_hDC, rc.Width() / 2, rc.Height() / 2, NULL) ;
		pDCMem->TextOut (0, 0, strText) ;
		SetViewportOrgEx (pDCMem->m_hDC, pt.x / 2, pt.y / 2, NULL) ;
		pDCMem->SetTextAlign (nAlign);
	}
	else
	{
		pDCMem->DrawText(strText, rc, dwFlags);
		if (m_bFont3d)
		{
			pDCMem->SetTextColor(m_cr3DHiliteColor);

			if (m_3dType == Raised)
				rc.OffsetRect(-1, -1);
			else
				rc.OffsetRect(1, 1);

			pDCMem->DrawText(strText, rc, dwFlags);
		}
	}

	// Restore DC's State
	pDCMem->SetBkMode(nMode);
	pDCMem->SelectObject(pOldFont);
	pDCMem->SetTextColor(crText);

	if (!m_bTransparent)
	{
		dc.BitBlt(0, 0, rc.Width(), rc.Height(), pDCMem, 0, 0, SRCCOPY);
		// continue DC restore
		pDCMem->SelectObject ( pOldBitmap ) ;
		pDCMem->DeleteDC();
		bmp.DeleteObject();
		delete pDCMem;
	}
}

void CLabel::OnTimer(UINT_PTR nIDEvent)
{

	if(nIDEvent == 1)
		m_bState = !m_bState;

	if(nIDEvent == 2)
		m_bECState = !m_bECState;

	UpdateSurface();

	CStatic::OnTimer(nIDEvent);
}

BOOL CLabel::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{

	if (m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CLabel::OnLButtonDown(UINT nFlags, CPoint point)
{

	if (!m_bNotifyParent) // Fix
	{
		CString strLink;

		GetWindowText(strLink);
		if (m_Link == HyperLink)
		{
			ShellExecute(NULL, _T("open"), m_sLink.IsEmpty() ? strLink : m_sLink, NULL, NULL, SW_SHOWNORMAL);
		}
		if (m_Link == MailLink)
		{
			strLink = CString(_T("mailto:")) + strLink;
			ShellExecute( NULL, NULL,  strLink,  NULL, NULL, SW_SHOWNORMAL );
		}
	}
	else
	{
		// To use notification in parent window
		// Respond to a OnNotify in parent and disassemble the message
		//
		NMHDR nm;

		nm.hwndFrom = GetSafeHwnd();
		nm.idFrom  = GetDlgCtrlID();
		nm.code = NM_LINKCLICK;
		GetParent()->SendMessage(WM_NOTIFY, nm.idFrom, (LPARAM) &nm);
	}

	CStatic::OnLButtonDown(nFlags, point);
}

CLabel &CLabel::SetText(const CString &strText, BOOL bValidate)
{
	if(IsWindow(this->GetSafeHwnd()))
	{
		SetWindowText(strText);

		if(bValidate)
			UpdateSurface();
		else
			RedrawWindow();
	}

	return *this;
}

CLabel &CLabel::SetTextColor(COLORREF crText)
{
	m_crText = crText;

	UpdateSurface();

	return *this;
}

CLabel &CLabel::SetFontBold(BOOL bBold)
{
	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

CLabel &CLabel::SetFontUnderline(BOOL bSet)
{
	m_lf.lfUnderline = (unsigned char)bSet;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

CLabel &CLabel::SetFontItalic(BOOL bSet)
{
	m_lf.lfItalic = (unsigned char)bSet;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

CLabel &CLabel::SetSunken(BOOL bSet)
{
	if (!bSet)
		ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_DRAWFRAME);
	else
		ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_DRAWFRAME);

	return *this;
}

CLabel &CLabel::SetBorder(BOOL bSet)
{
	if (!bSet)
		ModifyStyle(WS_BORDER, 0, SWP_DRAWFRAME);
	else
		ModifyStyle(0, WS_BORDER, SWP_DRAWFRAME);

	return *this;
}

CLabel &CLabel::SetFontSize(int nSize)
{
	m_nFontSize = nSize;

	CFont cf;
	LOGFONT lf;

	cf.CreatePointFont(nSize * 10, m_lf.lfFaceName);
	cf.GetLogFont(&lf);


	m_lf.lfHeight = lf.lfHeight;
	m_lf.lfWidth  = lf.lfWidth;

	//	nSize*=-1;
	//	m_lf.lfHeight = nSize;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

CLabel &CLabel::SetBkColor(COLORREF crBkgnd, COLORREF crBkgndHigh, int mode)
{
	COLORREF crBkOld = GetBkColor();
	if (crBkgndHigh == 0 && mode == 0 && crBkOld == crBkgnd)
		return *this;

	m_crLoColor = crBkgnd;
	m_crHiColor = crBkgndHigh;

	m_fillmode = mode;

	if (m_hBackBrush)
		::DeleteObject(m_hBackBrush);


	if (m_fillmode == Normal)
		m_hBackBrush = ::CreateSolidBrush(crBkgnd);

	UpdateSurface();

	return *this;
}

COLORREF CLabel::GetBkColor()
{
	return m_crLoColor;
}

CLabel &CLabel::SetFontName(const CString &strFont, BYTE byCharSet /* Default = ANSI_CHARSET */)
{

	m_lf.lfCharSet = byCharSet;

	_tcscpy_s(m_lf.lfFaceName, strFont);
	ReconstructFont();
	UpdateSurface();

	return *this;
}

CLabel &CLabel::FlashText(BOOL bActivate)
{

	if (m_bTimer)
	{
		if(NULL != m_nTimerID1)
		{
			KillTimer(m_nTimerID1);
			m_nTimerID1 = 0;
		}
	}

	if (bActivate)
	{
		m_bState = FALSE;

		m_bTimer = TRUE;

		if(NULL == m_nTimerID1)
		{
			m_nTimerID1 = SetTimer(1, 500, NULL);
		}

		m_FlashType = Text;
	}
	else
		m_FlashType = None; // Fix

	return *this;
}

CLabel &CLabel::EyeCatch(BOOL bActivate)
{

	if (m_bTimer)
	{
		if(NULL != m_nTimerID2)
		{
			KillTimer(m_nTimerID2);
			m_nTimerID2 = 0;
		}
	}

	if (bActivate)
	{
		m_bECState = FALSE;

		m_bTimer = TRUE;

		if(NULL == m_nTimerID2)
		{
			m_nTimerID2 = SetTimer(2, 500, NULL);
		}

		m_ECType = Enable;
	}
	else
		m_ECType = Disable;

	return *this;
}

CLabel &CLabel::FlashBackground(BOOL bActivate)
{

	if (m_bTimer)
	{
		if(NULL != m_nTimerID1)
		{
			KillTimer(m_nTimerID1);
			m_nTimerID1 = 0;
		}
	}

	if (bActivate)
	{
		m_bState = FALSE;

		m_bTimer = TRUE;
		if(NULL == m_nTimerID1)
		{
			m_nTimerID1 = SetTimer(1, 500, NULL);
		}

		m_FlashType = Background;
	}

	return *this;
}


CLabel &CLabel::SetLink(BOOL bLink, BOOL bNotifyParent)
{

	if (bLink)
		m_Link = HyperLink;
	else
		m_Link = LinkNone;

	m_bNotifyParent = bNotifyParent;

	if (m_Link != LinkNone)
		ModifyStyle(0, SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY, 0);



	return *this;
}

CLabel &CLabel::SetLinkCursor(HCURSOR hCursor)
{

	m_hCursor = hCursor;
	return *this;
}

CLabel &CLabel::SetTransparent(BOOL bSet)
{

	m_bTransparent = bSet;
	ModifyStyleEx(0, WS_EX_TRANSPARENT); // Fix for transparency
	UpdateSurface();

	return *this;
}

void CLabel::SetFillFullStep(int nStep)
{
	m_nFillFullStep = nStep;
}

void CLabel::SetFillCurrentStep(int nStep)
{
	m_nFillCurrentStep = nStep;
	Invalidate(FALSE);
}

void CLabel::SetFillByDegree(COLORREF crBkgnd, COLORREF crBkgndHigh, int nDirection)
{
	m_crLoColor = crBkgnd;
	m_crHiColor = crBkgndHigh;
	m_nFillDirection = nDirection;
	m_fillmode = ByDegrees;
}

CLabel &CLabel::SetFont3D(BOOL bSet, Type3D type)
{

	m_bFont3d = bSet;
	m_3dType = type;
	UpdateSurface();


	return *this;
}

void CLabel::OnSysColorChange()
{

	if (m_hwndBrush)
		::DeleteObject(m_hwndBrush);

	m_hwndBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	UpdateSurface();


}

CLabel &CLabel::SetRotationAngle(UINT nAngle, BOOL bRotation)
{
	// Arrrrh...
	// Your looking in here why the font is rotating, aren't you?
	// Well try setting the font name to 'Arial' or 'Times New Roman'
	// Make the Angle 180 and set bRotation to true.
	//
	// Font rotation _ONLY_ works with TrueType fonts...
	//
	//
	m_lf.lfEscapement = m_lf.lfOrientation = (nAngle * 10);
	m_bRotation = bRotation;

	ReconstructFont();

	UpdateSurface();


	return *this;
}

CLabel &CLabel::SetText3DHiliteColor(COLORREF cr3DHiliteColor)
{
	m_cr3DHiliteColor = cr3DHiliteColor;
	UpdateSurface();


	return *this;
}

void CLabel::PreSubclassWindow()
{

	CStatic::PreSubclassWindow();

	CFont *cf = GetFont();
	if(cf != NULL)
	{
		cf->GetObject(sizeof(m_lf), &m_lf);
	}
	else
	{
		GetObject(GetStockObject(SYSTEM_FONT), sizeof(m_lf), &m_lf);
	}

	ReconstructFont();

}

BOOL CLabel::PreCreateWindow(CREATESTRUCT &cs)
{
	return CStatic::PreCreateWindow(cs);
}

void CLabel::DrawGradientFill(CDC *pDC, CRect *pRect, COLORREF crStart, COLORREF crEnd, int nSegments)
{
	// Get the starting RGB values and calculate the incremental
	// changes to be applied.

	COLORREF cr;
	int nR = GetRValue(crStart);
	int nG = GetGValue(crStart);
	int nB = GetBValue(crStart);

	int neB = GetBValue(crEnd);
	int neG = GetGValue(crEnd);
	int neR = GetRValue(crEnd);

	if(nSegments > pRect->Width())
		nSegments = pRect->Width();

	int nDiffR = (neR - nR);
	int nDiffG = (neG - nG);
	int nDiffB = (neB - nB);

	int ndR = 256 * (nDiffR) / (max(nSegments, 1));
	int ndG = 256 * (nDiffG) / (max(nSegments, 1));
	int ndB = 256 * (nDiffB) / (max(nSegments, 1));

	nR *= 256;
	nG *= 256;
	nB *= 256;

	neR *= 256;
	neG *= 256;
	neB *= 256;

	int nCX = pRect->Width() / max(nSegments, 1), nLeft = pRect->left, nRight;
	pDC->SelectStockObject(NULL_PEN);

	for (int i = 0; i < nSegments; i++, nR += ndR, nG += ndG, nB += ndB)
	{
		// Use special code for the last segment to avoid any problems
		// with integer division.

		if (i == (nSegments - 1))
			nRight = pRect->right;
		else
			nRight = nLeft + nCX;

		cr = RGB(nR / 256, nG / 256, nB / 256);

		{
			CBrush br(cr);
			CBrush *pbrOld = pDC->SelectObject(&br);
			pDC->Rectangle(nLeft, pRect->top, nRight + 1, pRect->bottom);
			pDC->SelectObject(pbrOld);
		}

		// Reset the left side of the drawing rectangle.

		nLeft = nRight;
	}
}

void CLabel::FillByDegrees(CDC *pDC, CRect *pRect, COLORREF crStart, COLORREF crEnd)
{
	pDC->SelectStockObject(BLACK_PEN);

	CBrush br1(crStart);
	CBrush br2(crEnd);
	CBrush *pbrOld = pDC->SelectObject(&br1);

	if(m_nFillDirection == Horizontal)
		pDC->Rectangle(pRect->left, pRect->top, pRect->right * m_nFillCurrentStep / m_nFillFullStep, pRect->bottom);
	else
		pDC->Rectangle(pRect->left, (int)(pRect->top * (1 - (double)m_nFillCurrentStep / m_nFillFullStep)), pRect->right, pRect->bottom);

	pDC->SelectObject(br2);

	if(m_nFillDirection == Horizontal)
		pDC->Rectangle(pRect->right * m_nFillCurrentStep / m_nFillFullStep, pRect->top, pRect->right, pRect->bottom);
	else
		pDC->Rectangle(pRect->left, pRect->top, pRect->right, (int)(pRect->bottom * (1 - (double)m_nFillCurrentStep / m_nFillFullStep)) );

	pDC->SelectObject(pbrOld);

}

CLabel &CLabel::SetFont(LOGFONT lf)
{
	CopyMemory(&m_lf, &lf, sizeof(m_lf));
	ReconstructFont();
	UpdateSurface();
	return *this;

}

BOOL CLabel::OnEraseBkgnd(CDC * /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default

	return FALSE;
}

CLabel &CLabel::SetMailLink(BOOL bEnable, BOOL bNotifyParent)
{
	if (bEnable)
		m_Link = MailLink;
	else
		m_Link = LinkNone;


	m_bNotifyParent = bNotifyParent;

	if (m_Link != LinkNone)
		ModifyStyle(0, SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY, 0);

	return *this;
}

CLabel &CLabel::SetHyperLink(const CString &sLink)
{
	m_sLink = sLink;

	return *this;
}
