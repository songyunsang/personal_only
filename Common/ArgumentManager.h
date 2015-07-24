#pragma once

#include <map>

using namespace std;

class CArgumentManager
{
public:
	long	GetCount();
	LPCTSTR Get(CString strKey);
	BOOL	In(CString strKey);

public:
	BOOL Parsing(TCHAR** argv, int argc);
	long GetValidCommandCount(const vector<CString>& vecValidCommandList);

private:
	void __ParsingArgument(CString arg);

private:
	map<CString, CString> m_mapArgument;

	CString m_strTokenKey;
	CString m_strTokenValue;

public:
	CArgumentManager(LPCTSTR strTokenKey, LPCTSTR strTokenValue);
	~CArgumentManager(void);
};
