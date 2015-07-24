/*******************************************************
* Project Name	: WBI Project
* Program Name	: WBI
* File Name		: ClientSocket.h
* Developer		: Joonho You
* Created Date	: 2010-01-21 By Joonho YouV
* Version No	: 0.01
* Last Update	: 2010-01-21
* Copyright		: Intekplus R&D
* Module with	:
* Explanation	: This is for communicating between Rorze(Host PC) and Intekplus(Vision PC) through TCP/IP
                  We adapt AsyncSocket into our TCP/IP module because it is more genenal, flexible and faster than
				  normal Socket. I refer to several books such as "Visual C++ Programming Bible Ver. 6.x(이상엽 저)",
				  "컴퓨터 네트웍 프로그래밍(김화중 저)".
				  원섭 수정함
*******************************************************/

#pragma once

enum
{
    UWM_SOCKET_UPDATE_CONNECTION = (WM_USER + 0x1334),
    UWM_SOCKET_ALL_DATA_RECEIVED,
};
enum enumSocketCommEvent
{
    EVT_CONSUCCESS = 0,
    EVT_CONFAILURE,
    EVT_CONDROP,
};

class CDataPackage
{
public:
	CDataPackage(void);
	~CDataPackage(void);

	BOOL Begin(LPBYTE &pbyBuffer, long &nReadLength, long &nBufferRemain);
	void Process(LPBYTE &pbyBuffer, long &nReadLength, long &nBufferRemain);
	BOOL IsCompleted();
	void Reset();
	LPBYTE GetData();
	long GetLength();

private:
	BOOL		m_bIdle;
	long		m_nPackageLength;
	long		m_nPackageLengthReadBytes;
	long		m_nProcessedLength;
	long		m_nNecessaryLength;
	long		m_nRawBufferLength;
	LPBYTE		m_pData;

private:	// Deprecated
	CDataPackage(const CDataPackage &Src);
	CDataPackage &operator=(const CDataPackage &Src);
};

class CClientSocket : public CAsyncSocket
{
private:
	int m_nID;

	CDataPackage m_DataPackage;

	long m_nBufSendSize;
	BYTE *m_pBufSend;
	HWND m_hParentWnd;					// Parent window
	BOOL m_bUseReceiveCallback;

	UINT  m_nPortNo;					// connection port
	CString m_strServerIP;
	CString m_strClientIP;
	TCHAR  m_szServerIP[MAX_PATH];      // connection IP
	TCHAR  m_szClientIP[MAX_PATH];		// client IP
	BOOL m_bConnect;					// connect error (0 -> good , 1->error)

private:
	void *m_pParentPtr;
	void (*__pCallbackReceive)(void *pParentPtr, int nID, BYTE *pBuf, long nSize);

public:
	void InitClient(int nID, HWND hWnd, BOOL bUseReceiveCallback, void *pParentPtr = NULL, void (*CallbackReceive)(void *pParentPtr, int nID, BYTE *pBuf, long nSize) = NULL);
	BOOL ConnectToServer(LPCTSTR pszClientIP, LPCTSTR pszServerIP, UINT nPortNo);
	BOOL IsConnected();
	void CloseSock();

	BOOL SendMsg(BYTE *pBuf, long nSizeBuf);

	static CString GetLocalComputerIP();

	friend class CServerSocket;

public:
	CClientSocket();
	virtual ~CClientSocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);

};



