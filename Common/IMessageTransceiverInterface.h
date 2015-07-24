#pragma once

typedef long (*MessageProcessingFunction)(void *pProcessor, LPCTSTR szMessage);

interface IMessageTransceiverInterface
{	
	virtual long InternalMessageProcessingFunction(LPCTSTR szMessage) = 0;
	virtual void SetExternalMessageProcessingFunction(void* pMessageProcessor, MessageProcessingFunction pMessageProcessingFunction)	= 0;

	virtual long	iGetDialogNum()=0;																// DLL�� ���� ��ŷâ�� ������ �����´�.
	virtual CString iGetDialogName(long nIndex)=0;													// DLL�� ���� ��ŷâ�� �̸��� �����´�.
	virtual BOOL	iCreateDialog(int nDialogIndex, HWND hParentWnd, CDialog **pCreatedDlg)=0;		// DLL�� ���� ��ŷâ�� �ε��Ѵ�.
	virtual BOOL	iDestroyDialog(int nDialogIndex, HWND hParentWnd, CDialog **pCreatedDlg)=0;		// DLL�� ���� ��ŷâ�� �ε��Ѵ�.

	virtual long	iGetMenuNum()=0;																	// DLL�� ���� �޴��� ������ �����´�.
	virtual CString iGetMenuName(long nIndex)=0;														// DLL�� ���� �޴��� �̸��� �����´�.
	virtual BOOL	OnClickedMenu(int nIndex)=0;													// DLL�� ���� �ش� �Ķ������ �޴��� ����.

	virtual BOOL OpenSetupWindow(HWND /*hwndParent*/, long /*nStyle*/, long &/*o_nWidth*/, long &/*o_nHeight*/) { return FALSE;}
	virtual BOOL OpenMainWindow(HWND /*hwndParent*/, long /*nStyle*/, long &/*o_nWidth*/, long &/*o_nHeight*/)	{ return FALSE;}
	virtual void CloseSetupWindow() {}
	virtual void CloseMainWindow() {}
	virtual void ShowMainWindow(int /*nCmdShow*/) {}
	virtual void ShowSetupWindow(int /*nCmdShow*/) {}
};