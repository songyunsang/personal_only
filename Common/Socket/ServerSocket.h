/*******************************************************
* Project Name	: WBI Project
* Program Name	: WBI
* File Name		: ServerSocket.h
* Developer		: Joonho You
* Created Date	: 2010-01-21 By Joonho YouV
* Version No	: 0.01
* Last Update	: 2010-01-21
* Copyright		: Intekplus R&D
* Module with	:
* Explanation	: This is for communicating between Rorze(Host PC) and Intekplus(Vision PC) through TCP/IP
                  We adapt AsyncSocket into our TCP/IP module because it is more genenal, flexible and faster than
				  normal Socket. I refer to several books such as "Visual C++ Programming Bible Ver. 6.x(�̻� ��)",
				  "��ǻ�� ��Ʈ�� ���α׷���(��ȭ�� ��)".
*******************************************************/
#ifndef		__SERVER_SOCKET_H__
#define		__SERVER_SOCKET_H__

#pragma once

#include "ClientSocket.h"
#include <vector>
#include <afxmt.h>

class CServerSocket : public CAsyncSocket
{
private:
	UINT m_nPort;
	HWND m_hParentWnd;
	void *m_pParentPtr;
	long m_nClientStartID;
	BOOL m_bUseReceiveCallback;

	std::vector<BOOL>				m_vecnbClientUsed;
	std::vector<CClientSocket *>		m_vecpClient;

	CCriticalSection				m_csClientLock;

	long m_nBufSendSize;
	BYTE *m_pBufSend;

private:
	void (*__CallbackDataReceive)(void *pParentPtr, int nID, BYTE *pData, long nDataSize);
public:
	// Ŭ���̾�Ʈ�� ���� 1���� ����ϴ� ��� (ID�� ������ �� �ִ�.)
	BOOL Create1to1Server(UINT nPort, HWND hWnd, long nID                            , BOOL bUseReceiveCallback = FALSE, void *pParentPtr = NULL, void (*CallbackReceive)(void *pParentPtr, int nID, BYTE *pBuf, long nSize) = NULL);

	// ���ӵǴ� Ŭ���̾�Ʈ�� ID�� nClientStartID�� �������� nMaxClient�� ���� �Ǿ��ִ�
	BOOL Create1toNServer(UINT nPort, HWND hWnd, long nMaxClient, long nClientStartID, BOOL bUseReceiveCallback = FALSE, void *pParentPtr = NULL, void (*CallbackReceive)(void *pParentPtr, int nID, BYTE *pBuf, long nSize) = NULL);

	void CloseClientSocket(long nClientID);
	BOOL IsConnected(long nClientID);
	BOOL IsConnectedAny();


	BOOL SendMsg(long nClientID, BYTE *pBuf, int nSizeBuf);
	BOOL SendMsgAllClient(BYTE *pBuf, int nSizeBuf);

	CClientSocket *GetConnectedSocket(int nClientID);

public:
	CServerSocket();
	virtual ~CServerSocket();
	virtual void OnAccept(int nErrorCode);
};


#endif // __SERVER_SOCKET_H__

