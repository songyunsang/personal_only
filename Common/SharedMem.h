﻿#pragma once
//----------------------------------------------------------------------------------------------
// Shared Memory 를 쓰기 편하게 만든 클래스 (송준호)
//----------------------------------------------------------------------------------------------

template<class _Type, class _ReturnType = _Type> class SharedMem
{
public:
	SharedMem(LPCTSTR lpKeyName)
	{
		__hMemMap = CreateFileMapping(	INVALID_HANDLE_VALUE,
										NULL,
										PAGE_READWRITE,
										0,
										sizeof(_Type),
										lpKeyName);

		if(__hMemMap == NULL)
		{
			AfxMessageBox(_T("Shared Memory 이상 오류!!"));
			return;
		}

		BOOL bCreateObject = TRUE;

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// 이미 생성되어 있을 시에는 오픈
			__hMemMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, lpKeyName);
			bCreateObject = FALSE;
		}

		__pData = (_Type *)MapViewOfFile(__hMemMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if(bCreateObject)
		{
			// 초기화
			memset(__pData, 0, sizeof(_Type));
		}
	}

	~SharedMem(void)
	{
		CloseHandle(__hMemMap);
	}

	operator _ReturnType()
	{
		return *__pData;
	}

	SharedMem& operator = (const _Type &Object)
	{
		if(__pData == &Object)
			return *this;

		*__pData = Object;
		return *this;
	}

private:
	_Type*	__pData;
	HANDLE	__hMemMap;
};
   