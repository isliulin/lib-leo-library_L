#include "stdafx.h"
#include "ListViewCtrlEx.h"
#include <algorithm>

const UINT WMH_LISTEX_HOVERCHANGED = RegisterWindowMessage(_T("WMH_LISTEX_HOVERCHANGED")); // start dragging
const UINT WMH_LISTEX_LINK = RegisterWindowMessage(_T("WMH_LISTEX_LINK"));  // dragging
const UINT WMH_LISTEX_SELECTCHANGED = RegisterWindowMessage(_T("WMH_LISTEX_SELECTCHANGED"));  // drop

#define COLOR_LIST_LINK				RGB(40,100,165)	
#define LEFT_MARGIN_TEXT_COLUMN		3

enum _DEF_UNIT {
	_CHECKBOX_MARGIN_LEFT = 18,
	_CHECKBOX_WIDTH = 16,
	_CHECKBOX_HEIGHT = 16,
	_MINUS_MARGIN_LEFT = 3,
	_MINUS_WIDTH = 9,
	_MINUS_HEIGHT = 9,
	_DEPTH_LEFT_OFFSET = 13,
};


void CreateBitmapMask( CBitmap &pBmpSource, CBitmap &pBmpMask, COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY)
{
	BITMAP bm;

	// Get the dimensions of the source bitmap
	pBmpSource.GetBitmap(&bm);
	// Create the mask bitmap
	pBmpMask.CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// We will need two DCs to work with. One to hold the Image
	// (the source), and one to hold the mask (destination).
	// When blitting onto a monochrome bitmap from a color, pixels
	// in the source color bitmap that are equal to the background
	// color are blitted as white. All the remaining pixels are
	// blitted as black.

	CDC hdcSrc, hdcDst;

	hdcSrc.CreateCompatibleDC(NULL);
	hdcDst.CreateCompatibleDC(NULL);

	// Load the bitmaps into memory DC
	HGDIOBJ hbmSrcT = hdcSrc.SelectObject(pBmpSource);
	HGDIOBJ hbmDstT = hdcDst.SelectObject(pBmpMask);

	// Dynamically get the transparent color
	COLORREF clrTrans;
	if (clrpTransColor == NULL)
	{
		// User did not specify trans color so get it from bmp
		clrTrans = hdcSrc.GetPixel(iTransPixelX, iTransPixelY);
	}
	else
	{
		clrTrans = clrpTransColor;
	}

	// Change the background to trans color
	COLORREF clrSaveBk  = hdcSrc.SetBkColor(clrTrans);

	// This call sets up the mask bitmap.
	hdcDst.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &hdcSrc, 0, 0, SRCCOPY);

	// Now, we need to paint onto the original image, making
	// sure that the "transparent" area is set to black. What
	// we do is AND the monochrome image onto the color Image
	// first. When blitting from mono to color, the monochrome
	// pixel is first transformed as follows:
	// if  1 (black) it is mapped to the color set by SetTextColor().
	// if  0 (white) is is mapped to the color set by SetBkColor().
	// Only then is the raster operation performed.

	COLORREF clrSaveDstText = hdcSrc.SetTextColor(RGB(255,255,255));
	hdcSrc.SetBkColor(RGB(0,0,0));

	hdcSrc.BitBlt(0,0,bm.bmWidth, bm.bmHeight, &hdcDst,0,0,SRCAND);

	// Clean up by deselecting any objects, and delete the
	// DC's.
	hdcDst.SetTextColor(clrSaveDstText);

	hdcSrc.SetBkColor(clrSaveBk);
	hdcSrc.SelectObject(hbmSrcT);
	hdcDst.SelectObject(hbmDstT);

	hdcSrc.DeleteDC();
	hdcDst.DeleteDC();
}



CListViewCtrlEx::CListViewCtrlEx( void )
{
	m_hWndObserver = NULL;
	m_nHoverItem = -1;
	m_fontLink.Attach( ResFontPool::GetFont(FALSE,TRUE,FALSE,0) );
	m_fontBold.Attach( ResFontPool::GetFont(TRUE,FALSE,FALSE) );
	m_fontTitle.Attach( ResFontPool::GetFont(TRUE,FALSE,FALSE,4) );
 	m_fontDef.Attach( ResFontPool::GetFont(KUIF_DEFAULTFONT));
	m_BitmapCheck.Attach(ResBmpPool::GetBitmap(IDB_BMP_FOLER_CHECKFLAGS));
	m_BitmapExpand.Attach(ResBmpPool::GetBitmap(IDB_BMP_MINUS_PLUS));
	m_BitmapRadio.Attach(ResBmpPool::GetBitmap(IDB_BMP_LISTCTRL_RADIO));
	CreateBitmapMask(m_BitmapRadio, m_bitmapRadioMask, 0, 0, 0);
	CreateBitmapMask(m_BitmapCheck, m_bitmapCheckMask, 0, 0, 0);

	m_strEmptyString = _T("终端不存在！！！");
	m_uHeight = 26;
	m_pRoot = NULL;
	BulidRoot();
}

CListViewCtrlEx::~CListViewCtrlEx( void )
{
	m_fontLink.Detach();
	m_fontBold.Detach();
	m_fontTitle.Detach();
	m_fontDef.Detach();

	m_BitmapCheck.Detach();
	m_BitmapExpand.Detach();
	m_BitmapRadio.Detach();
	m_bitmapRadioMask.DeleteObject();
	m_bitmapCheckMask.DeleteObject();

	delete m_pRoot;
}

IMPLEMENT_DYNAMIC(CListViewCtrlEx, CListCtrl)

BEGIN_MESSAGE_MAP(CListViewCtrlEx, CListCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_NOTIFY_REFLECT(NM_CLICK, &CListViewCtrlEx::OnNMClick)
	ON_WM_MEASUREITEM_REFLECT()
	ON_NOTIFY(HDN_ENDTRACKA, 0, &CListViewCtrlEx::OnHdnEndtrack)
	ON_NOTIFY(HDN_ENDTRACKW, 0, &CListViewCtrlEx::OnHdnEndtrack)
	ON_NOTIFY(HDN_TRACKA, 0, &CListViewCtrlEx::OnHdnTrack)
	ON_NOTIFY(HDN_TRACKW, 0, &CListViewCtrlEx::OnHdnTrack)
	ON_NOTIFY(HDN_DIVIDERDBLCLICKA, 0, &CListViewCtrlEx::OnHdnDividerdblclick)
	ON_NOTIFY(HDN_DIVIDERDBLCLICKW, 0, &CListViewCtrlEx::OnHdnDividerdblclick)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

int CListViewCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

//  	this->ModifyStyle(NULL, LVS_SINGLESEL);
	this->SetExtendedStyle(LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT |this->GetExtendedStyle());

	m_ctlHeader.SubclassDlgItem(0, this);

	__super::SetBkColor(BACKGROUND_COLOR);
	return 0;
}

BOOL CListViewCtrlEx::GetCheckState(TreeItem* pItem)
{
// 	if (pItem != TREEITEM_ROOT)
// 	{
		return pItem->bChecked;
// 	}
// 	else
// 	{
// 		return m_pRoot->bChecked;
// 	}
}

inline void CListViewCtrlEx::__SetDescentdentCheckSate(TreeItem* pItem, bool bCheck)
{
	for (TTreeItemPtrs::size_type ix = 0; ix != pItem->_clapsed_items.size(); ix++)
	{
		pItem->_clapsed_items[ix]->bChecked = bCheck;
		pItem->_clapsed_items[ix]->bSomeDecentdentChecked = bCheck;
		if (pItem->_bExpanded)
		{
			_InvalidateRect(pItem->_clapsed_items[ix]);
		}

		if ((pItem->_clapsed_items[ix]->dwFlags & LISTITEM_TITLE) 
			&& pItem->_clapsed_items.size())
		{
			__SetDescentdentCheckSate(pItem->_clapsed_items[ix], bCheck);
		}
	}
}

BOOL CListViewCtrlEx::SetCheckState(TreeItem* pItem /* = TREEITEM_ROOT */, bool bCheck /* = TRUE */)
{
	BOOL bRet = FALSE;
	TreeItem* pItemRel = (pItem != TREEITEM_ROOT) ? pItem : m_pRoot;
	// 当前节点
	pItemRel->bChecked = bCheck;
	pItemRel->bSomeDecentdentChecked = bCheck;
	if (pItemRel != m_pRoot)
	{
		if (!IsItemExit(pItemRel))
		{
			ASSERT(FALSE);
			return bRet;
		}
		_InvalidateRect(pItemRel);
	}
	// 处理子孙节点
	__SetDescentdentCheckSate(pItemRel, bCheck);
	// 处理父亲节点
	TreeItem* pParent = pItemRel->_parent_item;
	bool bSameState = true;
	while(pParent != NULL)
	{
		if (bSameState)
		{
			for (TTreeItemPtrs::size_type ix=0; ix < pParent->_clapsed_items.size(); ix++)
			{
				if (pParent->_clapsed_items[ix]->bChecked != bCheck)
				{
					bSameState = false;
					pParent->bChecked = false;
					pParent->bSomeDecentdentChecked = true;
					break;
				}
			}

			if (bSameState)
			{
				pParent->bChecked = bCheck;
				pParent->bSomeDecentdentChecked = bCheck ? true:false;
			}
		}
		else
		{
			pParent->bChecked = false;
			pParent->bSomeDecentdentChecked = true;
		}

		_InvalidateRect(pParent);
		pParent = pParent->_parent_item;
	}

	_FireEvent(WMH_LISTEX_SELECTCHANGED, (WPARAM)pItem, bCheck?1:0);
	return bRet;
}

void CListViewCtrlEx::CleanCheck()
{
	SetCheckState(TREEITEM_ROOT, FALSE);
}

void CListViewCtrlEx::CheckAll()
{
	SetCheckState(TREEITEM_ROOT, TRUE);
}

void CListViewCtrlEx::SetObserverWindow( HWND hWnd )
{
	m_hWndObserver = hWnd;
}

inline int CListViewCtrlEx::GetItemIndex(const TreeItem* pItem)
{
	int nCount = __super::GetItemCount();
	DWORD itData = (DWORD)pItem;
	for (int i=0; i < nCount; i++)
	{
		if (__super::GetItemData(i) == itData)
		{
			return i;
		}
	}
	return -1;
}


bool CListViewCtrlEx::IsItemExit(const TreeItem* pItem, TreeItem* pParentItem)
{
	TreeItem* pParentItemRel = (pParentItem != TREEITEM_ROOT) ? pParentItem : m_pRoot;

	TTreeItemPtrs::iterator iter = std::find(pParentItemRel->_clapsed_items.begin(), pParentItemRel->_clapsed_items.end(), pItem);

	if (iter != pParentItemRel->_clapsed_items.end())
	{
		return true;
	}
	else
	{
		iter = pParentItemRel->_clapsed_items.begin();
		while (iter != pParentItemRel->_clapsed_items.end())
		{
			if ((*iter)->_clapsed_items.size())
			{
				if (IsItemExit(pItem, *iter))
				{
					return true;
				}
			}
			iter++;
		}
	}
	return false;
}

CListViewCtrlEx::TreeItem* CListViewCtrlEx::InsertItemEx(_In_ LPCTSTR strItem, TreeItem* pParent/*=TREEITEM_ROOT*/, int nPos/*=-1 代表末段插入*/, DWORD dwFlags/*=0*/, COLORREF clr/*=0*/)
{
	TreeItem* pParentRel =  (pParent!=TREEITEM_ROOT) ? pParent : m_pRoot;
	if (pParentRel != m_pRoot)
	{
		if (!IsItemExit(pParentRel))
		{
			ASSERT(FALSE);
			return NULL;
		}
	}
	// 
	TreeItem *pItem = new TreeItem(dwFlags);
	pItem->bBold = dwFlags & LISTITEM_BOLD;
	TListSubItem subItem(strItem, SUBITEM_TEXT);
	subItem.clr = clr;
	pItem->subItems.push_back(subItem);
	pItem->_parent_item = pParentRel;

	if (pItem->dwFlags & LISTITEM_TITLE)
	{
		pItem->clrBg	=RGB(248,248,248);
		if (pItem->dwFlags & LISTITEM_EXPANDABLE)
		{
			pItem->nLeftmargin += 13;
		}
		if (pItem->dwFlags & (LISTITEM_RADIOBOX|LISTITEM_CHECKBOX))
		{
			pItem->nLeftmargin += 13;
		}
	}

	// 按位置插入
	TTreeItemPtrs& ttiPtrs = pParentRel->_clapsed_items;
	TTreeItemPtrs::iterator iter = (nPos<0) ? ttiPtrs.end() : (ttiPtrs.begin()+nPos);
	if (iter > ttiPtrs.end())
	{
		iter = ttiPtrs.end();
	}

	if (pParentRel->_bExpanded)
	{
		int nItem = -1;
		if (iter != ttiPtrs.end())
		{
			nItem = GetItemIndex(*(iter));
		}
		else
		{
			if (pParentRel != m_pRoot)
			{
				if (ttiPtrs.empty())
				{
					nItem = GetItemIndex(pParentRel);
				}
				else
				{
					nItem = GetItemIndex(*(iter-1));
				}
				if (nItem >= 0)
				{
					nItem++;
				}
			} 
			else
			{
				nItem = __super::GetItemCount();
			}
		}

		if (nItem >= 0)
		{
			nItem = __super::InsertItem(nItem, _T(""));
			ASSERT(nItem >= 0);
			__super::SetItemData(nItem, (DWORD)pItem);
		}
	}
	ttiPtrs.insert(iter, pItem);
	return pItem;
}

int CListViewCtrlEx::AppendSubItem(TreeItem* pItem, LPCTSTR strItem, E_SubItemType itemType/*=SUBITEM_TEXT*/)
{
	ASSERT(pItem);
	pItem->subItems.push_back(TListSubItem(strItem, itemType));
	return pItem->subItems.size();
}

// 当Item为LISTITEM_TITLE时
int CListViewCtrlEx::AppendSubItem(TreeItem* pItem, LPCTSTR strItem, CRect rc, E_SubItemType itemType, COLORREF clr, LPCTSTR szURL)
{
	ASSERT(pItem);
	TListSubItem  isubitem(strItem, rc, itemType);
	if(szURL)
		isubitem.strUrl = szURL;
	isubitem.clr = clr;
	pItem->subItems.push_back(isubitem);
	return pItem->subItems.size();
}

bool CListViewCtrlEx::SetItem(TreeItem* pItem, COLORREF clrBg, BOOL bRedraw/*=TRUE*/)
{
	ASSERT(pItem);
	pItem->clrBg = clrBg;
	if (bRedraw)
		_InvalidateRect(pItem);
	return true;
}

bool CListViewCtrlEx::SetSubItem(TreeItem* pItem, int nSubItem, LPCTSTR lpszItem, E_SubItemType itemType/*=SUBITEM_TEXT*/, BOOL bRedraw/*=TRUE*/)
{
	ASSERT(pItem);
	ASSERT(nSubItem>=0);
	TListSubItems::size_type ix = nSubItem;
	if (ix>=0 && ix < pItem->subItems.size())
	{
		pItem->subItems[ix].type = itemType;
		pItem->subItems[ix].str = lpszItem;
		if (bRedraw)
			_InvalidateRect(pItem, nSubItem);
		return true;
	}
	ASSERT(0);
	return false;
}

bool CListViewCtrlEx::SetSubItem(TreeItem* pItem, int nSubItem, COLORREF clr, BOOL bRedraw/*=TRUE*/)
{
	ASSERT(pItem);
	ASSERT(nSubItem>=0);
	TListSubItems::size_type ix = nSubItem;
	if (ix>=0 && ix < pItem->subItems.size())
	{
		pItem->subItems[ix].clr = clr;
		if (bRedraw)
			_InvalidateRect(pItem, nSubItem);
		return true;
	}
	ASSERT(0);
	return false;
}

CListViewCtrlEx::TListSubItem* CListViewCtrlEx::GetSubItem(TreeItem* pItem, int nSubItem)
{
	ASSERT(pItem);
	ASSERT(nSubItem>=0);
	TListSubItems::size_type ix = nSubItem;
	if (ix>=0 && ix < pItem->subItems.size())
	{
		return &(pItem->subItems[ix]);
	}
	ASSERT(0);
	return NULL;
}

bool CListViewCtrlEx::GetSubItemText(TreeItem* pItem, int nSubItem, CString &str)
{
	ASSERT(pItem);
	ASSERT(nSubItem>=0);
	TListSubItems::size_type ix = nSubItem;
	if (ix>=0 && ix < pItem->subItems.size())
	{
		str = pItem->subItems[ix].str;
		return true;
	}
	ASSERT(0);
	return false;
}

inline void CListViewCtrlEx::_InvalidateRect( int nItem)
{
	if (IsItemVisible(nItem))
	{
		CRect rcItem;
		__super::GetItemRect(nItem, &rcItem, LVIR_BOUNDS);
		InvalidateRect( rcItem );
	}
}

inline void CListViewCtrlEx::_InvalidateRect( int nItem, int nSubItem )
{
	if (IsItemVisible(nItem))
	{
		CRect rcItem;
		//__super::GetItemRect(nItem, &rcItem, LVIR_BOUNDS);
		__super::GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rcItem);
		InvalidateRect( rcItem );
	}
}

inline void CListViewCtrlEx::_InvalidateRect(TreeItem* pItem, int nSubItem)
{
	int index = GetItemIndex(pItem);
	if (index>=0)
	{
		_InvalidateRect(index);
	}
}

inline void CListViewCtrlEx::_InvalidateRect(TreeItem* pItem)
{
	int index = GetItemIndex(pItem);
	if (index>=0)
	{
		_InvalidateRect(index);
	}
}


void CListViewCtrlEx::ModifyItemFlags(TreeItem* pItem, DWORD dwFlagsAdd, DWORD dwFlagsRemove/* =NULL */, bool bRedraw /* = TRUE */)
{
	ASSERT(pItem);
	pItem->dwFlags |= dwFlagsAdd;
	pItem->dwFlags &= ~dwFlagsRemove;
	if (bRedraw)
		_InvalidateRect(pItem, 0);
}

BOOL CListViewCtrlEx::DeleteItem(TreeItem * pItem)
{
	ASSERT(pItem != NULL);
	if (IsItemExit(pItem))
	{
		if (pItem->_bExpanded)
		{
			ExpandItem(pItem, FALSE);
		}
		int index = GetItemIndex(pItem);
		if (index>=0)
		{
			__super::DeleteItem(index);
		}
		delete pItem;
	}
	else
	{
		ASSERT(FALSE);
	}
	return FALSE;
}

BOOL CListViewCtrlEx::DeleteAllItems()
{
	return BulidRoot();
}

void CListViewCtrlEx::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	TRACE1("CListViewCtrlEx::OnNMClick:%3d\n", pNMItemActivate->iItem);
	*pResult = 0;

	if (pNMItemActivate->iItem < 0)
	{
		return;
	}

	TreeItem *pItem =  GetItem(pNMItemActivate->iItem);
	if(pItem)
	{
		// - link check 
		int iSubItem = pNMItemActivate->iSubItem;
		if(_PtInSubItemLink(pNMItemActivate->ptAction, pNMItemActivate->iItem, iSubItem) )
		{
			_FireEvent(WMH_LISTEX_LINK, pNMItemActivate->iItem, iSubItem);
		}
		else if( pItem->dwFlags & (LISTITEM_CHECKBOX|LISTITEM_RADIOBOX) 
			&& _PtInSubItemCheckBox(pNMItemActivate->ptAction, pNMItemActivate->iItem))
		{
			if (GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT))
			{
				bool bCheck = !pItem->bChecked;
				int iSelected = -1;
				while ((iSelected = GetNextItem(iSelected, LVNI_SELECTED))>=0)
				{
					TreeItem *pItem = GetItem(iSelected);
					ASSERT(pItem!=NULL);
					SetCheckState(pItem, bCheck);
				}
			}
			else
			{
				SetCheckState(pItem, !pItem->bChecked);
			}
		}
		else if(pItem->dwFlags & LISTITEM_TITLE)
		{
			if(pItem->dwFlags & LISTITEM_EXPANDABLE)
			{
				INT iItem = pNMItemActivate->iItem;
				RECT rcItem = {0};
				if( GetItemRect(iItem, &rcItem, LVIR_BOUNDS) )
				{
					ExpandItem(pItem, !pItem->_bExpanded);
				}	
			}
		}
	}
}

void CListViewCtrlEx::OnPaint()
{
	if(!m_pRoot->_clapsed_items.empty())
	{
		__super::OnPaint();
	}
	else
	{
		CPaintDC dc(this);
		RECT rc = {0};
		GetClientRect( &rc );
		//dc.FillSolidRect(&rc, RGB(0,0,255));
		rc.top += 50;
		HGDIOBJ hOldFont = dc.SelectObject(m_fontDef);
		dc.DrawText(m_strEmptyString, -1, &rc, DT_SINGLELINE|DT_LEFT|DT_CENTER);
		dc.SelectObject( hOldFont );
	}
}

void CListViewCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect	rcItem;

	LVHITTESTINFO hti;
	hti.pt = point;
	SubItemHitTest( &hti );
	if( m_nHoverItem!=hti.iItem )
	{
		m_nHoverItem = hti.iItem;
		_FireEvent(WMH_LISTEX_HOVERCHANGED, m_nHoverItem);
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

BOOL CListViewCtrlEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint pt;
	GetCursorPos( &pt );
	ScreenToClient( &pt );

	LVHITTESTINFO hti;
	hti.pt = pt;
	SubItemHitTest( &hti );
	bool bInLink = _PtInSubItemLink(pt, hti.iItem, hti.iSubItem);
	::SetCursor(::LoadCursor(NULL, bInLink?IDC_HAND:IDC_ARROW));
	if (bInLink)
	{
		return TRUE;
	}
	return CListCtrl::OnSetCursor(pWnd, nHitTest, message);
}

void CListViewCtrlEx::DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int nItem = lpDrawItemStruct->itemID;
	const TreeItem *pItem = GetItem( nItem );
	if(!pItem)
		return;
	if(pItem->dwFlags & LISTITEM_TITLE)
	{
		_DrawTitleItem(lpDrawItemStruct, pItem);
	}
	else
	{
		_DrawNormalItem(lpDrawItemStruct, pItem);
	}
	return;
}

void CListViewCtrlEx::SetEmptyString( LPCTSTR szEmptyString )
{
	if(szEmptyString)
		m_strEmptyString = szEmptyString;
	else
		m_strEmptyString = _T("");
}

void CListViewCtrlEx::_DrawTitleItem( LPDRAWITEMSTRUCT lpdis, const TreeItem *pItem )
{
	ATLASSERT(pItem);

	if(pItem->subItems.empty())
		return ;

	int	nWinWidth=lpdis->rcItem.right-lpdis->rcItem.left;
	CRect	rcWindows;
	GetWindowRect(rcWindows);
	if ( nWinWidth > rcWindows.Width())
		nWinWidth = rcWindows.Width()-20;

	int nItem = lpdis->itemID;

	CDC dc;
	dc.Attach(lpdis->hDC);

	BOOL bSelect = FALSE ;
	if ((lpdis->itemAction | ODA_SELECT) &&
		(lpdis->itemState & ODS_SELECTED))
	{
		bSelect = TRUE ;
	}

	if ( bSelect )
		dc.FillSolidRect( &lpdis->rcItem, RGB(209,235,250));
	else
		dc.FillSolidRect( &lpdis->rcItem, pItem->clrBg);

	HGDIOBJ	hOldFont = dc.SelectObject(m_fontDef);
	COLORREF clrOld = dc.GetTextColor();
	COLORREF clrDef = clrOld;
	int nDepth = GetItemDepth(pItem);

	// 
	RECT rcItem = lpdis->rcItem;
	if( pItem->dwFlags&LISTITEM_EXPANDABLE )
	{
		//3 + 9 + 3
		if(rcItem.left>-12)
		{
			CDC	dcTmp;
			dcTmp.CreateCompatibleDC(&dc);
			HGDIOBJ hBmpOld	= dcTmp.SelectObject(m_BitmapExpand);				
			RECT rcMinus = _GetRectMinus(rcItem, nDepth);
			dc.BitBlt( rcMinus.left, rcMinus.top, 9, 9, &dcTmp, pItem->_bExpanded? 0:_MINUS_WIDTH, 0, SRCCOPY);
			dcTmp.SelectObject(hBmpOld);
		}
	}

	// draw check box 
	if( pItem->dwFlags&(LISTITEM_CHECKBOX|LISTITEM_RADIOBOX) )
		_DrawCheckBox(dc, _GetRectCheckBox(lpdis->rcItem, nDepth, TRUE), pItem->bChecked, pItem->dwFlags, pItem->bSomeDecentdentChecked);


	for (TListSubItems::size_type ix = 0; ix < pItem->subItems.size(); ix++)
	{
		bool	bVCenter=TRUE;
		const TListSubItem& subItem = pItem->subItems[ix];
		CRect rcItem = subItem.rcOffset;
		if ( ix == 0 )
		{
			rcItem = lpdis->rcItem;

			if(pItem->nTopMargin>=0)
			{
				rcItem.top += pItem->nTopMargin;
				rcItem.bottom -= 0;
				bVCenter=FALSE;
			}
			else
			{
				rcItem.top += 2;
				rcItem.bottom -= 2;
			}
			rcItem.left+= pItem->nLeftmargin;

			// 
			rcItem.left += nDepth*_DEPTH_LEFT_OFFSET;
		}
		else
		{
			if ( rcItem.left < 0 )
			{
				rcItem.left = nWinWidth+rcItem.left;
			}
			if (rcItem.right < 0)
			{
				rcItem.right = nWinWidth+rcItem.right;
			}
			rcItem.OffsetRect( lpdis->rcItem.left, lpdis->rcItem.top);
		}

		if ( subItem.type == SUBITEM_TEXT )
		{
			dc.SetTextColor( subItem.clr);
			dc.SelectObject(m_fontDef);
		}
		else if ( subItem.type == SUBITEM_LINK )
		{
			dc.SelectObject(m_fontLink);
			dc.SetTextColor(COLOR_LIST_LINK);
		}

		CString strTitle = subItem.str;	
		DWORD	nFlag=DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_END_ELLIPSIS;
		if(bVCenter)
			nFlag|=DT_VCENTER;

		if (ix==0&&pItem->bBold||pItem->nHeightAdd!=0)
		{
			HGDIOBJ	fntOld=dc.SelectObject(ResFontPool::GetFont(pItem->bBold,FALSE,FALSE,pItem->nHeightAdd));
			dc.DrawText( strTitle, -1, &rcItem, nFlag);
			dc.SelectObject(fntOld);
		}
		else
			dc.DrawText( strTitle, -1, &rcItem, nFlag);
	}

	CPen	pex;
	pex.CreatePen(PS_SOLID,1,pItem->clrBtmGapLine);
	HGDIOBJ	penOld = dc.SelectObject(pex);
	dc.MoveTo( lpdis->rcItem.left, lpdis->rcItem.bottom-1 );
	dc.LineTo( lpdis->rcItem.right, lpdis->rcItem.bottom-1 );

	dc.SetTextColor(clrOld);
	dc.SelectObject(penOld);
	dc.SelectObject(hOldFont);
	dc.Detach();
}

void CListViewCtrlEx::_DrawNormalItem( LPDRAWITEMSTRUCT lpdis, const TreeItem *pItem )
{
	ASSERT(pItem);
	int nItem = lpdis->itemID;

	CDC dc;
	dc.Attach(lpdis->hDC);

	HGDIOBJ	hOldFont = dc.SelectObject(m_fontDef);

	BOOL bSelect = FALSE ;
	if ((lpdis->itemAction | ODA_SELECT) &&
		(lpdis->itemState & ODS_SELECTED))
	{
		bSelect = TRUE ;
	}

	if ( bSelect )
		dc.FillSolidRect( &lpdis->rcItem, RGB(185,219,255));
	else
		dc.FillSolidRect( &lpdis->rcItem, pItem->clrBg);

	int nDepth = GetItemDepth(pItem);

	// draw check box 
	if( pItem->dwFlags&(LISTITEM_CHECKBOX|LISTITEM_RADIOBOX) )
		_DrawCheckBox(dc, _GetRectCheckBox(lpdis->rcItem, nDepth, FALSE), pItem->bChecked, pItem->dwFlags);

	COLORREF oldClr = dc.GetTextColor();
	for(TListSubItems::size_type ix=0; ix<pItem->subItems.size(); ++ix)
	{
		CRect	rcSubItem;
		DWORD	nMarginWidth = 0;
		CRect	rcBounds;
		GetSubItemRect(nItem, ix, LVIR_LABEL, rcSubItem);
		if (rcSubItem.Width() == 0)
		{
			continue;
		}

		nMarginWidth = LEFT_MARGIN_TEXT_COLUMN+3;

		if(ix==0)
		{
			if( pItem->dwFlags&(LISTITEM_CHECKBOX|LISTITEM_RADIOBOX) )
			{
				nMarginWidth+=rcSubItem.left;
				rcSubItem.left += _CHECKBOX_MARGIN_LEFT - 6;
			}
			else
			{
				rcSubItem.left -= 19;
				nMarginWidth+=5;
			}
			rcSubItem.left += nDepth*_DEPTH_LEFT_OFFSET;
			nMarginWidth += nDepth*_DEPTH_LEFT_OFFSET;
		}

#define DT_FLAGS_DRAW_TEXT		(DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_END_ELLIPSIS|DT_VCENTER)

		rcSubItem.left += LEFT_MARGIN_TEXT_COLUMN;
		rcSubItem.right -= 3;
		const TListSubItem &subItem = pItem->subItems[ix];
		if(subItem.type==SUBITEM_LINK)
		{
			dc.SelectObject(m_fontLink);
			dc.SetTextColor(COLOR_LIST_LINK);

			CRect	rcProbeItem;
			dc.DrawText( subItem.str, -1, &rcProbeItem, DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_VCENTER|DT_CALCRECT);
			dc.DrawText( subItem.str, -1, &rcSubItem, DT_FLAGS_DRAW_TEXT);

			DWORD nMaxWidth = rcProbeItem.Width()+nMarginWidth;
			_SetColumnNeedWidth(ix,nMaxWidth);
		}
		else
		{
			dc.SetTextColor( subItem.clr );
			BOOL bFontBold = (pItem->bBold||pItem->nHeightAdd!=0);
			HGDIOBJ	fntOld;
			if (bFontBold)
			{
				fntOld=dc.SelectObject(ResFontPool::GetFont(pItem->bBold,FALSE,FALSE,pItem->nHeightAdd));
			}

			dc.DrawText( subItem.str, -1, &rcSubItem, DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_END_ELLIPSIS|DT_VCENTER);

			CRect	rcProbeItem;
			dc.DrawText( subItem.str, -1, &rcProbeItem, DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_VCENTER|DT_CALCRECT);
			DWORD nMaxWidth = rcProbeItem.Width()+nMarginWidth;
			_SetColumnNeedWidth(ix,nMaxWidth);

			if (bFontBold)
			{
				dc.SelectObject(fntOld);
			}
		}
	}

	CPen	penx;
	penx.CreatePen(PS_SOLID,1,pItem->clrBtmGapLine);
	HGDIOBJ	penOld = dc.SelectObject(penx);
	dc.MoveTo( lpdis->rcItem.left, lpdis->rcItem.bottom-1 );
	dc.LineTo( lpdis->rcItem.right, lpdis->rcItem.bottom-1);
	dc.SelectObject(penOld);

	dc.SelectObject(hOldFont);
	dc.SetTextColor(oldClr);	
	dc.Detach();
}

BOOL CListViewCtrlEx::_PtInSubItemCheckBox( const POINT& pt, int nItem )
{
	RECT rcItem = {0};
	if( nItem>=0 && nItem<__super::GetItemCount() && GetItemRect(nItem, &rcItem, LVIR_BOUNDS) )
	{
		TreeItem *pItem = GetItem(nItem);
		ATLASSERT(pItem);
		// 
		if( pItem->dwFlags&(LISTITEM_CHECKBOX|LISTITEM_RADIOBOX) )
		{
			RECT rcCheckBox = _GetRectCheckBox( rcItem, GetItemDepth(pItem), pItem->dwFlags & LISTITEM_TITLE);
			return PtInRect(&rcCheckBox, pt);
		}
	}
	return FALSE;
}

BOOL CListViewCtrlEx::_GetSubItemLinkRect( int nItem, int nSubItem, LPCTSTR szText, CRect &rc )
{
	__super::GetSubItemRect( nItem, nSubItem, LVIR_LABEL, rc );
	CDC* pDC = GetWindowDC();
	if(!pDC)
		return FALSE;

	rc.left += LEFT_MARGIN_TEXT_COLUMN;
	HGDIOBJ hOldFont = pDC->SelectObject(m_fontLink);
	pDC->DrawText( szText, -1, &rc, DT_CALCRECT|DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_END_ELLIPSIS|DT_VCENTER );
	pDC->SelectObject(hOldFont);
	return TRUE;
}

bool CListViewCtrlEx::_PtInSubItemLink( const POINT &pt, int nItem, int& nSubItem )
{
	if( nItem>=0 && nItem<__super::GetItemCount() )
	{
		TreeItem *pItem = GetItem(nItem);
		ATLASSERT(pItem);

		if ( pItem->dwFlags & LISTITEM_TITLE )
		{
			for (TListSubItems::size_type ix = 0; ix < pItem->subItems.size(); ix++)
			{
				const TListSubItem&	subItem = pItem->subItems[ix];
				if ( subItem.type == SUBITEM_LINK)
				{
					CRect	rcItem;
					CRect	rcLink = subItem.rcOffset;
					GetItemRect(nItem,rcItem, LVIR_BOUNDS);
					rcLink.OffsetRect(rcItem.left, rcItem.top);
					if ( rcLink.PtInRect(pt) )
					{
						nSubItem = ix;
						return true;
					}
				}
			}
		}
		else
		{
			TListSubItems::size_type ixSubItem = nSubItem;
			if (ixSubItem >=0 && ixSubItem<pItem->subItems.size())
			{
				const TListSubItem &subItem = pItem->subItems[nSubItem];
				if( subItem.type==SUBITEM_LINK )
				{
					// Check if in LINK Rect 
					CRect rcLink ;
					return _GetSubItemLinkRect(nItem, nSubItem, subItem.str, rcLink) && PtInRect(&rcLink, pt);
				}
			}
		}
	}
	return false;
}
int CListViewCtrlEx::_DrawCheckBox(CDC &dc, RECT &rcCheckBox, BOOL bChecked, DWORD dwFlags, BOOL bSomeDescentdentChecked/* =FALSE */)
{
	CDC	dcTmp;
	dcTmp.CreateCompatibleDC(&dc);

	HGDIOBJ hBmpOld = NULL;
	int x=rcCheckBox.left, y=rcCheckBox.top, nWidth=_CHECKBOX_WIDTH, nHeight=_CHECKBOX_HEIGHT, xSrc=bChecked ? _CHECKBOX_WIDTH*3 : _CHECKBOX_WIDTH, ySrc=0;

	if(dwFlags&LISTITEM_CHECKBOX)
	{
		if (dwFlags&LISTITEM_TITLE)
		{
			if (!bChecked && bSomeDescentdentChecked)
			{
				xSrc = _CHECKBOX_WIDTH*2;
			}
		}
		hBmpOld	= dcTmp.SelectObject(m_bitmapCheckMask);
		dc.BitBlt(x, y, nWidth, nHeight, &dcTmp, xSrc, ySrc, SRCAND);
		dcTmp.SelectObject(m_BitmapCheck);
		dc.BitBlt(x, y, nWidth, nHeight, &dcTmp, xSrc, ySrc, SRCPAINT);
		//dc.TransparentBlt(x, y, nWidth, nHeight, &dcTmp, xSrc, ySrc,nWidth, nHeight, RGB(0,0,0));
	}
	else
	{
		hBmpOld	= dcTmp.SelectObject(m_bitmapRadioMask);
		dc.BitBlt(x, y, nWidth, nHeight, &dcTmp, xSrc, ySrc, SRCAND);
		dcTmp.SelectObject(m_BitmapRadio);
		dc.BitBlt(x, y, nWidth, nHeight, &dcTmp, xSrc, ySrc, SRCPAINT);
	}

	dcTmp.SelectObject(hBmpOld);
	dcTmp.DeleteDC();

	return 3 + 13 + 3;
}
RECT CListViewCtrlEx::_GetRectCheckBox(RECT &rcItem, int nDepth/* =0 */, BOOL bHasMinus/* =FALSE */)
{
	int nTop = rcItem.top + (rcItem.bottom - rcItem.top-_CHECKBOX_HEIGHT)/2;
	int nLeft = rcItem.left+_CHECKBOX_MARGIN_LEFT+ nDepth*_DEPTH_LEFT_OFFSET;
	RECT rcCheckBox = {nLeft, nTop, nLeft+_CHECKBOX_WIDTH, nTop+_CHECKBOX_HEIGHT};
	return rcCheckBox;
}

RECT CListViewCtrlEx::_GetRectMinus(RECT &rcItem, int nDepth/* =0 */)
{
	int nTop = rcItem.top + (rcItem.bottom - rcItem.top-_MINUS_HEIGHT)/2;
 	int nLeft = rcItem.left+_MINUS_MARGIN_LEFT+ nDepth*_DEPTH_LEFT_OFFSET;
	RECT rcMinus = {nLeft, nTop, nLeft+_MINUS_WIDTH, nTop+_MINUS_WIDTH};	
	return rcMinus;
}

BOOL CListViewCtrlEx::_FireEvent( UINT message, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/ )
{
	if(m_hWndObserver)
		return ::PostMessage(m_hWndObserver, message, wParam, lParam);
	else
		return GetParent()->PostMessage(message, wParam, lParam);
}

void CListViewCtrlEx::_SetColumnNeedWidth( int i, DWORD nWidth )
{
	if ((int)nWidth<=0)
	{
		return;
	}

	DWORD	nOrgWidth = 0;
	if ( m_columnMaxWidth.Lookup(i, nOrgWidth) )
	{
		if ( nOrgWidth < nWidth )
			m_columnMaxWidth.SetAt(i,nWidth);
	}
	else
		m_columnMaxWidth.SetAt(i,nWidth);
}

void CListViewCtrlEx::OnHdnTrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here

	//*pResult = TRUE;
}

void CListViewCtrlEx::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	InvalidateRect(NULL);
	*pResult = 0;
}

void CListViewCtrlEx::OnHdnDividerdblclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	DWORD		nWidth	= 0;
	INT col = phdr->iItem;
	if ( m_columnMaxWidth.Lookup(col,nWidth) )
	{
		SetColumnWidth(phdr->iItem,nWidth);
		InvalidateRect(NULL);
		*pResult = TRUE;
	}
}

void CListViewCtrlEx::_RedrawTitle()
{
	for ( int i = 0; i < __super::GetItemCount(); i++)
	{	
		TreeItem*	pItem  = GetItem(i);

		if ( pItem && pItem->dwFlags&LISTITEM_TITLE)
		{
			__super::RedrawItems(i,i);
		}
	}
}

inline BOOL CListViewCtrlEx::IsDescentdent(const TreeItem* pDescentdentItem, const TreeItem* pItem)
{
	TreeItem* pParent = pDescentdentItem->_parent_item;
	while (pParent != m_pRoot)
	{
		if (pItem == pParent)
		{
			return TRUE;
		}
		pParent = pParent->_parent_item;
	}
	return FALSE;
}

inline void CListViewCtrlEx::_ClapsedItem(TreeItem* pItem, int& nNext)
{
	for (int index=__super::GetItemCount()-1; index >= nNext; index--)
	{
		if (IsDescentdent((TreeItem*)GetItem(index), pItem))
		{
 			for (;  index >= nNext; index--)
 			{
 				__super::DeleteItem(index);
 			}
			break;
		}
	}
	pItem->_bExpanded = false;
}

inline void CListViewCtrlEx::_ExpandItem(TreeItem* pItem, int& nNext, BOOL bForceSubExpand/* = FALSE*/)
{
	for (TTreeItemPtrs::size_type ix = 0; ix != pItem->_clapsed_items.size(); ix++)
	{
		if (!pItem->_bExpanded)
		{
			__super::InsertItem(nNext, _T(""));
			__super::SetItemData(nNext, (DWORD)pItem->_clapsed_items[ix]);
		}
		++nNext;

		if (pItem->_clapsed_items[ix]->dwFlags & LISTITEM_TITLE
			&& pItem->_clapsed_items.size())
		{
			if (bForceSubExpand || pItem->_clapsed_items[ix]->_bExpanded)
			{
				_ExpandItem(pItem->_clapsed_items[ix], nNext, bForceSubExpand);
			}
		}
	}
	pItem->_bExpanded = true;
}

inline void CListViewCtrlEx::ExpandItem(TreeItem * pItem/* =TREEITEM_ROOT */, BOOL expand/* =TRUE */, BOOL bForceSubExpand /* = FALSE */)
{
	TreeItem * pItemRel = (pItem != TREEITEM_ROOT) ? pItem : m_pRoot;

	if (!(pItemRel->dwFlags & LISTITEM_TITLE))
	{
		return;
	}
	if (expand == pItemRel->_bExpanded)
	{
		// 前后无展开变化
		if (!bForceSubExpand)
		{
			return; 
		}
	}

	int iItem = GetItemIndex(pItemRel);
	if (iItem < 0)
	{
		ASSERT(FALSE);
		return;
	}

	int nNext = iItem + 1;

	RECT rcItem = {0};
	if( !GetItemRect(iItem, &rcItem, LVIR_BOUNDS) )
	{
		ASSERT(FALSE);
		return;
	}
	__super::SetRedraw(FALSE);
	if(expand)
	{
		_ExpandItem(pItemRel, nNext, bForceSubExpand);
	}
	else
	{
		_ClapsedItem(pItemRel, nNext);
		__super::Update(0);
	}
	__super::RedrawItems(iItem, iItem);
	__super::SetRedraw(TRUE);
 	//InvalidateRect(&rcItem);
}


//设置行高，如果给定的值,m_uHeight=0认为是一个无效的值，不会设置生效
void CListViewCtrlEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (0 == m_uHeight)
		return;

	lpMeasureItemStruct->itemHeight = m_uHeight;
}

void CListViewCtrlEx::SetItemHeight(UINT uHeight/* = 30*/)
{
	m_uHeight = uHeight;
}

void CListViewCtrlEx::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	if (m_ctlHeader.GetSafeHwnd() != NULL)
	{
		m_ctlHeader.RedrawWindow();
	}
	
	// 此处不能使用GetColumnWidth， 否则会出现删除item后 头部留白问题
}

void CListViewCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int iSelected = -1;
	if ((iSelected = GetNextItem(iSelected, LVNI_SELECTED))>=0)
	{
		TreeItem *pItem = GetItem(iSelected);
		ASSERT(pItem!=NULL);
		if (pItem->dwFlags & LISTITEM_TITLE)
		{
			switch (nChar)
			{
			case VK_ADD:
				ExpandItem(pItem, TRUE);
				break;
			case VK_SUBTRACT:
				ExpandItem(pItem, FALSE);
				break;
			case VK_RIGHT:
				if (!pItem->_bExpanded)
					ExpandItem(pItem, TRUE);
				else
					SelectItem(iSelected+1);
				break;
			case VK_LEFT:
				if (pItem->_bExpanded)
				{
					ExpandItem(pItem, FALSE);
				} 
				else
				{
					if (pItem->_parent_item != m_pRoot && !pItem->_bExpanded)
					{
						SelectItem(GetItemIndex(pItem->_parent_item));
					} 
				}
				break;
			}
		}
		else if (VK_LEFT == nChar)
		{
			if (pItem->_parent_item != m_pRoot)
			{
				int nIndex = GetItemIndex(pItem->_parent_item);
				ASSERT(nIndex >= 0);
				SelectItem(nIndex);
			}
		}
	}

	if ((nChar != VK_RIGHT) && (nChar != VK_LEFT))
		CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
