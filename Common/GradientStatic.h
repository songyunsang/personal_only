#pragma once

typedef UINT (CALLBACK *LPFNDLLFUNC1)(HDC, CONST PTRIVERTEX, DWORD, CONST PVOID, DWORD, DWORD);

class CGradientStatic : public CStatic
{
	// Construction
public:
	CGradientStatic();
	virtual ~CGradientStatic();

	void SetWindowText(LPCTSTR a_lpstr);

	void SetColor(long cl)
	{
		clLeft = cl;
	};
	void SetGradientColor(long cl)
	{
		clRight = cl;
	};
	void SetTextColor(long cl)
	{
		clText = cl;
	};
	void SetReverseGradient();
	void SetLeftSpacing(int iNoOfPixels)
	{
		m_iLeftSpacing = iNoOfPixels;
	};
	void SetTextAlign(int iAlign )
	{
		m_iAlign = iAlign;
	}; //0 - left, 1 - center, 2 -right
	void SetTextMultiLine( BOOL iMulti )
	{
		m_iMultiLine = iMulti;
	}; //FALSE : single line, TRUE : multi line //by kms
	void SetVerticalGradient(BOOL a_bVertical = TRUE)
	{
		m_bVertical = a_bVertical;
	};

	static void DrawGradRect(CDC *pDC, CRect r, COLORREF cLeft, COLORREF cRight, BOOL a_bVertical);

	void SetDrawGradient(BOOL b_DrawGradient = TRUE)
	{
		m_bDrawGradient = b_DrawGradient;
	}; //by kms
	void UpDateDraw(BOOL b_UpdateDraw = TRUE)
	{
		m_bUpdateDraw = b_UpdateDraw;
	}; //by kms

	// Generated message map functions
protected:
	void __Draw(CDC &dc);

	CString m_sTEXT;
	int m_iLeftSpacing;
	long clLeft;
	long clRight;
	long clText;
	int m_iAlign;
	BOOL m_iMultiLine;
	HINSTANCE hinst_msimg32;
	BOOL m_bCanDoGradientFill;
	BOOL m_bVertical;
	BOOL m_bDrawGradient;	//by kms
	BOOL m_bUpdateDraw;  //by kms
	LPFNDLLFUNC1 dllfunc_GradientFill;
	HFONT hfontOld;

	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnPaint();
};
