#pragma once

#define AUTHORIZESIGNATURE _T("[intekplus authorize signature]")

class CPCAuthorizer
{
public:
	static void Activate();
	static void Deactivate();
	static BOOL IsAuthorized();

public:
	CPCAuthorizer(void);
	~CPCAuthorizer(void);
};

