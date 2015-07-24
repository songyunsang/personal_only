#pragma once

typedef long (*MessageProcessingFunction)(void *pProcessor, LPCTSTR szMessage);

interface IMessageTransceiverInterface
{	
	virtual long InternalMessageProcessingFunction(LPCTSTR szMessage) = 0;
	virtual void SetExternalMessageProcessingFunction(void* pMessageProcessor, MessageProcessingFunction pMessageProcessingFunction)	= 0;

	virtual long	iGetDialogNum()=0;																// DLL이 가진 도킹창의 개수를 가져온다.
	virtual CString iGetDialogName(long nIndex)=0;													// DLL이 가진 도킹창의 이름을 가져온다.
	virtual BOOL	iCreateDialog(int nDialogIndex, HWND hParentWnd, CDialog **pCreatedDlg)=0;		// DLL이 가진 도킹창을 로딩한다.
	virtual BOOL	iDestroyDialog(int nDialogIndex, HWND hParentWnd, CDialog **pCreatedDlg)=0;		// DLL이 가진 도킹창을 로딩한다.

	virtual long	iGetMenuNum()=0;																	// DLL이 가진 메뉴의 개수를 가져온다.
	virtual CString iGetMenuName(long nIndex)=0;														// DLL이 가진 메뉴의 이름을 가져온다.
	virtual BOOL	OnClickedMenu(int nIndex)=0;													// DLL이 가진 해당 파라미터의 메뉴를 실행.

	virtual BOOL OpenSetupWindow(HWND /*hwndParent*/, long /*nStyle*/, long &/*o_nWidth*/, long &/*o_nHeight*/) { return FALSE;}
	virtual BOOL OpenMainWindow(HWND /*hwndParent*/, long /*nStyle*/, long &/*o_nWidth*/, long &/*o_nHeight*/)	{ return FALSE;}
	virtual void CloseSetupWindow() {}
	virtual void CloseMainWindow() {}
	virtual void ShowMainWindow(int /*nCmdShow*/) {}
	virtual void ShowSetupWindow(int /*nCmdShow*/) {}
};