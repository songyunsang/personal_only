#pragma once
#include <afxmt.h>

#include <vector>
using namespace std;

#pragma comment(lib, "gdiplus")
#include <gdiplus.h>
using namespace Gdiplus;

class CMouseTrace
{
public:
	__forceinline void Lock()
	{
		__CS.Lock();
	}

	__forceinline void Unlock()
	{
		__CS.Unlock();
	}

	void Init(Color argb, long nPenWidth = 0);

	void WayPoint(long nPosX, long nPosY);
	void WayPoint(const Point& point);

	CRect Boundary();

public:
	CMouseTrace(void);
	~CMouseTrace(void);

public:
	vector<Point> m_PointArray;

	long	m_nPenWidth;
	Color	m_argb;

private:
	CCriticalSection __CS;
};
