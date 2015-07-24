#include "stdafx.h"
#include "FileUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace FileUtil
{
	CString GetFileDir(CString strFilePath)
	{
		if( strFilePath.IsEmpty() ) return _T("");

		strFilePath.Replace('/', '\\');

		return strFilePath.Left( strFilePath.ReverseFind('\\') + 1);
	}

	CString GetFileName(CString strFilePath)
	{
		if( strFilePath.IsEmpty() ) return _T("");

		strFilePath.Replace('/', '\\');

		int nPosBegin	= strFilePath.ReverseFind('\\') + 1;
		int nPosEnd		= strFilePath.ReverseFind('.');//.Find('.', nPosBegin);

		//nPosEnd = 
		nPosEnd = nPosEnd >= 0 ? nPosEnd : strFilePath.GetLength();

		return strFilePath.Mid(nPosBegin, nPosEnd - nPosBegin);
	}

	CString GetFileExt(CString strFilePath)
	{
		long nFind = strFilePath.ReverseFind(_T('.'));
		if(nFind < 0) return _T("");

		return strFilePath.Mid(nFind+1);
	}

	BOOL IsFileExist(LPCTSTR szFilePath)
	{	
		CFile file;

		if( !file.Open(szFilePath, CFile::modeRead) ) return FALSE;

		file.Close();

		return TRUE;
	}

	BOOL CreateDirectories(LPCTSTR szDirectory)
	{
		int nRet = ::SHCreateDirectoryEx(NULL, szDirectory, NULL);

		return nRet != ERROR_BAD_PATHNAME && nRet != ERROR_FILENAME_EXCED_RANGE;
	}

	BOOL BackupFile(CString strFilePath)
	{
		if( strFilePath.IsEmpty() ) return FALSE;

		CString strFileDir		= FileUtil::GetFileDir(strFilePath);
		CString strFileName		= FileUtil::GetFileName(strFilePath);

		CString strCurrentTime	= CTime::GetCurrentTime().Format(_T("[%Y%m%d%H%M%S]"));

		CString strBackupPath	= strFileDir + strFileName + strCurrentTime + _T(".bak");

		return CopyFile(strFilePath, strBackupPath, FALSE);
	}

	void OpenExplorer(LPCTSTR szFilePath)
	{
		CString strFile			= _T("explorer.exe");
		CString strParameter;	strParameter.Format(_T("/select,\"%s\""), szFilePath);

		SHELLEXECUTEINFO sei;
		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

		sei.cbSize			= sizeof(SHELLEXECUTEINFO);
		sei.hwnd			= NULL;
		sei.nShow			= SW_SHOW;
		sei.fMask			= SEE_MASK_NO_CONSOLE;
		sei.lpVerb			= __TEXT("open");
		sei.lpFile			= strFile;
		sei.lpParameters	= strParameter;

		ShellExecuteEx(&sei);
	}

	CString& PathNormalize(CString& strFilePath)
	{
		while( strFilePath.Replace(_T("\\\\"), _T("\\")) != 0 )
		{

		}

		return strFilePath;
	}
}
