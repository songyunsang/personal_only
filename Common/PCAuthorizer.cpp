#include "stdafx.h"
#include "PCAuthorizer.h"


CPCAuthorizer::CPCAuthorizer(void)
{
}


CPCAuthorizer::~CPCAuthorizer(void)
{
}

void CPCAuthorizer::Activate()
{
	HKEY hKey = NULL;

	TCHAR szSubKeyPath[1000] = _T("SOFTWARE\\Intekplus");

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, szSubKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS )
	{
		::RegSetValueEx(hKey, _T("Authorized"), 0, REG_SZ, (BYTE*)AUTHORIZESIGNATURE, (DWORD)(sizeof(TCHAR) * _tcslen(AUTHORIZESIGNATURE)));
		::RegCloseKey(hKey);
	}
}

void CPCAuthorizer::Deactivate()
{
	HKEY hKey = NULL;

	TCHAR szPath[MAX_PATH + 1];

	szPath[MAX_PATH] = 0;

	TCHAR szSubKeyPath[1000]	= _T("SOFTWARE");

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, szSubKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS )
	{
		::RegDeleteKey(hKey, _T("Intekplus"));
		::RegCloseKey(hKey);
	}
}

BOOL CPCAuthorizer::IsAuthorized()
{
	HKEY hKey = NULL;

	TCHAR szSubKeyPath[1000] = _T("SOFTWARE\\Intekplus");

	if (::RegOpenKeyEx  (HKEY_LOCAL_MACHINE, szSubKeyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dw = 1024;
		DWORD dwType;
		TCHAR szValue[1024];		
		
		memset(szValue, 0, sizeof(TCHAR) * 1024);

		BOOL bRet = ::RegQueryValueEx(hKey, _T("Authorized"), NULL, &dwType, (LPBYTE)szValue, &dw) == ERROR_SUCCESS;

		::RegCloseKey (hKey);

		if (bRet)
		{
			if (dwType != REG_SZ)
			{
				return FALSE;
			}
			else
			{
				CString strValue = szValue;

				if( strValue == AUTHORIZESIGNATURE ) return TRUE;

				return FALSE;
			}
		}

	}

	return FALSE;
}