#pragma once

#define	NM_LINKCLICK	WM_APP + 0x200

/////////////////////////////////////////////////////////////////////////////
// CLabel window

enum
{
    Normal = 0,
    Gradient,
    ByDegrees
};
enum
{
    Horizontal = 0,
    Vertical
};

class CLabel : public CStatic
{
	// Construction
public:
	static enum LinkStyle
	{
	    LinkNone = 0,
	    HyperLink,
	    MailLink
	};
	static enum FlashType
	{
	    None = 0,
	    Text,
	    Background
	};
	static enum EyeCatchType
	{
	    Disable = 0,
	    Enable
	};
	static enum Type3D
	{
	    Raised = 0,
	    Sunken
	};

	CLabel();
	virtual CLabel &SetBkColor(COLORREF crBkgnd, COLORREF crBkgndHigh = 0, int mode = Normal);
	virtual CLabel &SetTextColor(COLORREF crText);
	virtual CLabel &SetText(const CString &strText, BOOL bValidate = TRUE);
	virtual CLabel &SetFontBold(BOOL bBold);
	virtual CLabel &SetFontName(const CString &strFont, BYTE byCharSet = ANSI_CHARSET);
	virtual CLabel &SetFontUnderline(BOOL bSet);
	virtual CLabel &SetFontItalic(BOOL bSet);
	virtual CLabel &SetFontSize(int nSize);
	virtual CLabel &SetSunken(BOOL bSet);
	virtual CLabel &SetBorder(BOOL bSet);
	virtual CLabel &SetTransparent(BOOL bSet);
	virtual CLabel &FlashText(BOOL bActivate);
	virtual CLabel &EyeCatch(BOOL bActivate);
	virtual CLabel &FlashBackground(BOOL bActivate);
	virtual CLabel &SetLink(BOOL bLink, BOOL bNotifyParent);
	virtual CLabel &SetLinkCursor(HCURSOR hCursor);
	virtual CLabel &SetFont3D(BOOL bSet, Type3D type = Raised);
	virtual CLabel &SetRotationAngle(UINT nAngle, BOOL bRotation);
	virtual CLabel &SetText3DHiliteColor(COLORREF cr3DHiliteColor);
	virtual CLabel &SetFont(LOGFONT lf);
	virtual CLabel &SetMailLink(BOOL bEnable, BOOL bNotifyParent);
	virtual CLabel &SetHyperLink(const CString &sLink);
	virtual COLORREF GetBkColor();
	virtual void	SetFillFullStep(int nStep);
	virtual void	SetFillCurrentStep(int nStep);
	virtual void	SetFillByDegree(COLORREF crBkgnd, COLORREF crBkgndHigh, int nDirection);

	COLORREF GetTextColor()
	{
		return m_crText;
	}

	// Attributes
public:
	UINT_PTR	m_nTimerID1;
	UINT_PTR	m_nTimerID2;
protected:
	void UpdateSurface();
	void ReconstructFont();
	void DrawGradientFill(CDC *pDC, CRect *pRect, COLORREF crStart, COLORREF crEnd, int nSegments);
	void FillByDegrees(CDC *pDC, CRect *pRect, COLORREF crStart, COLORREF crEnd);
	COLORREF		m_crText;
	COLORREF		m_cr3DHiliteColor;
	HBRUSH			m_hwndBrush;
	HBRUSH			m_hBackBrush;
	LOGFONT			m_lf;
	CFont			m_font;
	BOOL			m_bState;
	BOOL			m_bECState;
	BOOL			m_bTimer;
	LinkStyle		m_Link;
	BOOL			m_bTransparent;
	BOOL			m_bFont3d;
	BOOL			m_bToolTips;
	BOOL			m_bNotifyParent;
	BOOL			m_bRotation;
	FlashType		m_FlashType;
	EyeCatchType	m_ECType;

	HCURSOR			m_hCursor;
	Type3D			m_3dType;
	int				m_fillmode;
	COLORREF		m_crHiColor;
	COLORREF		m_crLoColor;
	CString			m_sLink;
	int				m_nFontSize;

	int				m_nFillFullStep;
	int				m_nFillCurrentStep;
	int				m_nFillDirection;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabel)
protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLabel)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg	void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysColorChange();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
