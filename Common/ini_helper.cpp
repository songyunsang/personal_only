#include "stdafx.h"
#include "ini_helper.h"

#ifndef _UNICODE
#include <conio.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4996)

CString TrimSymbol(LPCTSTR szName)
{
	// [] 은 Category 로 쓰이므로 분류명으로 사용되서는 안되는 내용이다.
	// <> 로 바꾸어주자
	CString strName(szName);

	strName.Replace(_T("["), _T("$#<"));
	strName.Replace(_T("]"), _T("$#>"));

	return strName;
}

CString	CIniHelp::MakeKeyName(CString strNameHeader, CString strKeyname, int nKeyPostNum)
{
	CString dest;
	CString dest2;

	if(strNameHeader == _T(""))
		dest = strKeyname;
	else
		dest.Format(_T("%s_%s"), strNameHeader.GetBuffer(0), strKeyname);


	if(nKeyPostNum >= 0)
	{
		dest2.Format(_T("_%d"), nKeyPostNum);
		dest += dest2;
	}

	return dest;
}

BYTE CIniHelp::LoadBYTE(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, BYTE byDefValue)
{
	return (BYTE)GetPrivateProfileInt(TrimSymbol(szCategory), TrimSymbol(szKey), byDefValue, szFilename);
}

void CIniHelp::SaveBYTE(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, BYTE byDefValue)
{
	CString strValue;
	strValue.Format(_T("%d"), byDefValue);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

short CIniHelp::LoadSHORT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, short nDefValue)
{
	return (short)GetPrivateProfileInt(TrimSymbol(szCategory), TrimSymbol(szKey), nDefValue, szFilename);
}

void CIniHelp::SaveSHORT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, short nValue)
{
	CString strValue;
	strValue.Format(_T("%d"), nValue);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

int CIniHelp::LoadINT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, int nDefValue)
{
	return GetPrivateProfileInt(TrimSymbol(szCategory), TrimSymbol(szKey), nDefValue, szFilename);
}

void CIniHelp::SaveINT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, int nValue)
{
	CString strValue;
	strValue.Format(_T("%d"), nValue);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

CString CIniHelp::LoadSTRING(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CString nDefValue)
{
	TCHAR strValue[MAX_PATH];
	GetPrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), nDefValue, strValue, MAX_PATH - 1, szFilename);
	return strValue;
}

void CIniHelp::SaveSTRING(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CString strValue)
{
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

float CIniHelp::LoadFLOAT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, float dDefValue)
{
	TCHAR szVal[MAX_PATH - 1];

	CString strTemp;
	float dResult;

	strTemp.Format(_T("%f"), dDefValue);
	GetPrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strTemp, szVal, MAX_PATH - 1, szFilename);

	_stscanf(szVal, _T("%f"), &dResult);

	return dResult;
}

void CIniHelp::SaveFLOAT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, float dValue)
{
	CString strValue;
	strValue.Format(_T("%f"), dValue);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

double CIniHelp::LoadDOUBLE(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, double dDefValue)
{
	TCHAR szVal[MAX_PATH - 1];

	CString strTemp;
	double dResult;

	strTemp.Format(_T("%lf"), dDefValue);
	GetPrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strTemp, szVal, MAX_PATH - 1, szFilename);

	_stscanf(szVal, _T("%lf"), &dResult);

	return dResult;
}

void CIniHelp::SaveDOUBLE(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, double dValue)
{
	CString strValue;
	strValue.Format(_T("%lf"), dValue);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

CPoint CIniHelp::LoadPOINT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CPoint nDefValue)
{
	TCHAR szVal[MAX_PATH - 1];

	CString strTemp;
	CPoint ptResult;

	strTemp.Format(_T("%d,%d"), nDefValue.x, nDefValue.y);
	GetPrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strTemp, szVal, MAX_PATH - 1, szFilename);

	_stscanf(szVal, _T("%d,%d"), &ptResult.x, &ptResult.y);

	return ptResult;
}

void CIniHelp::SavePOINT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CPoint ptValue)
{
	CString strValue;
	strValue.Format(_T("%d,%d"), ptValue.x, ptValue.y);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

CRect CIniHelp::LoadRECT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CRect nDefValue)
{
	TCHAR szVal[MAX_PATH - 1];
	CString strTemp;

	CRect rtResult(0, 0, 0, 0);
	strTemp.Format(_T("%d,%d,%d,%d"), nDefValue.left, nDefValue.top, nDefValue.right, nDefValue.bottom);
	GetPrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strTemp, szVal, MAX_PATH - 1, szFilename);

	_stscanf(szVal, _T("%d,%d,%d,%d"), &rtResult.left, &rtResult.top, &rtResult.right, &rtResult.bottom);

	return rtResult;
}

void CIniHelp::SaveRECT(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CRect rtValue)
{
	CString strValue;
	strValue.Format(_T("%d,%d,%d,%d"), rtValue.left, rtValue.top, rtValue.right, rtValue.bottom);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

ULONG CIniHelp::LoadULONG(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONG nDefValue)
{
	TCHAR szVal[MAX_PATH - 1];
	CString strDefault;
	//	ULONGLONG lResult = 0;

	strDefault.Format(_T("%u"), nDefValue);
	GetPrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strDefault, szVal, MAX_PATH - 1, szFilename);

	return (ULONG)_ttoi64(szVal);
}

void CIniHelp::SaveULONG(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONG nValue)
{
	CString strValue;
	strValue.Format(_T("%u"), nValue);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

ULONGLONG CIniHelp::LoadULONGLONG(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONGLONG nDefValue)
{
	TCHAR szVal[MAX_PATH - 1];
	CString strDefault;
	//	ULONGLONG lResult = 0;

	strDefault.Format(_T("%d"), nDefValue);
	GetPrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strDefault, szVal, MAX_PATH - 1, szFilename);

	return _ttoi64(szVal);
}

void CIniHelp::SaveULONGLONG(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONGLONG nValue)
{
	CString strValue;
	strValue.Format(_T("%d"), nValue);
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, BYTE *byValue, BYTE byDefValue)
{
	*byValue = LoadBYTE(szFilename, szCategory, szKey, byDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, BYTE *byValue)
{
	SaveBYTE(szFilename, szCategory, szKey, *byValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, short *nValue, short nDefValue)
{
	*nValue = LoadSHORT(szFilename, szCategory, szKey, nDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, short *nValue)
{
	SaveSHORT(szFilename, szCategory, szKey, *nValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, int *nValue, int nDefValue)
{
	*nValue = LoadINT(szFilename, szCategory, szKey, nDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, int *nValue)
{
	SaveINT(szFilename, szCategory, szKey, *nValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONG *nValue, ULONG nDefValue)
{
	*nValue = LoadULONG(szFilename, szCategory, szKey, nDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONG *nValue)
{
	SaveULONG(szFilename, szCategory, szKey, *nValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONGLONG *nValue, ULONGLONG nDefValue)
{
	*nValue = LoadULONGLONG(szFilename, szCategory, szKey, nDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, ULONGLONG *nValue)
{
	SaveULONGLONG(szFilename, szCategory, szKey, *nValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, long *nValue, long nDefValue)
{
	*nValue = LoadINT(szFilename, szCategory, szKey, (int)nDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, long *nValue)
{
	SaveINT(szFilename, szCategory, szKey, (int)*nValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, unsigned short *nValue, unsigned short nDefValue)
{
	*nValue = (unsigned short)LoadINT(szFilename, szCategory, szKey, (int)nDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, unsigned short *nValue)
{
	SaveINT(szFilename, szCategory, szKey, (int)*nValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CString *strValue, CString strDefValue)
{
	*strValue = LoadSTRING(szFilename, szCategory, szKey, strDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CString *strValue)
{
	SaveSTRING(szFilename, szCategory, szKey, *strValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, float *fValue, float fDefValue)
{
	*fValue = LoadFLOAT(szFilename, szCategory, szKey, fDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, float *fValue)
{
	SaveFLOAT(szFilename, szCategory, szKey, *fValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, double *fValue, double fDefValue)
{
	*fValue = LoadDOUBLE(szFilename, szCategory, szKey, fDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, double *fValue)
{
	SaveDOUBLE(szFilename, szCategory, szKey, *fValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CPoint *ptValue, CPoint ptDefValue)
{
	*ptValue = LoadPOINT(szFilename, szCategory, szKey, ptDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CPoint *ptValue)
{
	SavePOINT(szFilename, szCategory, szKey, *ptValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CRect *rtValue, CRect rtDefValue)
{
	*rtValue = LoadRECT(szFilename, szCategory, szKey, rtDefValue);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CRect *rtValue)
{
	SaveRECT(szFilename, szCategory, szKey, *rtValue);
}

void CIniHelp::Read(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CTimeSpan *tmValue, CTimeSpan tmDefValue)
{
	TCHAR szVal[MAX_PATH - 1];
	CString strTemp;

	CRect rtResult(0, 0, 0, 0);
	strTemp = tmDefValue.Format(_T("%D, %H, %M, %S"));
	GetPrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strTemp, szVal, MAX_PATH - 1, szFilename);

	long nD(0), nH(0), nM(0), nS(0);
	_stscanf(szVal, _T("%d, %d, %d, %d"), &nD, &nH, &nM, &nS);
	*tmValue = CTimeSpan(nD, nH, nM, nS);
}

void CIniHelp::Write(LPCTSTR szFilename, LPCTSTR szCategory, LPCTSTR szKey, CTimeSpan *tmValue)
{
	CString strValue = tmValue->Format(_T("%D, %H, %M, %S"));
	WritePrivateProfileString(TrimSymbol(szCategory), TrimSymbol(szKey), strValue, szFilename);
}

void CIniHelp::Delete(LPCTSTR szFilename, LPCTSTR szCategory)
{
	CString strCategory(TrimSymbol(szCategory));

	long	nFileLength = 0;
	BYTE	*pBuffer	= nullptr;

	//------------------------------------------------------------------------------------
	// 파일내용읽기
	//------------------------------------------------------------------------------------

	{
		CFile file(szFilename, CFile::modeRead);

		nFileLength =  (long)file.GetLength();
		pBuffer		= new BYTE[nFileLength + 1];
		pBuffer[nFileLength] = 0;

		file.Read(pBuffer, nFileLength);
		file.Close();
	}

	//------------------------------------------------------------------------------------
	// 줄별로 STRING 으로 만들기
	//------------------------------------------------------------------------------------

	CString strTotal;
	strTotal = pBuffer;

	std::vector<CString> vecLine;
	long nStartLine = -1;

	for(long n = 0; n <= strTotal.GetLength(); n++)
	{
		if((strTotal.GetAt(n) == 13 || n == strTotal.GetLength()) && nStartLine >= 0)
		{
			CString str = strTotal.Mid(nStartLine, n - nStartLine);
			str = str.TrimLeft();
			str = str.TrimRight();

			vecLine.push_back(str);
			nStartLine = -1;
		}
		else if(n != strTotal.GetLength() && nStartLine == -1 && strTotal.GetAt(n) != 13 && strTotal.GetAt(n) != 10)
		{
			nStartLine = n;
		}
	}

	CString strNCategory = _T("[") + strCategory + _T("]");
	long nDelete1 = -1;
	long nDelete2 = -1;

	for(long n = 0; n < (long)vecLine.size(); n++)
	{
		if(nDelete1 == -1 && vecLine[n] == strNCategory)
		{
			nDelete1 = n;
			continue;
		}

		if(nDelete1 != -1 && vecLine[n].Left(1) == _T("["))
		{
			nDelete2 = n;
			break;
		}
	}

	if(nDelete1 != -1 && nDelete2 == -1)
	{
		vecLine.erase(vecLine.begin() + nDelete1, vecLine.end());
	}

	if(nDelete1 != -1 && nDelete2 != -1)
	{
		vecLine.erase(vecLine.begin() + nDelete1, vecLine.begin() + nDelete2);
	}

	CString strNewLine;
	strNewLine.Format(_T("%c%c"), 13, 10);

	for(long n = 0; n < (long)vecLine.size(); n++)
	{
		vecLine[n] += strNewLine;
	}

	//------------------------------------------------------------------------------------
	// 파일내용쓰기
	//------------------------------------------------------------------------------------

	{
		CFile file(szFilename, CFile::modeCreate | CFile::modeWrite);

		for(long n = 0; n < (long)vecLine.size(); n++)
		{
			BYTE *pBuffer	= nullptr;
			long nStrLength	= vecLine[n].GetLength();

#ifdef _UNICODE
			pBuffer = new BYTE[(nStrLength + 1) * 2];
			memset(pBuffer, 0, (nStrLength + 1) * 2);
			WideCharToMultiByte(CP_ACP,
			                    0,
			                    vecLine[n].GetBuffer(),
			                    -1,
			                    (LPSTR)pBuffer,
			                    (nStrLength + 1) * 2,
			                    NULL,
			                    NULL);

			pBuffer[nStrLength] = 0;
			file.Write(pBuffer, (UINT)strlen((char *)pBuffer));
			delete []pBuffer;

#else
			pBuffer = (BYTE *)vecLine[n].GetBuffer();
			file.Write(pBuffer, nStrLength);
#endif
		}
		file.Close();
	}

	delete []pBuffer;
}
