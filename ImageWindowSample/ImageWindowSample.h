
// ImageWindowSample.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CImageWindowSampleApp:
// �� Ŭ������ ������ ���ؼ��� ImageWindowSample.cpp�� �����Ͻʽÿ�.
//

class CImageWindowSampleApp : public CWinApp
{
public:
	CImageWindowSampleApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CImageWindowSampleApp theApp;