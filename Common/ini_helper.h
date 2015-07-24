#pragma once

#include <vector>
#include <map>
using namespace std;

class CIniHelp
{
public:
	static CString	MakeKeyName(CString strNameHeader, CString strKeyname, int nKeyPostNum);

	static BYTE			LoadBYTE(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, BYTE byDefValue = 0);
	static void			SaveBYTE(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, BYTE byValue);
	static short		LoadSHORT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, short nDefValue = 0);
	static void			SaveSHORT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, short nValue);
	static int			LoadINT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, int nDefValue = 0);
	static void			SaveINT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, int nValue);
	static CString		LoadSTRING(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CString nDefValue = _T(""));
	static void			SaveSTRING(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CString strValue);
	static float		LoadFLOAT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, float dDefValue = 0.f);
	static void			SaveFLOAT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, float dValue);
	static double		LoadDOUBLE(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, double dDefValue = 0.f);
	static void			SaveDOUBLE(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, double dValue);
	static CPoint		LoadPOINT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CPoint nDefValue = CPoint(0, 0));
	static void			SavePOINT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CPoint ptValue);
	static CRect		LoadRECT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CRect nDefValue = CRect(0, 0, 0, 0));
	static void			SaveRECT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CRect rtValue);
	static ULONG		LoadULONG(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONG nDefValue = 0);
	static void			SaveULONG(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONG nValue);
	static ULONGLONG	LoadULONGLONG(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONGLONG nDefValue = 0);
	static void			SaveULONGLONG(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONGLONG nValue);
	
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, BYTE *byValue, BYTE byDefValue = 0);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, BYTE *byValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, short *nValue, short nDefValue = 0);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, short *nValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, int *nValue, int nDefValue = 0);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, int *nValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONG *nValue, ULONG nDefValue = 0);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONG *nValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONGLONG *nValue, ULONGLONG nDefValue = 0);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONGLONG *nValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, long *nValue, long nDefValue = 0);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, long *nValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, unsigned short *nValue, unsigned short nDefValue = 0);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, unsigned short *nValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CString *strValue, CString strDefValue = _T(""));
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CString *strValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, float *nValue, float nDefValue = 0);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, float *nValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, double *fValue, double fDefValue = 0.f);
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, double *fValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CPoint *ptValue, CPoint ptDefValue = CPoint(0, 0));
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CPoint *ptValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CRect *rtValue, CRect rtDefValue = CRect(0, 0, 0, 0));
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CRect *rtValue);
	static void			Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CTimeSpan *tmValue, CTimeSpan tmDefValue = CTimeSpan());
	static void			Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CTimeSpan *tmValue);
	static void			Delete(LPCTSTR szFilename, LPCTSTR szCategory);

	template <class T>
	static void Link(BOOL bSave, LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, T *value)
	{
		if(bSave)
			Write(szFilename, szCategory, szKey, value);
		else
			Read(szFilename, szCategory, szKey, value, *value);
	}
};
