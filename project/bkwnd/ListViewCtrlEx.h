#pragma once
#include <vector>
#include <map>

#define BACKGROUND_COLOR RGB(0xfb,0xfc,0xfd)

extern const UINT WMH_LISTEX_HOVERCHANGED; // start dragging
extern const UINT WMH_LISTEX_LINK;  // dragging
extern const UINT WMH_LISTEX_SELECTCHANGED;  // drop


enum E_SubItemType
{
	SUBITEM_TEXT,
	SUBITEM_LINK,
};

enum E_ListItemType
{
	LISTITEM_TITLE		= 1,
	LISTITEM_CHECKBOX	= 2,
	LISTITEM_RADIOBOX	= 4,
	LISTITEM_EXPANDABLE	= 8,
	LISTITEM_BOLD		= 16,
};

#define  TREEITEM_ROOT  NULL

enum E_ListTitleType
{
	LISTTITLE_DEFAULT = 0x1,

	LISTITEM_THIRD_LINE_TOP = 0x2,
	LISTITEM_THIRD_LINE_MID = 0x4,
	LISTITEM_THIRD_LINE_BTM = 0x8,

	//居中
	LISTTILTE_CENTER=0x10,
};

class CListViewCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListViewCtrlEx)
public:
	struct TreeItem;
	struct TListSubItem;
	typedef std::vector<TreeItem*> TTreeItemPtrs;
	typedef std::vector<TListSubItem> TListSubItems;
	struct TListSubItem
	{
		TListSubItem() : type(SUBITEM_TEXT)
		{
			nMarginLeft = 2;
			clr = RGB(0,0,0);
			rcOffset = CRect(0,0,0,0);
		}

		TListSubItem(LPCTSTR szText, CRect rc, E_SubItemType aType=SUBITEM_TEXT)
			: str(szText), type(aType), rcOffset(rc)
		{
		}

		TListSubItem(LPCTSTR szText, E_SubItemType aType=SUBITEM_TEXT)
			: str(szText), type(aType)
		{
			rcOffset = CRect(0,0,0,0);
			nMarginLeft = 2;
			clr = RGB(0,0,0);
		}
		E_SubItemType type;
		CString str;
		COLORREF clr;
		int	nMarginLeft;
		CRect	rcOffset;
		CString	strUrl;
	};

	struct TreeItem
	{
	public:
		TreeItem(DWORD dwFlags){
			this->dwFlags = dwFlags;

			dwFlags		=0;
			clrBg		=BACKGROUND_COLOR;
			clrBtmGapLine=RGB(234,233,225);
			nLeftmargin	=10;
			nTopMargin	=-1;
			bBold		=FALSE;
			nHeightAdd	=0;

			_bExpanded = FALSE;
			_parent_item = NULL;
			bChecked = false;
			bSomeDecentdentChecked = false;
		}
		~TreeItem()
		{
			for(TTreeItemPtrs::iterator it=_clapsed_items.begin(); it!=_clapsed_items.end(); ++it)
				delete *it;
			_clapsed_items.clear();
		}

		bool bChecked;
		bool bSomeDecentdentChecked;

		int			nLeftmargin;
		int			nTopMargin;
		BOOL		bBold;
		int			nHeightAdd;
		DWORD		dwFlags;
		COLORREF	clrBg;
		COLORREF	clrBtmGapLine;

		DWORD_PTR _itemData;
		TreeItem* _parent_item;
		TListSubItems subItems;
		BOOL _bExpanded;
		TTreeItemPtrs _clapsed_items;
	};

public:
	CListViewCtrlEx(void);
	~CListViewCtrlEx(void);
	CMFCHeaderCtrl m_ctlHeader;

protected:
	CFont m_fontLink, m_fontBold, m_fontTitle, m_fontDef;
	CBitmap m_BitmapCheck, m_BitmapExpand, m_BitmapRadio;
	CBitmap m_bitmapCheckMask, m_bitmapRadioMask;
	int m_nHoverItem;

	TreeItem* m_pRoot;
	HWND m_hWndObserver;
	CString m_strEmptyString;
public:
	BOOL BulidRoot() { 
		BOOL bRet = TRUE;
		if (m_pRoot)
		{
			delete m_pRoot;
			bRet = __super::DeleteAllItems();
		}
		m_pRoot = new TreeItem(LISTITEM_TITLE);
		m_pRoot->_bExpanded = TRUE; // 根节点必须展开
		return bRet;
	}
	void SetEmptyString(LPCTSTR szEmptyString);
	void SetObserverWindow(HWND hWnd);
	void SetItemHeight(UINT uHeight = 27);

	int GetItemDepth(const TreeItem* pItem){
		int iDepth = 0;
		TreeItem* pItemParent = pItem->_parent_item;
		while (pItemParent != m_pRoot)
		{
			pItemParent = pItemParent->_parent_item;
			iDepth++;
		}
		return iDepth;
	}
	int GetItemIndex(const TreeItem* pItem); // -1：没显示或不存在；>=0：列表中的位置
	BOOL GetCheckState(TreeItem* pItem);
	void __SetDescentdentCheckSate(TreeItem* pItem, bool bCheck);
	BOOL SetCheckState(TreeItem* pItem = TREEITEM_ROOT, bool bCheck = TRUE); // pItem == TREEITEM_ROOT 表示全部
	void CleanCheck();
	void CheckAll();
public:
	TreeItem* GetRootItem() {return m_pRoot;}
	bool IsItemExit(const TreeItem* pItem, TreeItem* pParentItem=TREEITEM_ROOT);
	TreeItem* InsertItemEx(_In_ LPCTSTR strItem, TreeItem* pParent=TREEITEM_ROOT, int nPos=-1 /*-1代表末段插入*/, DWORD dwFlags=0, COLORREF clr=0);
	int AppendSubItem(TreeItem* pItem, LPCTSTR strItem, E_SubItemType itemType=SUBITEM_TEXT);
	int AppendSubItem(TreeItem* pItem, LPCTSTR strItem, CRect rc, E_SubItemType itemType, COLORREF clr, LPCTSTR szURL); // 当Item为LISTITEM_TITLE时
	void ModifyItemFlags(TreeItem* pItem, DWORD dwFlagsAdd, DWORD dwFlagsRemove=NULL, bool bRedraw = TRUE);
	bool SetItem(TreeItem* pItem, COLORREF clrBg, BOOL bRedraw=TRUE);
	bool SetSubItem(TreeItem* pItem, int nSubItem, LPCTSTR lpszItem, E_SubItemType itemType=SUBITEM_TEXT, BOOL bRedraw=TRUE);
	bool SetSubItem(TreeItem* pItem, int nSubItem, COLORREF clr, BOOL bRedraw=TRUE);
	TListSubItem* GetSubItem(TreeItem* pItem, int nSubItem);
	bool GetSubItemText(TreeItem* pItem, int nSubItem, CString &str);
	void SetItemData(TreeItem* pItem, DWORD itemData) { ASSERT(pItem != NULL); pItem->_itemData = itemData;}
	DWORD GetItemData(TreeItem* pItem) {ASSERT(pItem != NULL); return pItem->_itemData;}
	TreeItem* GetItem(int nIndex) {
		ASSERT(nIndex>=0); 
		if (nIndex < GetItemCount())
		{
			return (TreeItem *)__super::GetItemData(nIndex); 
		}
		else
		{
			ASSERT(FALSE);
			return NULL;
		}
	}

	BOOL DeleteItem(TreeItem * pItem);
	BOOL DeleteAllItems();

	BOOL IsDescentdent(const TreeItem* pDescentdentItem, const TreeItem* pItem);
	void ExpandItem(TreeItem * pItem=TREEITEM_ROOT, BOOL expand=TRUE, BOOL bForceSubExpand = FALSE);
protected:
	void _ClapsedItem(TreeItem* pItem, int& nNext);
	void _ExpandItem(TreeItem* pItem, int& nNext, BOOL bForceSubExpand = FALSE);
protected:
	// single-selection only
	BOOL SelectItem(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ASSERT(GetStyle() & LVS_SINGLESEL);

		BOOL bRet = SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		if(bRet)
			bRet = EnsureVisible(nIndex, FALSE);
		return bRet;
	}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	afx_msg void OnHdnDividerdblclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnTrack(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);

public:
	void _SetColumnNeedWidth(int i, DWORD nWidth);
protected:
	void _DrawTitleItem( LPDRAWITEMSTRUCT lpdis, const TreeItem *pItem );
	void _DrawNormalItem( LPDRAWITEMSTRUCT lpdis, const TreeItem *pItem );
	BOOL _PtInSubItemCheckBox( const POINT& pt, int nItem );
	BOOL _GetSubItemLinkRect( int nItem, int nSubItem, LPCTSTR szText, CRect &rc );
	bool _PtInSubItemLink( const POINT &pt, int nItem, int & nSubItem );
	int _DrawCheckBox( CDC &dc, RECT &rcCheckBox, BOOL bChecked, DWORD dwFlags, BOOL bSomeDescentdentChecked=FALSE);
	RECT _GetRectCheckBox( RECT &rcItem, int nDepth=0, BOOL bHasMinus=FALSE);
	RECT _GetRectMinus( RECT &rcItem, int nDepth=0);
	void _InvalidateRect( int nItem);
	void _InvalidateRect( int nItem, int nSubItem );
	void _InvalidateRect(TreeItem* pItem, int nSubItem);
	void _InvalidateRect(TreeItem* pItem);
	virtual BOOL _FireEvent(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	void _RedrawTitle();
protected:
	CRBMap<int,DWORD>	m_columnMaxWidth;
	UINT m_uHeight;

public:
	UINT GetItemHeight() { return m_uHeight; }

public: 
	// 禁用的继承函数
// 	int GetItemCount() {ASSERT(FALSE); return 0;}
	int InsertItem(_In_ const LVITEM* pItem) { ASSERT(FALSE); return FALSE; }
	int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem) { ASSERT(FALSE); return FALSE; }
	int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage) { ASSERT(FALSE); return FALSE; }
	int InsertItem(_In_ UINT nMask, _In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ UINT nState,
		_In_ UINT nStateMask, _In_ int nImage, _In_ LPARAM lParam) { ASSERT(FALSE); return FALSE;}
	BOOL SetItemText(_In_ int nItem, _In_ int nSubItem, _In_z_ LPCTSTR lpszText) { ASSERT(FALSE); return FALSE;}
//	BOOL DeleteItem(int nItem) {ASSERT(FALSE); return FALSE; }	
};

// 列表列宽度自适应
inline void AdjustColumnWidth(CListCtrl& wndList, int nCol=-1, int iReserveMargin = 16)
{
	CHeaderCtrl/*CMFCHeaderCtrl*/* pHeaderCtrl = wndList.GetHeaderCtrl();
	if (pHeaderCtrl->GetSafeHwnd())
	{
		if (pHeaderCtrl->GetItemCount() <= nCol)
		{
			ASSERT(FALSE);
			return;
		}
		if (-1 == nCol)
		{
			nCol = pHeaderCtrl->GetItemCount()-1;
		}

		int iReserveWidth=0;
		for (int i=0; i<pHeaderCtrl->GetItemCount(); i++)
		{
			iReserveWidth += wndList.GetColumnWidth(i);
		}
		CRect rectClient;
		wndList.GetClientRect(rectClient);
		iReserveWidth = rectClient.Width()-iReserveWidth;
		//
		int nNewColWidth = wndList.GetColumnWidth(nCol) + iReserveWidth - iReserveMargin;
		if (nNewColWidth < 0)
		{
			nNewColWidth = 0;
		}
		wndList.SetColumnWidth(nCol, nNewColWidth);
	}
}
