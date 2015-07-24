#pragma once

class CPacketSerialize
{
private:
	BOOL __bArchiving;
	BYTE *__pPacketBuffer;
	long __nPacketBufferSize;

	CMemFile *__pmemFile;
	CArchive *__pArchive;

public:
	CArchive& StartStoreArchive()
	{
		ASSERT(!__bArchiving);
		__bArchiving = TRUE;
		// 메모리를 새로 만든다.
		if(__pPacketBuffer != NULL)
			ReleaseArchive();
		__pmemFile = new CMemFile(1024);
		__pArchive = new CArchive(__pmemFile, CArchive::store);
		return *__pArchive;
	}
	CArchive& StartLoadArchive(BYTE *pPacketData, long nLen)
	{
		ASSERT(!__bArchiving);
		__bArchiving = TRUE;
		__pmemFile = new CMemFile(pPacketData, nLen, 0);
		__pArchive = new CArchive(__pmemFile, CArchive::load);
		return *__pArchive;
	}
	void EndStoreArchive(BYTE **pPacketBuffer=NULL, long *pnLen=NULL)
	{
		__pArchive->Close();
		long nDataLength = (long)__pmemFile->GetPosition();
		long nNewMemSize = (long)__pmemFile->GetLength();
		BYTE *pNewBuffer = __pmemFile->Detach();
		if(__pPacketBuffer != pNewBuffer)
		{
			__pPacketBuffer = pNewBuffer;
			__nPacketBufferSize = nNewMemSize;
		}

		if(pPacketBuffer)
			*pPacketBuffer = __pPacketBuffer;
		if(pnLen)
			*pnLen = nDataLength;

		delete __pArchive;
		__pArchive = nullptr;

		delete __pmemFile;
		__pmemFile = nullptr;

		__bArchiving = FALSE;
	};

	void EndLoadArchive()
	{
		__pArchive->Close();

		delete __pArchive;
		__pArchive = nullptr;

		delete __pmemFile;
		__pmemFile = nullptr;

		__bArchiving = FALSE;
	}

	BYTE* GetArchivedBuffer()
	{
		return __pPacketBuffer;
	}
	long GetArchivedSize()
	{
		return __nPacketBufferSize;
	}

	void ReleaseArchive()
	{
		if(__pPacketBuffer)
		{
			delete __pPacketBuffer;
			__pPacketBuffer = nullptr;
		}
		__nPacketBufferSize = 0;
	}
	CPacketSerialize()
	{
		__bArchiving = FALSE;
		__nPacketBufferSize = 0;//10*1024;
		__pPacketBuffer = NULL;//new BYTE[nLen];
	};

	virtual ~CPacketSerialize()
	{
		ReleaseArchive();
	}
};

