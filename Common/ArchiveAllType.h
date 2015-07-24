#pragma once

#ifdef _AFXDLL
#include <afx.h>
#endif

class ArchiveAllType : public CArchive  
{
public:
	ArchiveAllType(CFile* pFile, UINT nMode) : CArchive(pFile, nMode) {};
	virtual ~ArchiveAllType() {};

	template <class T> void Serialize_Element(T& t)
	{
		if(IsStoring())
			(*this) << t;
		else
			(*this) >> t;
	};

	typedef BYTE* BYTEPTR;

	void Serialize_Element(CString &istr)
	{
		//try
		//{
			int nValid = istr.IsEmpty() ? -1 : 1;

			Serialize_Element(nValid);

			if(nValid==1)
			{
				#ifdef _UNICODE
					if( IsStoring() )
					{
						(*this) << istr;
					}
					else
					{
						(*this) >> istr;
					}
				#else
					CStringW strWBlockName;
					strWBlockName = istr;

					if( IsStoring() )
					{
						(*this) << strWBlockName;
					}
					else
					{
						(*this) >> strWBlockName;
						istr = strWBlockName;
					}
				#endif
			}
		//}
		//catch(CArchiveException *ae)
		//{
		//	throw;
		//}
	}

	void Serialize_Element(CPoint &pt)
	{
		if( IsStoring() )
		{
			(*this) << pt.x;
			(*this) << pt.y;
		}
		else
		{
			(*this) >> pt.x;
			(*this) >> pt.y;
		}
	}

	void Serialize_Element(CSize &sz)
	{
		if( IsStoring() ){
			(*this) << sz.cx;
			(*this) << sz.cy;
		}
		else
		{
			(*this) >> sz.cx;
			(*this) >> sz.cy;
		}
	}

	void Serialize_Element(CRect &rt)
	{
		if( IsStoring() ){
			(*this) << rt.left;
			(*this) << rt.top;
			(*this) << rt.right;
			(*this) << rt.bottom;
		}else{
			(*this) >> rt.left;
			(*this) >> rt.top;
			(*this) >> rt.right;
			(*this) >> rt.bottom;
		}
	}

	void Serialize_Binary(BYTEPTR &pData, ULONG &nDataNum)
	{
		if( IsStoring() )
		{
			(*this) << nDataNum;
			Write(pData, nDataNum);
		}
		else
		{
			(*this) >> nDataNum;

			delete []pData;
			pData = nullptr;
			
			pData = new BYTE[nDataNum];
			Read(pData, nDataNum);
		}
	}

	void Serialize_RawData(BYTE *pData, ULONG nDataNum)
	{
		if( IsStoring() )
		{
			Write(pData, nDataNum);
		}
		else
		{
			Read(pData, nDataNum);
		}
	}
};
