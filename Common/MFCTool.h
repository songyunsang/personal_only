#pragma once

#include <vector>

namespace MFCTool
{
	std::vector<long> GetCurSels(const CListCtrl& ListCtrl);
	
	CString GetWindowString(const CWnd* pWnd);

	CString Flt2Str(double	num);
	CString Int2Str(int		num);

	int		Str2Int(LPCTSTR str);
	double	Str2Flt(LPCTSTR str);

}