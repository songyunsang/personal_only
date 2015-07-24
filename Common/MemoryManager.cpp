#include "StdAfx.h"
#include "MemoryManager.h"
//#include "basedef.h"
#include "log_text.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMemoryManager::CMemoryManager(void)
	: m_pbyBuffer(nullptr)
	, m_nCurPos(0)
	, m_nReverseCurPos(0)
	, m_nMaxPos(0)
	, m_nPosReverseMaxPos(0)
	, m_bNotEnoughMemory(TRUE)
{
}

CMemoryManager::CMemoryManager(ULONGLONG nAllocSizeMB)
{
	Realloc(nAllocSizeMB);
}

CMemoryManager::~CMemoryManager(void)
{
	// 버퍼 제거
	if(m_pbyBuffer)
	{
		GlobalFree(m_pbyBuffer);
		m_pbyBuffer = nullptr;
	}
}

BOOL CMemoryManager::Realloc(ULONGLONG nAllocSizeMB)
{
	ULONGLONG ullAllocSizeBytes = nAllocSizeMB * 1024 * 1024;

	// 기존 버퍼 제거
	if(m_pbyBuffer)
	{
		//LOG_TEXT(_T("MemeryManager - GlobalFree : [ address : %x ], [size : %ld] \r\n"), m_pbyBuffer, ullAllocSizeBytes);

		GlobalFree(m_pbyBuffer);
		m_pbyBuffer = nullptr;
	}

	// 새 버퍼 생성
	m_pbyBuffer = (BYTE *)GlobalAlloc(GPTR, (SIZE_T)ullAllocSizeBytes);
	//LOG_TEXT(_T("MemeryManager - GlobalAlloc : [ address : %x ], [size : %ld] \r\n"), m_pbyBuffer, ullAllocSizeBytes);

	if(m_pbyBuffer == nullptr)
	{
		// 할당이 안될경우 로그에 원인을 남긴다.
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		              NULL,
		              GetLastError(),
		              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		              (LPTSTR)&lpMsgBuf, 0, NULL);
		MEMORYSTATUS ms;
		GlobalMemoryStatus(&ms);

		LOG_TEXT(_T("MemeryManager - GlobalAlloc Fail : [ address : %x ] [size : %ld] %s\r\n"), m_pbyBuffer, ullAllocSizeBytes, lpMsgBuf);
		LOG_TEXT(_T("MemeryManager - Memory Info : %d percent Use\r\n\t\tPageFile : %d/%d\r\n\t\tPhysical : %d/%d\r\n\t\tVirtual : %d/%d\r\n"), ms.dwMemoryLoad, ms.dwAvailPageFile, ms.dwTotalPageFile, ms.dwAvailPhys, ms.dwTotalPhys, ms.dwAvailVirtual, ms.dwTotalVirtual);
		
		LocalFree(lpMsgBuf);

		ASSERT(0);
		return FALSE;
	}

	m_nCurPos			= 0;
	m_nTotalSize		= ullAllocSizeBytes;
	m_bNotEnoughMemory	= FALSE;

	return TRUE;
}

ULONGLONG CMemoryManager::_GetLocalMemSize()
{
	return m_nCurPos;
}

BOOL CMemoryManager::_SetLocalMemSize(ULONGLONG nSize)
{
	long j = -1;
	long nPrevSize = (long)m_vecRegisterdBuffer.size();
	for(j = (long)m_vecRegisterdBuffer.size() - 1; j >= 0; j--)
	{
		if(m_vecRegisterdBuffer[j] + m_vecRegisterdBufferSize[j] == m_pbyBuffer + nSize)
			break;

		RestoreTempBuffer(m_vecRegisterdBuffer[j]);
	}

	if(nPrevSize != 0 && j == -1 && nSize != 0)
	{
		// nSize는 잘못 사용한 메모리를 가르키고 있음.  중대 오류.
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

void CMemoryManager::_ClearAll()
{
	// 전역 할당 메모리를 지운다.
	m_nCurPos = 0;
	m_vecRegisterdBuffer.clear();
	m_vecRegisterdBufferSize.clear();


	// 역 할당 메모리를 지운다.
	m_nReverseCurPos = 0;
	m_vecRegisterdReverseBuffer.clear();
	m_vecRegisterdReverseBufferSize.clear();

	m_nMaxPos			= 0;
	m_nPosReverseMaxPos = 0;
	m_bNotEnoughMemory	= FALSE;
}

BYTE *CMemoryManager::ReserveTempBuffer(ULONGLONG nSize)
{
	if(m_nCurPos + nSize > m_nTotalSize)
	{
		m_bNotEnoughMemory = TRUE;
		return NULL;
	}

	BYTE *pBuffer = (m_pbyBuffer + m_nCurPos);
	m_vecRegisterdBuffer.push_back(pBuffer);
	m_vecRegisterdBufferSize.push_back(nSize);
	m_nCurPos += nSize;
	
	if(m_nMaxPos < m_nCurPos)
	{
		m_nMaxPos = m_nCurPos;
	}

	return pBuffer;
}

BOOL CMemoryManager::RestoreTempBuffer(BYTE *pBufferPtr)
{
	if(!pBufferPtr)
	{
		ASSERT(0);
		return FALSE;
	}

	if(m_vecRegisterdBuffer.size() == 0) return FALSE;
	ASSERT(m_vecRegisterdBuffer[m_vecRegisterdBuffer.size() - 1] == pBufferPtr);

	m_nCurPos -= m_vecRegisterdBufferSize[m_vecRegisterdBufferSize.size() - 1];

	m_vecRegisterdBuffer.pop_back();
	m_vecRegisterdBufferSize.pop_back();

	return TRUE;
}

BYTE *CMemoryManager::ReserveBuffer(ULONGLONG nSize)
{
	if(m_nReverseCurPos + nSize > m_nTotalSize - m_nCurPos)
	{
		m_bNotEnoughMemory = TRUE;
		return NULL;
	}

	m_nReverseCurPos += nSize;
	if(m_nPosReverseMaxPos < m_nReverseCurPos)
	{
		m_nPosReverseMaxPos = m_nReverseCurPos;
	}

	BYTE *pBuffer = (m_pbyBuffer + m_nTotalSize - m_nReverseCurPos);
	m_vecRegisterdReverseBuffer.push_back(pBuffer);
	m_vecRegisterdReverseBufferSize.push_back(nSize);
	return pBuffer;
}

BOOL CMemoryManager::RestoreBuffer(BYTE *pBufferPtr)
{
	if(!pBufferPtr)
	{
		ASSERT(0);
		return FALSE;
	}

	ASSERT(m_vecRegisterdReverseBuffer[m_vecRegisterdReverseBuffer.size() - 1] == pBufferPtr);

	m_nReverseCurPos -= m_vecRegisterdReverseBufferSize[m_vecRegisterdReverseBufferSize.size() - 1];

	m_vecRegisterdReverseBuffer.pop_back();
	m_vecRegisterdReverseBufferSize.pop_back();

	return TRUE;
}

ULONGLONG CMemoryManager::GetLastUsedSize()
{
	return m_nMaxPos + m_nPosReverseMaxPos;
}

BOOL CMemoryManager::IsNotEnoughMemory()
{
	return m_bNotEnoughMemory;
}
