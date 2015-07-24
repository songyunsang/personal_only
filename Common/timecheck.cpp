#include "stdafx.h"
#include "TimeCheck.h"

#include <windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTimeCheck::CTimeCheck()
{
}
CTimeCheck::~CTimeCheck()
{
}

void CTimeCheck::StartTime()
{
	QueryPerformanceCounter( &__st );
}
float CTimeCheck::EndTime()
{
	QueryPerformanceCounter		( &__ed	  );
	QueryPerformanceFrequency	( &__freq );

	// 소요 시간 계산
	double gap = ( (double) (__ed.QuadPart - __st.QuadPart )) / ( (double) __freq.QuadPart ) ;

	return (float)(gap * 1000.f); // msec
}
double CTimeCheck::EndTimeD()
{
	QueryPerformanceCounter		( &__ed	  );
	QueryPerformanceFrequency	( &__freq );

	// 소요 시간 계산
	double gap = ( (double) (__ed.QuadPart - __st.QuadPart )) / ( (double) __freq.QuadPart ) ;

	return (gap * 1000.0); // msec
}

void CTimeCheck::Reset()
{
    QueryPerformanceCounter( &__st );
}
float CTimeCheck::Elapsed_ms()
{
	// 생성된 시점에서 현재까지의 시간을 계산해서 보내주기
	LARGE_INTEGER ed;

    QueryPerformanceCounter( &ed ) ;
  
    // 소요 시간 계산 
    double gap = ( (double) (ed.QuadPart - __st.QuadPart )) / ( (double) __freq.QuadPart ) ;

	return (float)(gap * 1000.f); // msec
}

//* Pauses for a specified number of milliseconds. */
void CTimeCheck::sleep( float fwaitmsec )//2003.12.12 LHJ
{
	StartTime();
	while( EndTime() < fwaitmsec );
}


CSingleTimeCheck::CSingleTimeCheck(float &fRecorder)
	: __fRecorder(fRecorder)
{
	__stc.StartTime();
}

CSingleTimeCheck::~CSingleTimeCheck()
{
	__fRecorder = __stc.EndTime();
}