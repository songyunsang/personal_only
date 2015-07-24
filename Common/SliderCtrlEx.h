#pragma once

#include <afxtempl.h>
#include "ColorNames.h"

typedef void(*ptr2Func)(void *p2Object, LPARAM data1, int sValue, BOOL IsDragging);

typedef UINT (CALLBACK* LPFNDLLFUNC1)(HDC,CONST PTRIVERTEX,DWORD,CONST PVOID,DWORD,DWORD);

class CSliderCtrlEx : public CSliderCtrl
{
public:
	CSliderCtrlEx();
	virtual ~CSliderCtrlEx();

public:
	BOOL m_extendColors;	// TRUE if background colors should be extended to edge of client rectangle
	BOOL m_dumpBitmaps;		// debugging variable to dump intermediate bitmaps to file
public:
	BOOL SaveBitmap(LPCSTR lpFileName, CBitmap &hBitmap, HPALETTE hPal = NULL);

public:
	void Refresh(void);		// force update of displayed slider -- user after all range and color changes

public:
	ptr2Func m_Callback;		// call this to report slider position
	void *m_p2Object;			// user-supplied value (typically pointer to class instance (this))
	LPARAM m_data1;				// user-supplied value (typically control ID or pointer)
public:
	void SetCallback(void *funcptr, void *p2Object, LPARAM data1);	// set callback function for reporting slider position
	void ClearCallback(void);	// clear out the callback function

public:
	void SetRangeMax( int nMax, BOOL bRedraw = FALSE);			// set max value for slider
	void SetRange( int nMin, int nMax, BOOL bRedraw = FALSE);	// set min and max value for slider
	void SetRangeMin( int nMin, BOOL bRedraw = FALSE);			// min min value for slider

public:
	BOOL AddColor(int nLow, int nHigh, COLORREF color);	// add a new range and color (TRUE if no errors)
	BOOL AddColor(int nLow, int nHigh, COLORREF strColor, COLORREF endColor); // add a new range and color gradient
	void ClearColors(void);			// clear out all color ranges

	int SetErrResponse(int resp);	// Control handling of AddColor() errors

	// Generated message map functions
protected:
	struct clrRange 
	{
		int lval;	// start new color at this slider value
		int hval;	// finish end color at this slider value
		COLORREF strColor;	// starting color (use named values or RGB() macro)
		COLORREF endColor;	// ending color (same as strColor if no gradient)
	};
	CArray<clrRange,clrRange> colorList;	// ordered list of ranges and associated colors

	int errResponse;	// how to handle errors in the AddColor() functions
	int m_oldPosition;	// used to detect if the slider has actually moved
	BOOL m_IsDragging;	// Set if the mouse button is DOWN

	// for using GradientFill from msimg32.dll
	HINSTANCE hinst_msimg32;	// DLL handle for msimg32.dll
	LPFNDLLFUNC1 GradientFill;	// function pointer for GradientFill from DLL

	//{{AFX_MSG(CSliderCtrlEx)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
