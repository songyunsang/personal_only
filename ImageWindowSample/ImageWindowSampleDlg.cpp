
// ImageWindowSampleDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ImageWindowSample.h"
#include "ImageWindowSampleDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CImageWindowSampleDlg ��ȭ ����



CImageWindowSampleDlg::CImageWindowSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageWindowSampleDlg::IDD, pParent)
	, m_pImageWindow(new CImageWindow(NULL))
	, m_pScreenshotImage(new CDIBImage)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CImageWindowSampleDlg::~CImageWindowSampleDlg()
{
	delete m_pImageWindow;

	delete m_pScreenshotImage;

}

void CImageWindowSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageWindowSampleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CImageWindowSampleDlg �޽��� ó����

BOOL CImageWindowSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	CDC* pDC = GetDesktopWindow()->GetDC();

	CRect rtScreenResolution;
	GetDesktopWindow()->GetWindowRect(&rtScreenResolution);
	rtScreenResolution = CRect(0, 0, 1920 * 2, 1080);
	m_pScreenshotImage->Create(pDC, rtScreenResolution.Width(), rtScreenResolution.Height(), 32);
	CScreenshot::Shot(pDC, rtScreenResolution, m_pScreenshotImage->GetDIB());
	ReleaseDC(pDC);


	//CxImage image;

	//image.Load(_T("c:\\Koala.jpg"), CXIMAGE_FORMAT_JPG);

	//image.Save(_T("c:\\loadomage.bmp"), CXIMAGE_FORMAT_BMP);
	//image.Save(_T("c:\\loadomage.jpg"), CXIMAGE_FORMAT_JPG);
	//image.Save(_T("c:\\loadomage.png"), CXIMAGE_FORMAT_PNG);
	//m_pScreenshotImage->SaveBMP(_T("c:\\screenshot.bmp"));

	m_pImageWindow->Create(this, IDC_IMAGEVIEW);
	m_pImageWindow->SetDisplayImage(m_pScreenshotImage->GetPtr(), m_pScreenshotImage->GetSizeX(), m_pScreenshotImage->GetSizeY(), m_pScreenshotImage->GetStepX(), m_pScreenshotImage->GetBpp());

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	SetTimer(777, 33, NULL);


	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CImageWindowSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CImageWindowSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CImageWindowSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CImageWindowSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 777:
		KillTimer(777);
		OnMonitoring();
		SetTimer(777, 1, NULL);
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CImageWindowSampleDlg::OnMonitoring()
{
	CDC* pDC = GetDesktopWindow()->GetDC();

	CRect rtScreenResolution;
	GetDesktopWindow()->GetWindowRect(&rtScreenResolution);
	rtScreenResolution = CRect(0, 0, 1920*2, 1080);
	m_pScreenshotImage->Create(pDC, rtScreenResolution.Width(), rtScreenResolution.Height(), 32);
	CScreenshot::Shot(pDC, rtScreenResolution, m_pScreenshotImage->GetDIB());
	ReleaseDC(pDC);

	m_pImageWindow->SetDisplayImage(m_pScreenshotImage->GetPtr(), m_pScreenshotImage->GetSizeX(), m_pScreenshotImage->GetSizeY(), m_pScreenshotImage->GetStepX(), m_pScreenshotImage->GetBpp());
}