/*******************************************************
* Project Name	: WBI Project
* Program Name	: WBI
* File Name		: ClientSocket.CPP
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
*******************************************************/

#include "stdafx.h"
#include "ClientSocket.h"

// CClientSocket

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const long	SIZE_SOCKET_BUFFER	= 4096;

CClientSocket::CClientSocket()
	: m_nID(-1)
	, m_nBufSendSize(0)
	, m_nPortNo(0)
	, m_bConnect(FALSE)
	, m_pBufSend(NULL)
	, m_pParentPtr(NULL)
{
}

CClientSocket::~CClientSocket()
{
	Close();
	if(nullptr != m_pBufSend)
	{
		delete [] m_pBufSend;
		m_pBufSend = nullptr;
	}
}

void CClientSocket::InitClient(int nID, HWND hWnd, BOOL bUseReceiveCallback, void *pParentPtr, void (*CallbackReceive)(void *pServerSock, int nID, BYTE *pBuf, long nSize))
{
	m_nID					= nID;
	m_hParentWnd			= hWnd;
	__pCallbackReceive		= CallbackReceive;
	m_pParentPtr			= pParentPtr;
	m_bUseReceiveCallback	= bUseReceiveCallback;
}

BOOL CClientSocket::IsConnected()
{
	return m_bConnect;
}

BOOL CClientSocket::ConnectToServer(LPCTSTR pszClientIP, LPCTSTR pszServerIP, UINT nPortNo)
{
	m_strClientIP	= pszClientIP;
	m_strServerIP	= pszServerIP;
	m_nPortNo		= nPortNo;
	m_bConnect		= FALSE;

	if(m_hSocket == INVALID_SOCKET)
	{
		if(!Create())//0, SOCK_STREAM, FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE, m_strClientIP))
			return FALSE;
	}
	return Connect(m_strServerIP, m_nPortNo);
}

void CClientSocket::CloseSock()
{
	ShutDown(2);
	Close();
}

void CClientSocket::OnReceive(int nErrorCode)
{
	// 메세지를 받는다.
	BYTE byReceive[SIZE_SOCKET_BUFFER];
	memset( byReceive, 0, SIZE_SOCKET_BUFFER);
	int nSize = Receive(byReceive, SIZE_SOCKET_BUFFER);

	if(0 != nSize && SOCKET_ERROR != nSize)
	{
		long nBufferRead = 0;
		long nBufferRemain = long(nSize);
		LPBYTE lpData = byReceive;

		while(nBufferRemain > 0)
		{
			if(!m_DataPackage.Begin(lpData, nBufferRead, nBufferRemain))
			{
				continue;
			}

			m_DataPackage.Process(lpData, nBufferRead, nBufferRemain);

			if(!m_DataPackage.IsCompleted())
			{
				continue;
			}

			if(m_bUseReceiveCallback)
				__pCallbackReceive(m_pParentPtr, m_nID, m_DataPackage.GetData(), m_DataPackage.GetLength());
			else
				::SendMessage(m_hParentWnd, UWM_SOCKET_ALL_DATA_RECEIVED, m_nID, (LPARAM)m_DataPackage.GetData());

			m_DataPackage.Reset();
		}
	}
	else
	{
		// 에러 처리
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

/**********************************
* 2010년 3월 10일 Lee Jin-Ho ( OnConnect(int nErrorCode) )
* Remark : Connect의 상태를 m_bConnect에 저장, host와 connect상태 전송시 값 사용 (Run_Etc.cpp)
***********************************/
void CClientSocket::OnConnect(int nErrorCode)
{
	if (0 != nErrorCode)
	{
		if(m_hParentWnd)
			::PostMessage(m_hParentWnd, UWM_SOCKET_UPDATE_CONNECTION, m_nID, EVT_CONFAILURE);
		else
			__pCallbackReceive(m_pParentPtr, m_nID, (BYTE *) - 1, EVT_CONFAILURE);

		switch( nErrorCode )
		{
			case WSAEADDRINUSE:
				TRACE("The specified address is already in use.\n");
				break;
			case WSAEADDRNOTAVAIL:
				TRACE("The specified address is not available from the local machine.\n");
				break;
			case WSAEAFNOSUPPORT:
				TRACE("Addresses in the specified family cannot be used with this socket.\n");
				break;
			case WSAECONNREFUSED:
				TRACE("The attempt to connect was forcefully rejected.\n");
				break;
			case WSAEDESTADDRREQ:
				TRACE("A destination address is required.\n");
				break;
			case WSAEFAULT:
				TRACE("The lpSockAddrLen argument is incorrect.\n");
				break;
			case WSAEINVAL:
				TRACE("The socket is already bound to an address.\n");
				break;
			case WSAEISCONN:
				TRACE("The socket is already connected.\n");
				break;
			case WSAEMFILE:
				TRACE("No more file descriptors are available.\n");
				break;
			case WSAENETUNREACH:
				TRACE("The network cannot be reached from this host at this time.\n");
				break;
			case WSAENOBUFS:
				TRACE("No buffer space is available. The socket cannot be connected.\n");
				break;
			case WSAENOTCONN:
				TRACE("The socket is not connected.\n");
				break;
			case WSAENOTSOCK:
				TRACE("The descriptor is a file, not a socket.\n");
				break;
			case WSAETIMEDOUT:
				TRACE("The attempt to connect timed out without establishing a connection. \n");
				break;
			default:
				char szError[256];
				sprintf_s(szError, 255, "OnConnect error: %d", nErrorCode);
				TRACE(szError);
				break;
		}
		m_bConnect = FALSE;
	}
	else
	{
		// 커넥트 되었음을 알림
		m_bConnect = TRUE;
		if(m_hParentWnd)
			::PostMessage(m_hParentWnd, UWM_SOCKET_UPDATE_CONNECTION, m_nID, EVT_CONSUCCESS);
		else
			__pCallbackReceive(m_pParentPtr, m_nID, (BYTE *) - 1, EVT_CONSUCCESS);
	}

	m_DataPackage.Reset();

	CAsyncSocket::OnConnect(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode)
{
	m_bConnect = FALSE;

	m_DataPackage.Reset();

	CAsyncSocket::OnClose(nErrorCode);

	// 연결 끊김을 알림
	if(m_hParentWnd)
		::PostMessage(m_hParentWnd, UWM_SOCKET_UPDATE_CONNECTION, m_nID, EVT_CONDROP);
	else
		__pCallbackReceive(m_pParentPtr, m_nID, (BYTE *) - 1, EVT_CONDROP);
}

BOOL CClientSocket::SendMsg(BYTE *pBuf, long nSizeBuf)
{
	if(m_bConnect == FALSE)
		return FALSE;

	if(m_pBufSend && m_nBufSendSize < nSizeBuf + (long)sizeof(long))
	{
		delete []m_pBufSend;
		m_pBufSend = nullptr;
	}
	if(m_pBufSend == nullptr)
	{
		m_nBufSendSize = nSizeBuf + sizeof(long);
		m_pBufSend = new BYTE[m_nBufSendSize];
		ASSERT(m_pBufSend);
	}

	long nSendSize = nSizeBuf +	sizeof(long);

	memcpy(m_pBufSend + 0,				&nSizeBuf,	sizeof(long));
	memcpy(m_pBufSend + sizeof(long),	pBuf,		nSizeBuf);

	return (Send(m_pBufSend, nSendSize) != SOCKET_ERROR);
}

CString CClientSocket::GetLocalComputerIP()
{
	char		chName[255];
	CString		strAddress;
	PHOSTENT	pHostEntry;
	IN_ADDR		inAddr;

	//추가
	WSADATA wsaData;

	if (WSAStartup(0x0002, &wsaData) == SOCKET_ERROR)
	{
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	}
	// 추가 끝
	if( gethostname( chName, 255 ) != 0 )
		return FALSE;
	else
	{
		if( ( pHostEntry = gethostbyname( chName ) ) == NULL )
			return FALSE;
		else
		{
			memcpy( &inAddr, pHostEntry->h_addr, 4 );

			CStringA strA(inet_ntoa( inAddr ));
			strAddress = strA;
			return strAddress;
		}
	}
}

CDataPackage::CDataPackage()
	: m_bIdle(TRUE)
	, m_nPackageLength(0)
	, m_nPackageLengthReadBytes(0)
	, m_nProcessedLength(0)
	, m_nNecessaryLength(0)
	, m_nRawBufferLength(0)
	, m_pData(NULL)
{
}

CDataPackage::CDataPackage(const CDataPackage &Src)
	: m_bIdle(Src.m_bIdle)
	, m_nPackageLength(Src.m_nPackageLength)
	, m_nPackageLengthReadBytes(Src.m_nPackageLengthReadBytes)
	, m_nProcessedLength(Src.m_nProcessedLength)
	, m_nNecessaryLength(Src.m_nNecessaryLength)
	, m_nRawBufferLength(Src.m_nRawBufferLength)
	, m_pData(new BYTE [Src.m_nRawBufferLength])
{
	memcpy(m_pData, Src.m_pData, Src.m_nRawBufferLength);
}

CDataPackage::~CDataPackage()
{
	delete [] m_pData;
	m_pData = nullptr;
}

CDataPackage &CDataPackage::operator = (const CDataPackage &Src)
{
	m_bIdle						= Src.m_bIdle;
	m_nPackageLength			= Src.m_nPackageLength;
	m_nPackageLengthReadBytes	= Src.m_nPackageLengthReadBytes;
	m_nProcessedLength			= Src.m_nProcessedLength;
	m_nNecessaryLength			= Src.m_nNecessaryLength;
	m_nRawBufferLength			= Src.m_nRawBufferLength;

	delete [] m_pData;
	m_pData = nullptr;
	m_pData = new BYTE [Src.m_nRawBufferLength];

	memcpy(m_pData, Src.m_pData, Src.m_nRawBufferLength);

	return *this;
}


BOOL CDataPackage::Begin(LPBYTE &pbyBuffer, long &nBufferRead, long &nBufferRemain)
{
	// 세션을 진행중이었을 경우 그냥 나감
	if(!m_bIdle)
		return TRUE;

	for(long n = m_nPackageLengthReadBytes ; n < sizeof(long) ; n++)
	{
		((BYTE *)&m_nPackageLength)[n] = *pbyBuffer;

		pbyBuffer++;
		nBufferRead++;
		nBufferRemain--;

		m_nPackageLengthReadBytes++;

		// 버퍼는 다 읽었는데, Package Length 는 아직 다 채워지지 않았다면 나감.
		if(nBufferRemain <= 0 && m_nPackageLengthReadBytes < sizeof(long))
		{
			return FALSE;
		}
	}

	if(m_nPackageLength > m_nRawBufferLength)
	{
		delete [] m_pData;
		m_pData = nullptr;
		m_nRawBufferLength = m_nPackageLength;
		m_pData = new BYTE[m_nRawBufferLength];
	}

	m_nNecessaryLength = m_nPackageLength;
	m_bIdle = FALSE;

	return TRUE;
}

void CDataPackage::Process(LPBYTE &pbyBuffer, long &nBufferRead, long &nBufferRemain)
{
	long nLengthToRead = min(nBufferRemain, m_nNecessaryLength);

	memcpy(m_pData + m_nProcessedLength, pbyBuffer, nLengthToRead);

	pbyBuffer		+= nLengthToRead;
	nBufferRead		+= nLengthToRead;
	nBufferRemain	-= nLengthToRead;

	m_nProcessedLength += nLengthToRead;
	m_nNecessaryLength = m_nPackageLength - m_nProcessedLength;
}

BOOL CDataPackage::IsCompleted()
{
	return (m_bIdle == FALSE) && (m_nPackageLength > 0) && (m_nPackageLengthReadBytes == sizeof(long)) && (m_nNecessaryLength <= 0);
}

void CDataPackage::Reset()
{
	m_bIdle						= TRUE;
	m_nPackageLength			= 0;
	m_nPackageLengthReadBytes	= 0;
	m_nProcessedLength			= 0;
	m_nNecessaryLength			= 0;
}

LPBYTE CDataPackage::GetData()
{
	return m_pData;
}

long CDataPackage::GetLength()
{
	return m_nPackageLength;
}

