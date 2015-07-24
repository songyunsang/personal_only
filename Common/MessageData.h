//**************************************************************************************************
/**
        @file   MessageData.h
        @brief  
        @author 
        @date   2007-06-22 오전 2:21:15
        @note   iVisionMadang에서 사용하는 사용자정의 메시지를 Define 한다
*/
//**************************************************************************************************
#pragma once

// 사용자정의 메시지 Define 방법
//--------------------------------------------------------------------------------------------------
// #define WM_[메시지를보내는곳]_[메시지명]		WM_USER + 번호
//--------------------------------------------------------------------------------------------------
#define WM_VISIONMADANG_AGENTCMD			WM_USER+0x0500
#define WM_AGENT_EVENTCMD					WM_USER+0x1500
#define WM_USER_INSPECTIONPROC_END			WM_USER+1011
#define WM_USER_IQM_MODULE					WM_USER+1000									/// 추후 적당한 위치와 값을 지정해야 함

// Control ID : 생성된 UI(Docking Bar)의 리소스 ID  (생성된 UI없을 경우엔 -1)
// lParam : 해당하는 파라메터의 포인터
#define WM_EXTERNAL_UIMESSAGE		WM_USER+1015

#define VMN_CHANGESELECT_BLOCK				10001
	// lParam : (long) nBlockID
#define VMN_CHANGESELECT_BLOCKITEM			10003
	// lParam : (long) wBlockID + wVMOIndex
#define VMN_CHANGEMODE_VMOTEACH				10005
	// lParam : (long) wBlockID + wVMOIndex
#define VMN_CHANGEMODE_NORMAL				10007
	// lParam : (long) wBlockID + wVMOIndex
#define VMN_ADDITEM_VMO						10009	// lParam : (long) wBlockID + wItemIndex
#define VMN_ADDITEM_NORMAL_BLOCK			10011	// lParam : (long) wBlockID + wItemIndex
#define VMN_ADDITEM_LIBRARY_BLOCK			10013
#define VMN_ADDITEM_EMPTYLIBRARY_BLOCK		10015

#define VMN_DELITEM							10017	// lParam : (long) wBlockID + wItemIndex
#define VMN_EXECUTE_ALL						10019	// lParam : NULL
#define VMN_EXECUTE_VMO						10021	// lParam : (long) wBlockID + wItemIndex
#define VMN_MOVEITEM						10023	// lParam : (long) wBlockID + byIndex + byTargetIndex
	struct SVMITEM_MOVE
	{
		long nBlockID;
		long nDragID;
		long nDropID;
	};

#define VMN_CHANGE_INPUTPIN					10025	// lParam : (long) wBlockID + wItemIndex
#define VMN_SET_EXCEPTION_COMMAND			10026	// lParam : (long) wBlockID + wItemIndex
#define VMN_DRAWIIDATA_SVMITEM				10027	// lParam : SVMITEM_MOVE
	// lParam : SVMITEM_IIDATA
	struct SVMITEM_IIDATA
	{
		BOOL bClear;			// Clear후 그리기
		long nBlockID;			// IQM일 경우 VMOID_IQM
		long nVMOIndex;			// IQM일 경우 IQM Index, -1일경우 VMOIQM
		CString strPinName;
		long nSubIndex;
	};

#define VMN_CONNECTPIN						10029
	struct SVMITEM_CONNECTION
	{
		long nBlockID;
		long nVMOIndex;
		long nPinConnIndex;
		CString strPinName;
		long nTargetVMOIndex;
		CString strTargetPinName;
		long nTargetSubClusterIndex;
	};

#define VMN_BLOCK_ADDRESULT						10031	// lParam : SVMITEM_IIDATA
#define VMN_BLOCK_DELRESULT						10033	// lParam : SVMITEM_IIDATA				(예외 : nVMOID는 데이터 인덱스로 사용한다.)
#define VMN_BLOCK_RENAMERESULT					10035	// lParam : SVMITEM_IIDATA
#define VMN_BLOCK_MOVEUPRESULT					10037	// lParam : SVMITEM_IIDATA
#define VMN_BLOCK_MOVEDOWNRESULT				10039	// lParam : SVMITEM_IIDATA
#define VMN_CHANGE_VMONAME						10041	// lParam : (long) wBlockID + wItemIndex
#define VMN_BLOCK_EXPORT						10043	// lParam : (long) lBlockID
#define VMN_BLOCK_IMPORT						10045	// lParam : (long) wBlockID + wInsertIndex
#define VMN_BLOCK_CLIPBOARD_IMPORT				10047	// lParam : (long) wBlockID + wInsertIndex
#define VMN_BLOCK_IMPORT_REPLACE				10049	// lParam : (long) wBlockID + wInsertIndex
#define VMN_RUN_ALL								10051	// lParam : null
#define VMN_RUN_ITEM							10053	// lParam : (long) wBlockID + wItemIndex
#define VMN_MODIFIED_VMO						10055	// lParam : (long) wBlockID + wIsRealInsp
#define VMN_VMO_INSTANT_LOAD					10057	// lParam : (long) wBlockID + wItemIndex
#define VMN_VMO_INSTANT_SAVE					10059	// lParam : (long) wBlockID + wItemIndex

#define VMN_RESULT_ADD_INSPBLOCK				10061	// lParam : (long) wBlockID
#define VMN_RESULT_DEL_INSPBLOCK				10063	// lParam : (long) wBlockID

#define VMN_RESULT_ADDMEASUREDITEM				10065	// lParam : SVMITEM_IIDATA*
#define VMN_RESULT_DELMEASUREDITEM				10067	// lParam : SVMITEM_IIDATA*
#define VMN_DRAWIIDATA_OVERLAYCLEAR				10069	// lParam : null
#define VMN_DRAWIIDATA_SBASEDATA				10071	// lParam : SBASEDATA_INTERFACE*
#define VMN_REFRESH_REALTIMECOLLECTOR			10073	// lParam : null
#define VMN_BATCH_START							10075	// lParam : 0:처음부터, -1:현재부터, 0> 해당Index부터
#define VMN_BATCH_STOP							10077	// lParam : null
#define VMN_SHOW_IQM_SETUPWINDOW				10079	// lParam : null
#define VMN_OVERLAYCOLLECTOR_ADDGROUP			10081	// lParam : LPCTSTR*
#define VMN_OVERLAYCOLLECTOR_REMOVEGROUP		10083	// lParam : (long)nGroupIndex
#define VMN_OVERLAYCOLLECTOR_RENAMEGROUP		10085	// lParam : SVMITEM_IIDATA*						(사용하는 멤버는 nBlockID, strPinName)
#define VMN_OVERLAYCOLLECTOR_ADDRESULT			10087	// lParam : SVMITEM_IIDATA*
#define VMN_OVERLAYCOLLECTOR_REMOVERESULT		10089	// lParam : SVMITEM_IIDATA*
#define VMN_LIBRARYRESOURCE_ADDRESULT			10091	// lParam : SVMITEM_IIDATA*
#define VMN_LIBRARYRESOURCE_ADDRESULTALLOUTPUT	10093
#define VMN_LIBRARYRESOURCE_DELRESULT			10095	// lParam : SVMITEM_IIDATA*
#define VMN_SETLIBRARY							10097
#define VMN_SETNORMALBLOCK						10099

inline void SendVMNMessage(HWND hWnd, WORD nMsg, WORD nResID, LPARAM lParam)
{
	WPARAM wP = MAKEWPARAM(nMsg, nResID);
	::SendMessage(hWnd, WM_EXTERNAL_UIMESSAGE, wP, lParam);
}
inline void PostVMNMessage(HWND hWnd, WORD nMsg, WORD nResID, LPARAM lParam)
{
	WPARAM wP = MAKEWPARAM(nMsg, nResID);
	::PostMessage(hWnd, WM_EXTERNAL_UIMESSAGE, wP, lParam);
}

inline void ProcessAMsg()
{
	MSG msg;
	if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}