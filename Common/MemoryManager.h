#pragma once

#include <vector>
using namespace std;

class CMemoryManager
{
	ULONGLONG m_nTotalSize;
	BYTE* m_pbyBuffer;

	ULONGLONG m_nCurPos;
	ULONGLONG m_nReverseCurPos;

	ULONGLONG m_nMaxPos;
	ULONGLONG m_nPosReverseMaxPos;

	vector<ULONGLONG> m_vecRegisterdBufferSize;
	vector<ULONGLONG> m_vecRegisterdReverseBufferSize;
	
	vector<BYTE*> m_vecRegisterdBuffer;										///< 앞에서 부터 접근하는 비소멸 버퍼 포인터들
	vector<BYTE*> m_vecRegisterdReverseBuffer;								///< 뒤어서 부터 접근하는 소멸 버퍼 포인터들

	BOOL m_bNotEnoughMemory;

public:
	//==============================
	// 관리계층에서 사용할 함수
	//==============================
	BOOL Realloc(ULONGLONG nAllocSizeMB);
	ULONGLONG _GetLocalMemSize();												///< 현재 할당된 메모리 정보를 얻어온다.
	BOOL _SetLocalMemSize(ULONGLONG nSize);										///< 현재 할당된 메모리 위치를 강제로 지정한다.
	void _ClearAll();														///< 모든 버퍼를 해제한다.

	//==============================
	// 사용계층에서 사용할 함수
	//==============================
	// 임시 버퍼(이미지등에 사용하며 형제, 자식들에게만 사용할 용도)
	BYTE* ReserveTempBuffer(ULONGLONG nSize);									///< 임시 버퍼를 할당한다. (VMO에서의 사용을 권장한다.)
	BOOL RestoreTempBuffer(BYTE *pBufferPtr);								///< 임시 버퍼를 해제한다. (VMO에서의 사용을 권장한다.)

	// 버퍼 후불 사용(BlobInfo 외에 사용 자제)
	//BYTE* GetTempPointer();												///< 현재 사용가능한 버퍼 포인터를 얻는다.
	//BOOL SetTempUsedSize(UINT nUsedsize);									///< GetPointer()를 사용했다면, 사용한 사이즈 만큼 알려준다.
	
	// 버퍼 선불 사용(BlobInfo 외에 사용 자제)
	BYTE* ReserveBuffer(ULONGLONG nSize);										///< 모든 검사가 끝날때 까지 클리어되지 않는 버퍼를 할당한다.  BlobInfo에만 사용하고 나머지는 사용을 권장하지 않는다.
	BOOL RestoreBuffer(BYTE *pBufferPtr);									///< 버퍼를 해제한다. (사용을 권장하지 않는다.)

	ULONGLONG GetLastUsedSize();
	BOOL IsNotEnoughMemory();

public:
	CMemoryManager();
	~CMemoryManager(void);
private:
	CMemoryManager(ULONGLONG nAllocSizeMB);
};
