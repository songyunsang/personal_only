
// ImageWindowSampleDlg.h : ��� ����
//

#pragma once

class CImageWindow;

// CImageWindowSampleDlg ��ȭ ����
class CImageWindowSampleDlg : public CDialog
{
// �����Դϴ�.
public:
	CImageWindowSampleDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CImageWindowSampleDlg();	

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_IMAGEWINDOWSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

	CImageWindow* m_pImageWindow;

	CDIBImage* m_pScreenshotImage;

	void OnMonitoring();
// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
