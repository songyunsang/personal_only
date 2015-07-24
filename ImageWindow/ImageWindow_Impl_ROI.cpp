#include "stdafx.h"
#include "ImageWindowApp.h"
#include "ImageWindow_Impl.h"
#include "ROIDrawBase.h"
#include "ROIDrawRect.h"
#include "ROIEditBase.h"
#include "ROIEditRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const long ROI_SELECT_REGION   = 5;

const long ROI_MOVE         = 0;
const long ROI_RESIZE_X1    = 1;
const long ROI_RESIZE_X2    = 2;
const long ROI_RESIZE_Y1    = 3;
const long ROI_RESIZE_Y2    = 4;
const long ROI_RESIZE_XY1   = 5;
const long ROI_RESIZE_XY2   = 6;
const long ROI_RESIZE_XY3   = 7;
const long ROI_RESIZE_XY4   = 8;

const long ROI_SIZEMIN		= 2;

////////////////////////////////////////////////////////////////////////////////////////////////

void CImageWindow_Impl::uDrawROIs(CDC* pDC)
{
	if( !m_isROIDraw ) return;
	if(m_vecROIDraw.size()==0) return;

	CBrush *pOldBrush = (CBrush *)pDC->SelectStockObject( NULL_BRUSH );
	pDC->SelectObject(&m_ROIFont);     
	int nOldBkMode = pDC->SetBkMode( TRANSPARENT );
	CPen *pOldPen = pDC->GetCurrentPen();

	for(unsigned int i=0;i<m_vecROIDraw.size();i++)
	{				
		uDrawROI(pDC, m_vecROIDraw[i]);
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SetBkMode( nOldBkMode );
}

void CImageWindow_Impl::uDrawROI(CDC* pDC, const CROIDrawBase* pROIDraw)
{
	if( !pROIDraw ) return;

	pROIDraw->Draw(pDC);
}

CROIGeometryBase* CImageWindow_Impl::GetROIGeometry(long nIndex)
{
	if( OutOfRange(m_vecROIGeo, nIndex) ) return NULL;

	return m_vecROIGeo[nIndex];
}

void CImageWindow_Impl::uDrawROIEditor(CDC* pDC, CROIEditBase* pROIEdit)
{
	if( !pROIEdit ) return;

	pROIEdit->Build();
	pROIEdit->Draw(pDC);
}

BOOL CImageWindow_Impl::IsROIEditMode() const
{
	return m_bROIEditMode;
}

int  CImageWindow_Impl::FindROI(const CPoint& pos)
{
	for(long i = (long)m_vecROIDraw.size() - 1 ; i >= 0 ; --i)
	{
		if( m_vecROIDraw[i]->m_DrawPara.isMovable == FALSE && m_vecROIDraw[i]->m_DrawPara.isResizable == FALSE ) continue;
		if( m_vecROIGeo[i]->PtInROI(pos) ) return i;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int CImageWindow_Impl::GetMinimumDistance( CPoint cp, const EditROI& roi  )
{
	if( roi.Data.nType != 0 ) return m_imgOriginalImage.nWidth + m_imgOriginalImage.nHeight;

	CRect rt = roi.Data.rt;

	rt.InflateRect( ROI_SELECT_REGION, ROI_SELECT_REGION );
	if(!rt.PtInRect(cp))
		return  m_imgOriginalImage.nWidth + m_imgOriginalImage.nHeight; //전혀 관계없음을 의미.

	int nd[4];
	nd[0] = abs(cp.x - roi.Data.rt.left) ; // x1
	nd[1] = abs(cp.y - roi.Data.rt.top) ; // y1
	nd[2] = abs(cp.x - roi.Data.rt.right) ; // x2
	nd[3] = abs(cp.y - roi.Data.rt.bottom) ; // y2

	int nMin = nd[0];
	for(int i=1;i<4;i++)
		if( nd[i]<nMin ) nMin = nd[i];

	return nMin;
}


////////////////////////////////////////////////////////////////////////////////////////////////
int CImageWindow_Impl::GetDirection( CPoint cp, CRect &rt, BOOL isResizable, BOOL isMovable )
{
	int nd[4];
	nd[0] = abs(cp.x - rt.left) ; // x1
	nd[1] = abs(cp.y - rt.top) ; // y1
	nd[2] = abs(cp.x - rt.right) ; // x2
	nd[3] = abs(cp.y - rt.bottom) ; // y2
	
	// 끝 꼭지점에 붙어있고 사이즈가 2 이하일 경우
	if(rt.left == 0 && 
		rt.top == 0 && 
		rt.right <= ROI_SIZEMIN && 
		rt.bottom <= ROI_SIZEMIN)
		return ROI_RESIZE_XY4;

	if(rt.right == m_imgOriginalImage.nWidth && 
		rt.top == 0 && 
		rt.left > m_imgOriginalImage.nWidth - ROI_SIZEMIN && 
		rt.bottom <= ROI_SIZEMIN)
		return ROI_RESIZE_XY3;

	if(rt.left == 0 && 
		rt.bottom == m_imgOriginalImage.nHeight && 
		rt.right <= ROI_SIZEMIN && 
		rt.top > m_imgOriginalImage.nHeight - ROI_SIZEMIN )
		return ROI_RESIZE_XY2;

	if(rt.right == m_imgOriginalImage.nWidth && 
		rt.bottom == m_imgOriginalImage.nHeight && 
		rt.left > m_imgOriginalImage.nWidth - ROI_SIZEMIN && 
		rt.top > m_imgOriginalImage.nHeight - ROI_SIZEMIN )
		return ROI_RESIZE_XY1;

	if( !isResizable )
	{
		if(isMovable)
			return ROI_MOVE;
		else
			return -1;
	}

	if( nd[0]<ROI_SELECT_REGION )
	{
		if( nd[1]<ROI_SELECT_REGION )
			return ROI_RESIZE_XY1;
		else if( nd[3]<ROI_SELECT_REGION )
			return ROI_RESIZE_XY3;
		else if( abs(nd[1]-nd[3]) < ROI_SELECT_REGION )
			return ROI_RESIZE_X1;
		else
		{
			if(isMovable) return ROI_MOVE;
			else return -1;
		}
	}

	if( nd[2]<ROI_SELECT_REGION )
	{
		if( nd[1]<ROI_SELECT_REGION )
			return ROI_RESIZE_XY2;
		else if( nd[3]<ROI_SELECT_REGION )
			return ROI_RESIZE_XY4;
		else if( abs(nd[1]-nd[3]) < ROI_SELECT_REGION )
			return ROI_RESIZE_X2;
		else
		{
			if(isMovable)
				return ROI_MOVE;
			else
				return -1;
		}
	}

	if( nd[1] < ROI_SELECT_REGION )
	{
		if( nd[0] < ROI_SELECT_REGION )
			return ROI_RESIZE_XY1;
		else if( nd[2] < ROI_SELECT_REGION )
			return ROI_RESIZE_XY3;
		if( abs(nd[0]-nd[2]) < ROI_SELECT_REGION )
			return ROI_RESIZE_Y1;
		else
		{
			if(isMovable)
				return ROI_MOVE;
			else
				return -1;
		}
	}
	if( nd[3] < ROI_SELECT_REGION )
	{
		if( nd[0] < ROI_SELECT_REGION )
			return ROI_RESIZE_XY3;
		else if( nd[2] < ROI_SELECT_REGION )
			return ROI_RESIZE_XY4;
		if( abs(nd[0]-nd[2]) < ROI_SELECT_REGION )
			return ROI_RESIZE_Y2;
		else
		{
			if(isMovable) 
				return ROI_MOVE;
			else return -1;
		}
	}
	
	if(isMovable)
		return ROI_MOVE;
	else
		return -1;

}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////



void CImageWindow_Impl::ROICursorChange( int nROI_Dir )
{
	switch(nROI_Dir){
		case ROI_MOVE:
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			break;
		case ROI_RESIZE_X1:
		case ROI_RESIZE_X2:
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			break;
		case ROI_RESIZE_Y1:
		case ROI_RESIZE_Y2:
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			break;
		case ROI_RESIZE_XY1:
		case ROI_RESIZE_XY4:
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			break;
		case ROI_RESIZE_XY2:
		case ROI_RESIZE_XY3:
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
			break;
		default:
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;
	}	 

}

void CImageWindow_Impl::ROIEditMode(int nROI_Dir)
{
	switch(nROI_Dir)
	{
		case ROI_MOVE:
		case ROI_RESIZE_X1:
		case ROI_RESIZE_X2:
		case ROI_RESIZE_Y1:
		case ROI_RESIZE_Y2:
		case ROI_RESIZE_XY1:
		case ROI_RESIZE_XY4:
		case ROI_RESIZE_XY2:
		case ROI_RESIZE_XY3:
			m_bROIEditMode = TRUE;
			break;
		default:
			m_bROIEditMode = FALSE;
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void  CImageWindow_Impl::uUpdateROI( int nSelected, CPoint cp )
{
	ASSERT(0);
	////if( cp.x < -ROI_SELECT_REGION ) cp.x = 0;
	////if( cp.y < -ROI_SELECT_REGION ) cp.y = 0;
	////if( cp.x >= m_imgShowSelImage.nWidth + ROI_SELECT_REGION ) 	cp.x = m_imgShowSelImage.nWidth-1;
	////if( cp.y >= m_imgShowSelImage.nHeight + ROI_SELECT_REGION )	cp.y = m_imgShowSelImage.nHeight-1;

	//int dx = cp.x - m_ptStart.x;
	//int dy = cp.y - m_ptStart.y;

	//if((int)m_vecEditROI.size() <= nSelected)
	//{
	////	CString str;
	////	str.Format("원섭 호출!!\nnSelected : %d\t ROINum : %d", nSelected, m_vecEditROI.size());
	////	MessageBox(str);
	//	return ;
	//}

	//CRect &rt = m_vecEditROI[nSelected].Data.rt;
	//CRect rtTemp(0, 0, 0, 0);
	//
	//switch(m_nROI_Dir)
	//{
	//	case ROI_MOVE:
	//		rtTemp = rt;
	//		rtTemp.OffsetRect( dx, dy );
	//		if(rtTemp.left < 0)
	//		{
	//			rtTemp.right += -rtTemp.left;
	//			rtTemp.left = 0;
	//		}
	//		if(rtTemp.top < 0)
	//		{
	//			rtTemp.bottom += -rtTemp.top;
	//			rtTemp.top = 0;
	//		}
	//		if(rtTemp.right > m_imgOriginalImage.nWidth)
	//		{
	//			rtTemp.left -= rtTemp.right - (m_imgOriginalImage.nWidth);
	//			rtTemp.right = m_imgOriginalImage.nWidth;
	//		}
	//		if(rtTemp.bottom > m_imgOriginalImage.nHeight)
	//		{
	//			rtTemp.top -= rtTemp.bottom - (m_imgOriginalImage.nHeight);
	//			rtTemp.bottom = m_imgOriginalImage.nHeight;
	//		}
	//		if(	rtTemp.left<0 || 
	//			rtTemp.top<0 ||
	//			rtTemp.right>m_imgOriginalImage.nWidth ||
	//			rtTemp.bottom>m_imgOriginalImage.nHeight )
	//		{
	//			return;
	//		}
	//		else
	//		{
	//			m_ptStart = cp;
	//			rt = rtTemp;
	//		}
	//		break;
	//	case ROI_RESIZE_X1:
	//		rt.left += dx;
	//		if( rt.Width() < ROI_SIZEMIN ){
	//			rt.left = max(0, rt.right - ROI_SIZEMIN);
	//		}
	//		if(rt.left < 0)	rt.left = 0;
	//		break;
	//	case ROI_RESIZE_X2:
	//		rt.right += dx;
	//		if( rt.Width() < ROI_SIZEMIN ){
	//			rt.right = min(m_imgOriginalImage.nWidth, rt.left + ROI_SIZEMIN);
	//		}
	//		if(rt.right > m_imgOriginalImage.nWidth)	rt.right = m_imgOriginalImage.nWidth;
	//		break;
	//	case ROI_RESIZE_Y1:
	//		rt.top += dy;
	//		if( rt.Height() < ROI_SIZEMIN ){
	//			rt.top = max(0, rt.bottom - ROI_SIZEMIN);
	//		}
	//		if(rt.top < 0)	rt.top = 0;
	//		break;
	//	case ROI_RESIZE_Y2:
	//		rt.bottom += dy;
	//		if( rt.Height() < ROI_SIZEMIN ){
	//			rt.bottom = min(m_imgOriginalImage.nHeight, rt.top + ROI_SIZEMIN);
	//		}
	//		if(rt.bottom > m_imgOriginalImage.nHeight)	rt.bottom = m_imgOriginalImage.nHeight;
	//		break;
	//	case ROI_RESIZE_XY1:
	//		rt.left += dx;
	//		rt.top += dy;
	//		if( rt.Width() < ROI_SIZEMIN ){
	//			rt.left = max(0, rt.right - ROI_SIZEMIN);
	//		}
	//		if( rt.Height() < ROI_SIZEMIN ){
	//			rt.top = max(0, rt.bottom - ROI_SIZEMIN);
	//		}
	//		if(rt.left < 0)	rt.left = 0;
	//		if(rt.top < 0)	rt.top = 0;
	//		break;
	//	case ROI_RESIZE_XY2:
	//		rt.right += dx;
	//		rt.top += dy;
	//		if( rt.Width() < ROI_SIZEMIN ){
	//			rt.right = min(m_imgOriginalImage.nWidth, rt.left + ROI_SIZEMIN);
	//		}
	//		if( rt.Height() < ROI_SIZEMIN ){
	//			rt.top = max(0, rt.bottom - ROI_SIZEMIN);
	//		}
	//		if(rt.right > m_imgOriginalImage.nWidth)	rt.right = m_imgOriginalImage.nWidth;
	//		if(rt.top < 0)	rt.top = 0;
	//		break;
	//	case ROI_RESIZE_XY3:
	//		rt.left += dx;
	//		rt.bottom += dy;
	//		if( rt.Width() < ROI_SIZEMIN ){
	//			rt.left = max(0, rt.right - ROI_SIZEMIN);
	//		}
	//		if( rt.Height() < ROI_SIZEMIN ){
	//			rt.bottom = min(m_imgOriginalImage.nHeight, rt.top + ROI_SIZEMIN);
	//		}
	//		if(rt.left < 0)	rt.left = 0;
	//		if(rt.bottom > m_imgOriginalImage.nHeight)	rt.bottom = m_imgOriginalImage.nHeight;
	//		break;
	//	case ROI_RESIZE_XY4:
	//		rt.right += dx;
	//		rt.bottom += dy;
	//		if( rt.Width() < ROI_SIZEMIN ){
	//			rt.right = min(m_imgOriginalImage.nWidth, rt.left + ROI_SIZEMIN);
	//		}
	//		if( rt.Height() < ROI_SIZEMIN ){
	//			rt.bottom = min(m_imgOriginalImage.nHeight, rt.top + ROI_SIZEMIN);
	//		}
	//		if(rt.right > m_imgOriginalImage.nWidth)		rt.right = m_imgOriginalImage.nWidth;
	//		if(rt.bottom > m_imgOriginalImage.nHeight)		rt.bottom = m_imgOriginalImage.nHeight;
	//		break;
	//}

	//m_ptStart = cp;
}
