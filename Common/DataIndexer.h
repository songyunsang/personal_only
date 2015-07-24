#pragma once

#include <vector>
#include "unit.h"

#define NOTFOUND (-1)

template<typename T>	// template type을 포인터로 사용하면 안됨, smart ptr 지원되는 class를 구현해서 사용해야 함
class CDataIndexer
{
private:
	std::vector<CString>	m_vecName;
	std::vector<T>			m_vecData;

public:
	void SetData(long nIndex, const T& data)
	{
		if( OutOfRange(m_vecData, nIndex) ) return;

		m_vecData[nIndex] = data;
	}

	void SetName(long nIndex, LPCTSTR szName)
	{
		if( OutOfRange(m_vecName, nIndex) ) return;

		m_vecName[nIndex] = szName;
	}

	long Find(LPCTSTR szName) const
	{
		for(long i = 0 ; i < (long)m_vecName.size() ; ++i )
		{
			if( m_vecName[i] == szName ) return i;
		}

		return NOTFOUND;
	}

	long Count() const
	{
		return (long)m_vecData.size();
	}

	void Clear()
	{
		m_vecName.clear();
		m_vecData.clear();
	}

	long Add(LPCTSTR szName, const T& data)
	{
		m_vecName.push_back(szName);
		m_vecData.push_back(data);

		return Count() - 1;
	}

	CDataIndexer& operator = (const CDataIndexer &rhs)
	{
		Clear();

		for(long i = 0 ; i < rhs.Count() ; ++i )
		{
			Add(rhs.GetName(i), rhs.GetData(i));
		}

		return *this;
	}

	BOOL Del(long nIndex)
	{
		if( OutOfRange(m_vecData, nIndex) ) return FALSE;

		m_vecName.erase(m_vecName.begin() + nIndex);
		m_vecData.erase(m_vecData.begin() + nIndex);

		return TRUE;
	}

	BOOL Del(LPCTSTR szName)
	{
		return Del(Find(szName));
	}


	LPCTSTR	 GetName(long nIndex) const
	{
		if( OutOfRange(m_vecName, nIndex) ) return _T("");

		return m_vecName[nIndex];
	}

	const T& FindData(LPCTSTR szName) const
	{
		return GetData(Find(szName));
	}

	const T& GetData(long nIndex) const
	{
		if( OutOfRange(m_vecData, nIndex) )
		{
			static T invalid;

			return invalid;
		}

		return m_vecData[nIndex];
	}

	void SetData(LPCTSTR szName, const T& data)
	{
		SetData(Find(szName), data);
	}
};

