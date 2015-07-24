/*******************************************************
* Project Name	: WBI Project
* Program Name	: WBI
* File Name		: ServerSocket.cpp
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
#include "ServerSocket.h"


// CServerSocket

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CServerSocket::CServerSocket()
	: m_nPort(0)
	, m_nBufSendSize(0)
	, m_pBufSend(NULL)
	, __CallbackDataReceive(NULL)
{
}

CServerSocket::~CServerSocket()
{
	// 접속된 소켓은 모두 닫는다.
	for(long i = 0; i < (long)m_vecpClient.size(); i++)
	{
		CloseClientSocket(i);
	}

	Close();

}
BOOL CServerSocket::Create1to1Server(UINT nPort, HWND hWnd, long nID, BOOL bUseReceiveCallback, void *pParentPtr, void (*CallbackReceive)(void *pParentPtr, int nID, BYTE *pBuf, long nSize))
{
	return Create1toNServer(nPort, hWnd, 1, nID, bUseReceiveCallback, pParentPtr, CallbackReceive);
}

BOOL CServerSocket::Create1toNServer(UINT nPort, HWND hWnd, long nMaxClient, long nClientStartID, BOOL bUseReceiveCallback, void *pParentPtr, void (*CallbackReceive)(void *pParentPtr, int nID, BYTE *pBuf, long nSize))
{
	m_nPort					= nPort;
	m_hParentWnd			= hWnd;
	m_bUseReceiveCallback	= bUseReceiveCallback;
	__CallbackDataReceive	= CallbackReceive;
	m_pParentPtr			= pParentPtr;
	m_nClientStartID		= nClientStartID;

	m_vecpClient.resize(nMaxClient);
	m_vecnbClientUsed.resize(nMaxClient);
	fill(m_vecpClient.begin(), m_vecpClient.end(), (CClientSocket *)NULL);
	fill(m_vecnbClientUsed.begin(), m_vecnbClientUsed.end(), 0);

	Create(nPort);
	return Listen();
}

void CServerSocket::CloseClientSocket(long nClientID)
{
	if(m_vecnbClientUsed[nClientID] != TRUE)
		return;
	if(m_vecpClient[nClientID] == NULL)
		return;

	CClientSocket *pCurClient = m_vecpClient[nClientID];
	pCurClient->ShutDown();
	pCurClient->Close();

	delete pCurClient;
	pCurClient = nullptr;

	m_vecpClient[nClientID] = NULL;
	m_vecnbClientUsed[nClientID] = FALSE;

	TRACE("CServerSocket::CloseClientSocket\n");
}

void CServerSocket::OnAccept(int nErrorCode)
{
	CSingleLock sl(&m_csClientLock, TRUE);

	if( nErrorCode != 0 )
	{
		TRACE("CServerSocket::OnAccept Error : %d\n", ::WSAGetLastError());
	}
	else
	{
		// 빈 클라이언트를 찾는다
		long nClientID = -1;
		long nMaxClient = (long)m_vecnbClientUsed.size();
		for(long i = 0; i < nMaxClient; i++)
		{
			if(m_vecnbClientUsed[i] == FALSE)
			{
				nClientID = i;
				break;
			}
		}
		if(nClientID != -1)
		{
			if(m_vecpClient[nClientID] == NULL)
				m_vecpClient[nClientID] = new CClientSocket;

			CClientSocket *pCurSocket = m_vecpClient[nClientID];
			pCurSocket->InitClient(m_nClientStartID + nClientID, m_hParentWnd, m_bUseReceiveCallback, m_pParentPtr, __CallbackDataReceive);
			m_vecnbClientUsed[nClientID] = TRUE;
			m_vecpClient[nClientID]->m_bConnect = TRUE;
			Accept(*pCurSocket, NULL, NULL);

			TRACE("CServerSocket::OnAccept\n");
			if(m_hParentWnd)
				::PostMessage(m_hParentWnd, UWM_SOCKET_UPDATE_CONNECTION, nClientID, EVT_CONSUCCESS);
			else
				__CallbackDataReceive(m_pParentPtr, nClientID, (BYTE *) - 1, EVT_CONSUCCESS);
		}
		else
		{
			CSocket Socket;
			Accept(Socket, NULL, NULL);
			Socket.ShutDown();
			Socket.Close();
		}
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

BOOL CServerSocket::SendMsg(long nClientID, BYTE *pBuf, int nSizeBuf)
{
	CSingleLock sl(&m_csClientLock, TRUE);

	if(nClientID >= (long)m_vecnbClientUsed.size())
		return FALSE;
	if(m_vecnbClientUsed[nClientID] == FALSE)
		return FALSE;

	return m_vecpClient[nClientID]->SendMsg(pBuf, nSizeBuf);
}

BOOL CServerSocket::SendMsgAllClient(BYTE *pBuf, int nSizeBuf)
{
	BOOL bRet = TRUE;
	for(size_t i = 0; i < m_vecnbClientUsed.size(); i++)
	{
		if(m_vecnbClientUsed[i] == FALSE)
			continue;
		if(m_vecpClient[i] == NULL)
			continue;

		bRet &= m_vecpClient[i]->SendMsg(pBuf, nSizeBuf);
	}

	return bRet;
}

BOOL CServerSocket::IsConnected(long nClientID)
{
	if(nClientID >= (long)m_vecnbClientUsed.size())
		return FALSE;
	if(m_vecnbClientUsed[nClientID] == FALSE)
		return FALSE;

	return m_vecpClient[nClientID]->IsConnected();
}

BOOL CServerSocket::IsConnectedAny()
{
	for each(const BOOL & bCon in m_vecnbClientUsed)
	{
		if(bCon == TRUE)
			return TRUE;
	}
	return FALSE;
}

CClientSocket *CServerSocket::GetConnectedSocket(int nClientID)
{
	if(nClientID >= (long)m_vecnbClientUsed.size())
		return NULL;
	if(m_vecnbClientUsed[nClientID] == FALSE)
		return NULL;

	return m_vecpClient[nClientID];
}
