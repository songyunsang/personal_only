// CrashDump.cpp: implementation of the CCrashDump class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrashDump.h"

#include "process.h"
#include "dbghelp.h"
#include <afxmt.h>

#include "../iCubeTR/iCubeTR/MainFrm.h"

// From Platform SDK : "ntstatus.h"
#ifndef STATUS_INVALID_PARAMETER
#define STATUS_INVALID_PARAMETER         0xC000000DL	//((NTSTATUS)0xC000000DL)
#endif  /* STATUS_INVALID_PARAMETER */

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                         CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                         CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                         CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                        );

//unsigned int WINAPI DumpThread(void *pData);

struct SDumpThreadData
{
	HANDLE m_hProcess;
	DWORD m_dwProcessID;
	DWORD m_dwThreadID;
	LPCTSTR m_szFullPathName;
	PEXCEPTION_POINTERS m_pExcInf;
	CCrashDump::USERHANDLER	m_pUserHandlerPre;
	CCrashDump::USERHANDLER	m_pUserHandlerPost;
	CCrashDump::enumDumpThreadResult m_ResultCode;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CCrashDump *g_This = nullptr;

CCrashDump::CCrashDump(LPCTSTR szFullPathName, USERHANDLER pUserExceptionHandlerPre, USERHANDLER pUserExceptionHandlerPost)
	: m_strFullPathName(szFullPathName)
	, m_pUserExceptionHandlerPre(pUserExceptionHandlerPre)
	, m_pUserExceptionHandlerPost(pUserExceptionHandlerPost)
{
	::AddVectoredExceptionHandler(1, VectoredExceptionHandler);
	::SetUnhandledExceptionFilter(UnhandledExceptionHandler);		// For SEH
	::_set_invalid_parameter_handler(InvalidParameterHandler);		// For C-Runtime Library
	::_set_new_handler(CustomNewHandler);

	g_This = this;
}

CCrashDump::~CCrashDump()
{

}

void CCrashDump::InvalidParameterHandler(const wchar_t *expression, const wchar_t *function, const wchar_t *file, unsigned int line, uintptr_t pReserved)
{
	UnhandledExceptionHandler(nullptr);

	::TerminateProcess(::GetCurrentProcess(), STATUS_INVALID_PARAMETER);
}

int CCrashDump::CustomNewHandler(size_t size)
{
	UnhandledExceptionHandler(nullptr);

	::TerminateProcess(::GetCurrentProcess(), STATUS_INVALID_PARAMETER);

	return 0;
}

LONG WINAPI CCrashDump::VectoredExceptionHandler(PEXCEPTION_POINTERS pExcInf)
{
	switch(pExcInf->ExceptionRecord->ExceptionCode)
	{
		case EXCEPTION_ACCESS_VIOLATION:
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		case EXCEPTION_ILLEGAL_INSTRUCTION:
		case EXCEPTION_IN_PAGE_ERROR:
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
		case EXCEPTION_STACK_OVERFLOW:
			UnhandledExceptionHandler(nullptr);
			return EXCEPTION_CONTINUE_EXECUTION;
		default:
			return EXCEPTION_CONTINUE_SEARCH;
	}
}

LONG WINAPI CCrashDump::UnhandledExceptionHandler(PEXCEPTION_POINTERS pExcInf)
{
	SDumpThreadData DumpThreadData;
	DumpThreadData.m_hProcess			= ::GetCurrentProcess();
	DumpThreadData.m_dwProcessID		= ::GetCurrentProcessId();
	DumpThreadData.m_dwThreadID			= ::GetCurrentThreadId();
	DumpThreadData.m_szFullPathName		= LPCTSTR(g_This->m_strFullPathName);
	DumpThreadData.m_pExcInf			= pExcInf;
	DumpThreadData.m_pUserHandlerPre	= g_This->m_pUserExceptionHandlerPre;
	DumpThreadData.m_pUserHandlerPost	= g_This->m_pUserExceptionHandlerPost;
	DumpThreadData.m_ResultCode			= UNKNOWN_FAILURE;

	unsigned int dwThreadID = 0;

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, DumpThread, &DumpThreadData, 0, &dwThreadID);

	if(hThread != INVALID_HANDLE_VALUE)
	{
		::WaitForSingleObject(hThread, INFINITE);
		::CloseHandle(hThread);
	}

	switch(DumpThreadData.m_ResultCode)
	{
		case DUMP_OK:						::AfxMessageBox(_T("Internal Crash!\r\nCrash Dump Saved."));
			break;
		case FAILED_TO_SAVE_DUMP_FILE:		::AfxMessageBox(_T("Internal Crash!\r\nFailed To Save Dump File."));
			break;
		case FAILED_TO_CREATE_DUMP_FILE:	::AfxMessageBox(_T("Internal Crash!\r\nFailed To Create Dump File."));
			break;
		case DBGHELP_DLL_TOO_OLD:			::AfxMessageBox(_T("Internal Crash!\r\nDBGHELP.DLL Too Old."));
			break;
		case DBGHELP_DLL_NOT_FOUND:			::AfxMessageBox(_T("Internal Crash!\r\nDBGHELP.DLL Not Found."));
			break;
		default:
			::AfxMessageBox(_T("Internal Crash!\r\nUnknown Failure."));
			break;
	}

	// 열어둔 서버 소켓들은 닫아두자 (다음 실행시 포트를 못여는 문제가 발생함)
	//g_pMainFrame->__CloseServerSocket();

	_exit(0);		// 강제 종료를 수행한다.

	return EXCEPTION_EXECUTE_HANDLER;
}

void CCrashDump::DumpProcessStatus(HANDLE hProcess, DWORD dwProcessID, LPCTSTR szFullPathName)
{
	SDumpThreadData DumpThreadData;
	DumpThreadData.m_hProcess			= hProcess;
	DumpThreadData.m_dwProcessID		= dwProcessID;
	DumpThreadData.m_dwThreadID			= ::GetCurrentThreadId();
	DumpThreadData.m_szFullPathName		= szFullPathName;
	DumpThreadData.m_pExcInf			= nullptr;
	DumpThreadData.m_pUserHandlerPre	= nullptr;
	DumpThreadData.m_pUserHandlerPost	= nullptr;
	DumpThreadData.m_ResultCode			= UNKNOWN_FAILURE;

	unsigned int dwThreadID = 0;

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, DumpThread, &DumpThreadData, 0, &dwThreadID);

	if(hThread != INVALID_HANDLE_VALUE)
	{
		::WaitForSingleObject(hThread, INFINITE);
		::CloseHandle(hThread);
	}
}


unsigned int WINAPI CCrashDump::DumpThread(void *pData)
{
	SDumpThreadData &DumpThreadData = *(SDumpThreadData *)pData;

	if(DumpThreadData.m_pUserHandlerPre != nullptr)
		(DumpThreadData.m_pUserHandlerPre)();

	CString strMiniDumpResult;

	if (HINSTANCE hDll = ::LoadLibrary(_T("DBGHELP.DLL")))
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );

		if (pDump)
		{
			HANDLE hFile = ::CreateFile( DumpThreadData.m_szFullPathName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

			if (hFile != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId				= DumpThreadData.m_dwThreadID;
				ExInfo.ExceptionPointers	= DumpThreadData.m_pExcInf;
				ExInfo.ClientPointers		= TRUE;

				// write the dump
				if (pDump(DumpThreadData.m_hProcess, DumpThreadData.m_dwProcessID, hFile,  MiniDumpScanMemory, DumpThreadData.m_pExcInf ? (&ExInfo) : (nullptr), NULL, NULL ))
				{
					DumpThreadData.m_ResultCode = CCrashDump::DUMP_OK;
				}
				else
				{
					DumpThreadData.m_ResultCode = CCrashDump::FAILED_TO_SAVE_DUMP_FILE;
				}

				::CloseHandle(hFile);
			}
			else
			{
				DumpThreadData.m_ResultCode = CCrashDump::FAILED_TO_CREATE_DUMP_FILE;
			}
		}
		else
		{
			DumpThreadData.m_ResultCode = CCrashDump::DBGHELP_DLL_TOO_OLD;
		}
	}
	else
	{
		DumpThreadData.m_ResultCode = CCrashDump::DBGHELP_DLL_NOT_FOUND;
	}

	if(DumpThreadData.m_pUserHandlerPost != nullptr)
		(DumpThreadData.m_pUserHandlerPost)();

	return 0;
}
