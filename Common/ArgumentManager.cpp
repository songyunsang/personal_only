#include "StdAfx.h"
#include <vector>
#include "ArgumentManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CArgumentManager::CArgumentManager(LPCTSTR strTokenKey, LPCTSTR strTokenValue)
: m_strTokenKey(strTokenKey)
, m_strTokenValue(strTokenValue)
{
}

CArgumentManager::~CArgumentManager(void)
{
}

long	CArgumentManager::GetCount()
{
	return (long)m_mapArgument.size();
}

LPCTSTR CArgumentManager::Get(CString strKey)
{
	map<CString, CString>::iterator tar = m_mapArgument.find(strKey);

	if( tar == m_mapArgument.end() ) return NULL;

	return tar->second;
}

BOOL	CArgumentManager::In(CString strKey)
{
	return m_mapArgument.find(strKey) != m_mapArgument.end();
}

BOOL CArgumentManager::Parsing(TCHAR** argv, int argc)
{
	if( !argv || argc < 2 ) return FALSE;

	for(int i = 1 ; i < argc ; ++i )
	{
		__ParsingArgument(argv[i]);
	}

	return TRUE;
}

long CArgumentManager::GetValidCommandCount(const vector<CString>& vecValidCommandList)
{
	if( vecValidCommandList.empty() ) return -1;

	long nValidCommandCount = 0;

	for(long i = 0 ; i < (long)vecValidCommandList.size() ; ++i )
	{
		if( In(vecValidCommandList[i]) ) nValidCommandCount++;
	}

	return nValidCommandCount;
}


void CArgumentManager::__ParsingArgument(CString arg)
{
	if( arg.Find('/') < 0 ) return;

	// 띄어쓰기 안하고 명령어를 나열했을 경우에 대한 예외처리 추가

	vector<CString> vecArg;

	int nPosCommand(0);

	// Command별 분리
	while( nPosCommand >= 0 )
	{
		CString strArg = arg.Tokenize(m_strTokenKey, nPosCommand);				//'/' Tokenize

		if( strArg.IsEmpty() ) break;

		vecArg.push_back(strArg);
	}

	// Command의 key, Value 분리
	for(int i = 0 ; i < (long)vecArg.size() ; ++i )
	{
		int nPosValue= 0;
		CString strKey = vecArg[i].Tokenize(m_strTokenValue, nPosValue);		//':' Tokenize
		CString strValue = vecArg[i].Mid(nPosValue);

		m_mapArgument[strKey.MakeLower()] = strValue;
	}
}