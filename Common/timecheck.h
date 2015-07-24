#pragma once

class CTimeCheck
{
private:
	LARGE_INTEGER	__st;
	LARGE_INTEGER   __ed;
	LARGE_INTEGER   __freq;

public:
	void	StartTime();
	float	EndTime();
	double	EndTimeD();
	
	void	Reset();
	float	Elapsed_ms();

	void	sleep( float waitmsec );

public:
	CTimeCheck();
	~CTimeCheck();
};

class CSingleTimeCheck
{
private:
	CTimeCheck __stc;
	float &__fRecorder;

public:
	CSingleTimeCheck(float &fRecorder);
	~CSingleTimeCheck();
};