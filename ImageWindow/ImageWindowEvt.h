#pragma once

#ifdef __IMAGEWINDOW_WRAPPER_CLASS_EXPORT__
#define __IMAGEWINDOW_WRAPPER_CLASS__	__declspec(dllexport)
#else
#define __IMAGEWINDOW_WRAPPER_CLASS__	__declspec(dllimport)
#endif

class __IMAGEWINDOW_WRAPPER_CLASS__ CImageWindowEvt
{
public:
	virtual BOOL OnLButtonDblClk(UINT /*nFlags*/, long /*ptx*/, long /*pty*/, float /*fImgPosX*/, float /*fImgPosY*/)								{	return FALSE;	}
	virtual BOOL OnLButtonDown	(UINT /*nFlags*/, long /*ptx*/, long /*pty*/, float /*fImgPosX*/, float /*fImgPosY*/, BOOL /*bMinimapHit*/)			{	return FALSE;	}
	virtual BOOL OnLButtonUp	(UINT /*nFlags*/, long /*ptx*/, long /*pty*/, float /*fImgPosX*/, float /*fImgPosY*/)								{	return FALSE;	}
	virtual BOOL OnMouseMove	(UINT /*nFlags*/, long /*ptx*/, long /*pty*/, float /*fImgPosX*/, float /*fImgPosY*/)								{	return FALSE;	}
	virtual BOOL OnRButtonDown	(UINT /*nFlags*/, long /*ptx*/, long /*pty*/, float /*fImgPosX*/, float /*fImgPosY*/)								{	return FALSE;	}
	virtual BOOL OnRButtonUp	(UINT /*nFlags*/, long /*ptx*/, long /*pty*/, float /*fImgPosX*/, float /*fImgPosY*/, BOOL /*bOnContextMenuCall*/)	{	return FALSE;	}
	virtual void OnROIChanged	(long /*nROIID*/)																					{					}
	virtual void OnExtPopupMenu	(long /*nCmdID*/, double /*fx*/, double /*fy*/)														{					}
	virtual BOOL OnKeyEvent		(MSG* /*pMsg*/)																						{	return FALSE;	}
	virtual void OnScroll		()	{}
	virtual void OnZoom			()	{}
};
