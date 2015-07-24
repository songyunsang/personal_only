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

static CString g_strGlobalLogFileDir;
static void LOG_TEXT_SETDIR(CString strDir)
{
	g_strGlobalLogFileDir = strDir;
}

static void LOG_TEXT(LPCTSTR lpszFmt, ...)
{	
	CString str;
	CString strTime;

	SYSTEMTIME st;
	GetLocalTime(&st);
	
	// 파일의 이름은 log날짜.txt
	CString strLogFilename;
	if(g_strGlobalLogFileDir.GetLength() == 0)
	{
		TCHAR pszPathName[MAX_PATH];
		GetModuleFileName(NULL, pszPathName, MAX_PATH-1);
		CString strPath = pszPathName;
		strLogFilename = strPath.Left(strPath.ReverseFind(_T('\\'))) + _T("\\..\\log");
		g_strGlobalLogFileDir = strLogFilename;
		CreateDirectory(g_strGlobalLogFileDir, NULL);
	}

	strLogFilename.Format(_T("%s\\log%04d%02d%02d.txt"), (LPCTSTR)g_strGlobalLogFileDir, st.wYear, st.wMonth, st.wDay);
	
	strTime.Format(_T("[%02d:%02d:%02d] : "), st.wHour, st.wMinute, st.wSecond);
	va_list vargs;
	va_start(vargs,lpszFmt);
	str.FormatV(lpszFmt, vargs);
	va_end(vargs);

	strTime += str;
	
	LOG_FREETEXT(strLogFilename, strTime);
	return;
}
#endif	// LOG_TEXT_DEF
