﻿//*******************************************************************************
/** @file basevmo.h
*   @brief 외부에서 접근 가능한 VMO의 객체 인터페이스 정의
*   
*   - Project Name        :  
*   - Description         : 
*   - Author              : Han won sup
*   - date                : 2007-04-13 오전 9:14:53
*/ //****************************************************************************

#pragma once

#include "basedef.h"

interface IBaseVisionObject
{
	virtual ~IBaseVisionObject(){};

	// 설정
	virtual BOOL iLinkDatabase(BOOL bSave, void *pJobDB)=0;							///< 설정을 읽는다.
	virtual long iGetPropertyNum()=0;															///< 외부에서 설정할 속성 값의 갯수를 얻는다.
	virtual SBASESPECTYPE* iGetPropertyInfo(int nIndex)=0;										///< 속성값의 타입 정보를 얻는다.
	virtual void iSetPropertyInfo(int nIndex, LPCTSTR szData)=0;								///< 속성을 지정한다.
	virtual BOOL iOpenSetupWindow(HWND i_hParentWnd, HINSTANCE i_hInst, long i_nStyle, long *o_nWidth, long *o_nHeight)=0;		///< 설정창을 연다.
	virtual BOOL iCloseSetupWindow()=0;															///< 설정창을 닫는다.

	// 입력
	virtual long iGetInPinNum()=0;																///< 입력 데이터의 갯수를 얻는다.
	virtual SBASEDATA_INTERFACE* iGetInPinInfo(int nIndex)=0;									///< 입력 데이터의 정보를 얻는다.
	virtual SBASEDATA_INTERFACE* iGetInPinInfo(LPCTSTR szName)=0;								///< 입력 데이터의 정보를 얻는다.
	virtual BOOL iConnectInPin(int nIndex, SBASEDATA_INTERFACE *pData)=0;						///< 입력 데이터를 연결한다.

	// 실행
	virtual BOOL iInspRun()=0;																	///< 검사를 수행한다.
	virtual BOOL iTeachRun()=0;																	///< UI의 내용을 적용한뒤 검사를 수행한다.

	// 출력
	virtual long iGetResultNum()=0;																///< 결과의 갯수를 얻는다. (basedef.h의 II_DATATYPE 참조)
	virtual SBASEDATA_INTERFACE* iGetResultEnum(int nIndex)=0;									///< 결과를 나열한다.
	virtual SBASEDATA_INTERFACE* iGetResultEnum(LPCTSTR szName)=0;								///< 결과를 나열한다.

	// 예외처리
	virtual BOOL iGetException(LPCTSTR szName, void **o_pData, long *o_nSize)=0;				///< 예외처리를 수행한다.
	virtual BOOL iSetException(LPCTSTR szName, void *i_pData, long i_nSize)=0;					///< 예외처리를 수행한다.
};

   