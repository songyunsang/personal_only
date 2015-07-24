#include "stdafx.h"

#include "MFCTool.h"

namespace MFCTool
{
	std::vector<long> GetCurSels(const CListCtrl& ListCtrl)
	{
		std::vector<long> vecIndex;

		POSITION pos = ListCtrl.GetFirstSelectedItemPosition();

		while( pos != NULL ) vecIndex.push_back(ListCtrl.GetNextSelectedItem(pos));

		return vecIndex;
	}

	CString GetWindowString(const CWnd* pWnd)
	{
		CString str;

		if( pWnd && pWnd->GetSafeHwnd() ) pWnd->GetWindowText(str);

		return str;
	}

	CString Flt2Str(double num)
	{
		CString str;
		str.Format(_T("%g"), num);		// 소수점 이하 마지막 0 제거

		return str;
	}

	CString Int2Str(int num)
	{
		CString str;
		str.Format(_T("%d"), num);

		return str;
	}

	int Str2Int(LPCTSTR str)
	{
		return _ttoi(str);
	}

	double Str2Flt(LPCTSTR str)
	{
		return _ttof(str);
	}	
}