#pragma once

//#ifdef DPI_CRASHDUMP_EXPORTS
//#define DPI_CRASHDUMP_API __declspec(dllexport)
//#else
//#define DPI_CRASHDUMP_API __declspec(dllimport)
//#endif

class /*DPI_CRASHDUMP_API*/ CCrashDump
{
public:
	typedef void (__cdecl *USERHANDLER)();

public:

	CCrashDump(LPCTSTR szFullPathName, USERHANDLER pUserExceptionHandlerPre, USERHANDLER pUserExceptionHandlerPost);
	~CCrashDump();

	void DumpProcessStatus(HANDLE hProcess, DWORD dwProcessID, LPCTSTR szFullPathName);

private:
	enum enumDumpThreadResult
	{
	    DUMP_OK = 0,
	    FAILED_TO_SAVE_DUMP_FILE,
	    FAILED_TO_CREATE_DUMP_FILE,
	    DBGHELP_DLL_TOO_OLD,
	    DBGHELP_DLL_NOT_FOUND,
	    UNKNOWN_FAILURE,
	};

	static LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExcInf);
	static LONG WINAPI UnhandledExceptionHandler(PEXCEPTION_POINTERS pExcInf);
	static void InvalidParameterHandler(const wchar_t *expression, const wchar_t *function, const wchar_t *file, unsigned int line, uintptr_t pReserved);
	static int CustomNewHandler(size_t size);

	CString m_strFullPathName;
	USERHANDLER	m_pUserExceptionHandlerPre;
	USERHANDLER	m_pUserExceptionHandlerPost;

	static unsigned int WINAPI DumpThread(void *pData);

	CCrashDump();
	CCrashDump(const CCrashDump &Src);

	friend struct SDumpThreadData;
};