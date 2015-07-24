#pragma once

#ifndef LOG_TEXT_DEF
#define LOG_TEXT_DEF
static void LOG_FREETEXTW(LPCWSTR szfilename, LPCWSTR szText)
{
	HANDLE fd;
	DWORD len;
	
	fd = CreateFileW(szfilename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(fd == (HANDLE)-1)
	{
		fd = CreateFileW(szfilename, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(fd == (HANDLE)-1) return ;

		// 유니코드일 경우 유니코드 표시
		WORD nCode = 0xFEFF;
		DWORD len; 
		WriteFile(fd, &nCode, sizeof(WORD), &len, NULL);
	}
	
	//  2. 파일의 맨 끝으로 이동
	//---------------------
	SetFilePointer(fd, 0, NULL, FILE_END);
	WriteFile(fd, szText, (DWORD)(wcslen(szText))*sizeof(WCHAR), &len, NULL);
	CloseHandle(fd);
	return ;
}

static void LOG_FREETEXTA(LPCSTR szfilename, LPCSTR szText)
{
	HANDLE fd;
	DWORD len;
	
	fd = CreateFileA(szfilename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(fd == (HANDLE)-1)
	{
		fd = CreateFileA(szfilename, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(fd == (HANDLE)-1) return ;
	}
	
	//  2. 파일의 맨 끝으로 이동
	//---------------------
	SetFilePointer(fd, 0, NULL, FILE_END);
	WriteFile(fd, szText, (DWORD)(strlen(szText))*sizeof(CHAR), &len, NULL);
	CloseHandle(fd);
	return ;
}

#ifdef _UNICODE
	#define LOG_FREETEXT(szfilename, szText)	LOG_FREETEXTW(szfilename, szText)
#else 
	#define LOG_FREETEXT(szfilename, szText)	LOG_FREETEXTA(szfilename, szText)
#endif

#define APPPATH_BASE	0
#define APPPATH_BIN		1
#define APPPATH_CONFIG	2

static CString GetCurrentAppDirectory(int nMode)
{
	static CString strModuleDir;
	static CString strConfigDir;
	static CString strAppDir;

	if(strModuleDir.GetLength() == 0)
	{
		TCHAR szTemp[MAX_PATH];
		GetModuleFileName(NULL, szTemp, MAX_PATH - 1);
		CString strFilePath(szTemp);

		strModuleDir	= strFilePath.Left(strFilePath.ReverseFind(_T('\\')));
		strAppDir		= strModuleDir.Left(strModuleDir.ReverseFind(_T('\\')));
		strConfigDir	= strAppDir + _T("\\Config");

		// 자체 Config 폴더가 있으면 개별 읽지 않는다.
		if(_taccess(strConfigDir, 0) != 0)
		{
			strAppDir		= _T("c:\\intekplus");
			strConfigDir	= _T("c:\\intekplus\\config");
		}
	}

	switch(nMode)
	{
	case APPPATH_BIN:		return strModuleDir;
	case APPPATH_CONFIG:	return strConfigDir;
	case APPPATH_BASE:
	default:				return strAppDir;
	}
}
static CString g_strGlobalLogFileDir;
static void LOG_TEXT_SETDIR(LPCTSTR szDir)
{
	g_strGlobalLogFileDir = szDir;
}

static void LOG_TEXT(LPCTSTR lpszFmt, ...)
{	
	CString str;
	CString strTime;

	SYSTEMTIME st;
	GetLocalTime(&st);
	
	if(g_strGlobalLogFileDir.GetLength() == 0)
	{
		LOG_TEXT_SETDIR(GetCurrentAppDirectory(APPPATH_BIN) + _T("\\..\\Log"));

		CreateDirectory(g_strGlobalLogFileDir, NULL);
	}

	// 파일의 이름은 log날짜.txt

	CString strLogFilename;
	strLogFilename.Format(_T("%s\\log%04d%02d%02d.txt"), (LPCTSTR)g_strGlobalLogFileDir, st.wYear, st.wMonth, st.wDay);
	
	strTime.Format(_T("[%02d:%02d:%02d.%03d] : "), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	va_list vargs;
	va_start(vargs,lpszFmt);
	str.FormatV(lpszFmt, vargs);
	va_end(vargs);

	strTime += str;
	
	LOG_FREETEXT(strLogFilename, strTime);
	return;
}
#endif	// LOG_TEXT_DEF