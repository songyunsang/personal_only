//**************************************************************************************************
/**
        @file   IVMFactory.h
        @brief  
        @author 
        @date   2007-06-12 오후 3:46:11
        @note   
*/
//**************************************************************************************************
#pragma once

#include "../iVMBaseLibrary/basedef.h"
#include "../iVMBaseLibrary/baseexport.h"

#include <vector>
#include <map>
//--------------------------------------------------------------------------------------------------

enum class IV_enumStatus;
class CObjDescription;

struct SIVM_DLL
{
	CString strFilePath;
	HINSTANCE hModule;
	long nClassNum;
	SIVM_DLL()
		: hModule(nullptr)
		, nClassNum(0)
	{
	};
};

struct SIVM_CLASS
{
	SIVM_DLL *pDll;														///< 클래스 정보가 포함된 dll의 인덱스
	CObjDescription descClass;													///< 공통 모듈의 정보
	long nClassID;														///< dll에서 객체에 해당하는 클래스 ID
	SIVM_CLASS()
		: pDll(nullptr)
		, nClassID(-1)
	{
	};
};

template <class T>
class SIVM_OBJECT
{
public:
	SIVM_CLASS *pClass;
	long nRefID;														///< 클래스의 객체 참조 ID
//	CString strName;													///< 객체 이름
	T *pInterface;														///< IV Object 인터페이스

	SIVM_OBJECT()
		: pClass(nullptr)
		, nRefID(-1)
		, pInterface(nullptr)
	{
	}
};

template <class T>
class SIVM_CLASSOBJECTMANAGER
{
public:
	std::vector<BOOL> vecbUsing;
	std::vector<SIVM_OBJECT<T>*> vecpObject;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
/**
        @class  CIVMFactory
        @brief  
        @author 
        @date   2007-06-12 오후 4:06:32
        @note   
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class CIVMFactory  
{
//--생성자		소멸자--------------------------------------------------
public:
	CIVMFactory();
	virtual ~CIVMFactory();
//----------------------------------------------------------------------

//--Public		멤버함수------------------------------------------------
public:
	BOOL InitIVMFactory(CString strModulePath, CString strExtension, BOOL bPopupErrorMessage = TRUE);	///< Factory를 초기화 한다
	BOOL InitIVMFactory(CString strModulePath, CString strExtension, std::vector<CString> &vecstrErrorFilePath);///< Factory를 초기화 한다
	BOOL UnInitFactory();																				///< Factory를 정리한다
	
	long GetDllNum();
	SIVM_DLL* GetDLLDesc(long nIndex);																	///< DLL의 정보를 얻는다

	long GetClassNum() const;																			///< 클래스의 갯수를 얻는다
	const SIVM_CLASS* GetClassDesc(long nIndex) const;														///< 클래스의 정보를 얻는다
	const SIVM_CLASS* GetClassDesc(CString strRegKey) const;													///< 클래스의 정보를 얻는다  GetDescription()
	const SIVM_CLASS* GetClassDescByName(CString strName) const;												///< 클래스의 정보를 얻는다  GetDescription()
	LPCTSTR GetClassNameByRegKey(LPCTSTR strRegKey) const;												///< RegKey를 사용하여 클래스 이름을 얻는다
	LPCTSTR GetClassRegKeyByName(LPCTSTR strName) const;												///< RegKey를 사용하여 클래스 이름을 얻는다

	long GetObjectNum() const;																			///< Object의 갯수를 얻는다
	SIVM_OBJECT<T>* GetObjectItem(long nIndex);															///< 생성된 Object를 얻는다.
	SIVM_OBJECT<T>* CreateObjectItem(long nClassIndex);													///< Object를 생성한다
	SIVM_OBJECT<T>* CreateObjectItem(CString strClassRegKey);											///< Object를 생성한다
	BOOL DeleteObjectItem(SIVM_OBJECT<T> *pObjectItem);													///< Object를 제거한다
//----------------------------------------------------------------------

//--private		멤버함수------------------------------------------------
private:
	BOOL AddDll(CString strFileName, BOOL bPopupErrorMessage=TRUE);
	BOOL RemoveDll(long nIndex);

//----------------------------------------------------------------------

//--Public		멤버변수------------------------------------------------
public:
	std::vector<SIVM_DLL*> m_vecIVDLLFile;																	///< Module 리스트
	std::vector<SIVM_OBJECT<T>*> m_vecIVObject;																///< 생성된 IQM Object Item 리스트
//----------------------------------------------------------------------

//--private		멤버변수------------------------------------------------
private:
	CString m_strManagePath;																			///< IQM 모듈을 관리하는 INI파일의 이름
	std::vector	<SIVM_CLASS*>			m_vecIVClass;														///< IQM 모듈 Description 리스트
	std::map	<CString, SIVM_CLASS*>	m_mapIVClass_RegKey;												///< IQM 모듈 Description 리스트 (Reg Key 찾기용)
//----------------------------------------------------------------------
};

template <class T>
CIVMFactory<T>::CIVMFactory(void)
{
	UnInitFactory();
}

template <class T>
CIVMFactory<T>::~CIVMFactory(void)
{
	UnInitFactory();
}

template <class T>
BOOL CIVMFactory<T>::InitIVMFactory(CString strModulePath, CString strExtension, BOOL bPopupErrorMessage)
{
	UnInitFactory();
	m_strManagePath = strModulePath;

	HANDLE hFindFile;
	WIN32_FIND_DATA info;
	CString strPath;

	strPath.Format(_T("%s\\*.%s"), strModulePath, strExtension);
	hFindFile = ::FindFirstFile((LPCTSTR)strPath, &info);
	if(hFindFile == INVALID_HANDLE_VALUE)
		return FALSE;

	do
	{
		strPath.Format(_T("%s\\%s"), strModulePath, info.cFileName);
		if(!AddDll(strPath, bPopupErrorMessage))
			continue;
	} while(::FindNextFile(hFindFile, &info));	
	::FindClose(hFindFile);

	return TRUE;
}

template <class T>
BOOL CIVMFactory<T>::UnInitFactory()
{
	// 생성된 객체를 모두 제거한다.
	for(long i=(long)m_vecIVObject.size()-1; i>=0; i--)
	{
		DeleteObjectItem(m_vecIVObject[i]);
	}

	// 생성된 클래스 정보를 제거한다.
	//for(long i=(long)m_vecIVClass.size()-1; i>=0; i--)
	//{
	//	delete m_vecIVClass[i]->pDll;
	//	delete m_vecIVClass[i];
	//}
	//m_vecIVClass.clear();

	// 생성된 dll 정보를 제거한다.
	for(long i=(long)m_vecIVDLLFile.size()-1; i>=0; i--)
	{
		RemoveDll(i);
	}

	return TRUE;
}

template <class T>
BOOL CIVMFactory<T>::AddDll(CString strFileName, BOOL bPopupErrorMessage)
{
	SIVM_DLL *pdllTemp = new SIVM_DLL;

	// DLL을 연다.
	pdllTemp->hModule = LoadLibrary(strFileName);

	if(pdllTemp->hModule == NULL)
	{
		if(bPopupErrorMessage)
		{
			CString strTemp = _T("Can not load file \r\n   :");
			strTemp += strFileName;
			AfxMessageBox(strTemp);
		}
		delete pdllTemp;
		return FALSE;
	}

	// 클래스가 몇개인지 알아낸다.
	pifGetClassNum *FuncGetClassNum = (pifGetClassNum*)GetProcAddress(pdllTemp->hModule, "exiGetClassNum");
	if(FuncGetClassNum)
		pdllTemp->nClassNum = FuncGetClassNum();
	else
	{
		FreeLibrary(pdllTemp->hModule);
		delete pdllTemp;
		return FALSE;
	}

	// 파일 이름을 기억하게 한다.
	pdllTemp->strFilePath = strFileName;

	// 클래스를 추가한다.
	for(long i=0; i<pdllTemp->nClassNum; i++)
	{
		SIVM_CLASS *pclsTemp = new SIVM_CLASS;

		// 포함되어있는 dll 등록
		pclsTemp->pDll = pdllTemp;
		pclsTemp->nClassID = i;

		// 설명 등록
		pifQueryDescription *FuncDescription;
		CObjDescription *pModuleDesc = NULL;
		FuncDescription = (pifQueryDescription*)GetProcAddress(pdllTemp->hModule, "exiQueryDescription");
		if(!FuncDescription)
			ASSERT(0);
		FuncDescription(i, &pModuleDesc);
		if(!pModuleDesc)
			ASSERT(0);

		// 중복키 확인
		for(long j=0; j<(long)m_vecIVClass.size(); j++)
		{
			if(pModuleDesc->strRegKey == m_vecIVClass[j]->descClass.strRegKey)
			{
				if(bPopupErrorMessage)
				{
					// 경고를 띄운다.
					CString strTemp = _T("Same name VMO Found.\r\n\r\nFilename1 : \"");
					strTemp += m_vecIVClass[j]->pDll->strFilePath;
					strTemp += _T("\"\r\n    ");
					strTemp += m_vecIVClass[j]->descClass.strRegKey;
					strTemp += _T("(");
					strTemp += m_vecIVClass[j]->descClass.strName;
					strTemp += _T(")\r\nFilename2 : \"");
					strTemp += strFileName;
					strTemp += _T("\"\r\n    ");
					strTemp += pModuleDesc->strRegKey;
					strTemp += _T("(");
					strTemp += pModuleDesc->strName;
					strTemp += _T(")\r\n\r\nCan't Regist VMO.");
					AfxMessageBox(strTemp);
				}
				delete pclsTemp;
				if(i==0)
				{
					FreeLibrary(pdllTemp->hModule);
					delete pdllTemp;
				}
				return FALSE;
			}
		}
		pclsTemp->descClass = *pModuleDesc;

		// VMO를 등록한다.
		m_vecIVClass.push_back(pclsTemp);
		m_mapIVClass_RegKey[pModuleDesc->strRegKey] = pclsTemp;
	}

	// DLL을 등록한다.
	m_vecIVDLLFile.push_back(pdllTemp);

	return TRUE;
}

template <class T>
BOOL CIVMFactory<T>::RemoveDll(long nIndex)
{
	if(nIndex >= (long)m_vecIVDLLFile.size())
	{
		ASSERT(0);
		return FALSE;
	}

	// dll에 포함되는 클래스를 제거한다.
	for(long i=(long)m_vecIVClass.size()-1; i>=0; i--)
	{
		if(m_vecIVClass[i]->pDll == m_vecIVDLLFile[nIndex])
		{
//			delete m_vecIVClass[i]->pDll;
			// Map 을 제거한다
			m_mapIVClass_RegKey[m_vecIVClass[i]->descClass.strRegKey] = nullptr;

			delete m_vecIVClass[i];
			m_vecIVClass[i] = nullptr;
			m_vecIVClass.erase(m_vecIVClass.begin() + i);
		}
	}

	// 라이브러리를 닫는다.
	FreeLibrary(m_vecIVDLLFile[nIndex]->hModule);
	
	// dll정보를 제거한다.
	delete m_vecIVDLLFile[nIndex];
	m_vecIVDLLFile[nIndex] = nullptr;
	m_vecIVDLLFile.erase(m_vecIVDLLFile.begin() + nIndex);

	return TRUE;
}

template <class T>
long CIVMFactory<T>::GetDllNum()
{
	return (long)m_vecIVDLLFile.size();
}

template <class T>
SIVM_DLL* CIVMFactory<T>::GetDLLDesc(long nIndex)
{
	if(nIndex >= (long)m_vecIVDLLFile.size())
	{
		ASSERT(0);
		return NULL;
	}

	return &m_vecIVDLLFile[nIndex];
}

template <class T>
long CIVMFactory<T>::GetClassNum() const
{
	return (long)m_vecIVClass.size();
}

template <class T>
const SIVM_CLASS* CIVMFactory<T>::GetClassDesc(long nIndex) const
{
	if(nIndex >= (long)m_vecIVClass.size())
	{
		ASSERT(0);
		return NULL;
	}

	return m_vecIVClass[nIndex];
}

template <class T>
const SIVM_CLASS* CIVMFactory<T>::GetClassDesc(CString strRegKey) const
{
	if(m_mapIVClass_RegKey.find(strRegKey) == m_mapIVClass_RegKey.end())
		return NULL;
	return m_mapIVClass_RegKey.find(strRegKey)->second;
}

template <class T>
const SIVM_CLASS* CIVMFactory<T>::GetClassDescByName(CString strName) const
{
	for(long i=0 ; i<GetClassNum() ; i++)
	{
		if(strName.Compare(m_vecIVClass[i]->descClass.strName) == 0)
			return m_vecIVClass[i];
	}

	return NULL;
}

template <class T>
LPCTSTR CIVMFactory<T>::GetClassNameByRegKey(LPCTSTR strRegKey) const
{
	SIVM_CLASS* pClass = m_mapIVClass_RegKey[strRegKey];

	if(pClass)
	{
		return pClass->descClass.strName;
	}

	return NULL;
}

template <class T>
LPCTSTR CIVMFactory<T>::GetClassRegKeyByName(LPCTSTR strName) const
{
	for(long i=0; i<(long)m_vecIVClass.size(); i++)
	{
		if(m_vecIVClass[i]->descClass.strName == strName)
			return (LPCTSTR)m_vecIVClass[i]->descClass.strRegKey;
	}
	return NULL;
}

template <class T>
long CIVMFactory<T>::GetObjectNum() const
{
	return (long)m_vecIVObject.size();
}

template <class T>
SIVM_OBJECT<T>* CIVMFactory<T>::CreateObjectItem(long nClassIndex)
{
	//=================================
	// 무결성 검사
	//=================================
	if(nClassIndex < 0 || nClassIndex > (long)m_vecIVClass.size())
		return nullptr;

	SIVM_CLASS *pClass = m_vecIVClass[nClassIndex];
	//=================================
	// 객체 생성
	//=================================
	
	pifCreateInstance *FuncCreate;
	FuncCreate = (pifCreateInstance*)GetProcAddress(pClass->pDll->hModule , "exiCreateInstance");
	if(!FuncCreate)
	{
		return nullptr;
	}

	SIVM_OBJECT<T> *pInsertObjectItem = new SIVM_OBJECT<T>;

	// 생성한다. (리턴값은 nReferID)
	pInsertObjectItem->nRefID = FuncCreate(pClass->nClassID);

	//==============================
	// 인터페이스 얻어오기
	//==============================
	pifQueryInterface *FuncQueryInterface;
	FuncQueryInterface = (pifQueryInterface*)GetProcAddress(pClass->pDll->hModule, "exiQueryInterface");
	if(!FuncQueryInterface)
	{
		delete pInsertObjectItem;
		return nullptr;
	}

	if(IV_enumStatus::SUCCESS != FuncQueryInterface(pInsertObjectItem->nRefID, (void**)&pInsertObjectItem->pInterface))
	{
		delete pInsertObjectItem;
		return nullptr;
	}

	ASSERT(pInsertObjectItem->pInterface);


	//==============================
	// 객체 관리자에 등록
	//==============================
	pInsertObjectItem->pClass = pClass;
	m_vecIVObject.push_back(pInsertObjectItem);

	return pInsertObjectItem;
}

template <class T>
SIVM_OBJECT<T>* CIVMFactory<T>::CreateObjectItem(CString strClassRegKey)
{
	//=================================
	// Class 검색
	//=================================
	//std::map	<CString, SIVM_CLASS*>::iterator tar = m_mapIVClass_RegKey.find(strClassRegKey);

	//if( tar == m_mapIVClass_RegKey.end() )
	//{
	//	for(std::map	<CString, SIVM_CLASS*>::iterator it = m_mapIVClass_RegKey.begin() ; it != m_mapIVClass_RegKey.end() ; it++)
	//	{
	//		if( it->second->descClass 
	//	}
	//}

	SIVM_CLASS *pClass = m_mapIVClass_RegKey[strClassRegKey];

	if(pClass)
	{
		pifCreateInstance *FuncCreate;
		FuncCreate = (pifCreateInstance*)GetProcAddress(pClass->pDll->hModule , "exiCreateInstance");
		if(!FuncCreate)
		{
			return nullptr;
		}

		SIVM_OBJECT<T> *pInsertObjectItem = new SIVM_OBJECT<T>;

		// 생성한다. (리턴값은 nReferID)
		pInsertObjectItem->nRefID = FuncCreate(pClass->nClassID);

		//==============================
		// 인터페이스 얻어오기
		//==============================
		pifQueryInterface *FuncQueryInterface;
		FuncQueryInterface = (pifQueryInterface*)GetProcAddress(pClass->pDll->hModule, "exiQueryInterface");
		if(!FuncQueryInterface)
		{
			delete pInsertObjectItem;
			return nullptr;
		}

		if(IV_enumStatus::SUCCESS != FuncQueryInterface(pInsertObjectItem->nRefID, (void**)&pInsertObjectItem->pInterface))
		{
			delete pInsertObjectItem;
			return nullptr;
		}

		ASSERT(pInsertObjectItem->pInterface);

		//==============================
		// 객체 관리자에 등록
		//==============================
		pInsertObjectItem->pClass = pClass;
		m_vecIVObject.push_back(pInsertObjectItem);

		return pInsertObjectItem;
	}

	long i;

	// 레지스트리 키로 찾지 못했다면 이름으로 다시 찾는다.
	for(i=0; i<(long)m_vecIVClass.size(); i++)
	{
		if(strClassRegKey.Compare(m_vecIVClass[i]->descClass.strName) == 0)
			break;
	}
	if(i==(long)m_vecIVClass.size()) return nullptr;

	return CreateObjectItem(i);
}

template <class T>
BOOL CIVMFactory<T>::DeleteObjectItem(SIVM_OBJECT<T> *pObjectItem)
{
	//=================================
	// 해당 OBJECT 검색
	//=================================
	long i;
	for(i=0; i<(long)m_vecIVObject.size(); i++)
	{
		if(pObjectItem == m_vecIVObject[i])
			break;
	}
	if(i==m_vecIVObject.size())
	{
		ASSERT(0);
		return FALSE;
	}

	SIVM_OBJECT<T> *pObj = m_vecIVObject[i];

	//=================================
	// 제거
	//=================================
	pifDeleteInstance *FuncDelete;
	FuncDelete = (pifDeleteInstance*)GetProcAddress(pObj->pClass->pDll->hModule, "exiDeleteInstance");
	if(!FuncDelete)
		return FALSE;
	FuncDelete(pObj->nRefID);

	delete pObj;

	// VMO 내에 Engine 이 있었을 경우 VMO 순서가 바뀌었을수 있으므로 다시 찾자
	for(i=0; i<(long)m_vecIVObject.size(); i++)
	{
		if(pObjectItem == m_vecIVObject[i])
			break;
	}
	if(i==m_vecIVObject.size())
	{
		ASSERT(0);
		return FALSE;
	}

	m_vecIVObject.erase(m_vecIVObject.begin() + i);

	return TRUE;
}

template <class T>
SIVM_OBJECT<T>* CIVMFactory<T>::GetObjectItem(long nIndex)
{
	if(nIndex >= (long)m_vecIVObject.size())
	{
		return NULL;
	}
	return m_vecIVObject[nIndex];
}
