#include "mytreeview.h"
#include "main.h"

#define CX_BITMAP 			16   /* Breite eines Bitmaps */
#define CY_BITMAP 			16   /* Hoehe eines Bitmaps */



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: ClearTree
 Created  : Wed Feb  8 11:21:41 2006
 Modified : Wed Feb  8 11:21:41 2006

 Synopsys : loescht alle Elemente des TreeViews
 Input    : 
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void ClearMyTreeView( MYTREEVIEWPARAM *tv ){

	TreeView_DeleteAllItems( tv->hwndTV );

}


// CreateMyTreeView - creates a tree view control. 
// Returns the handle to the new control if successful,
//  or NULL otherwise. 
// hwndParent - handle to the control's parent window. 

#define MY_TREEVIEW_STYLE WS_VISIBLE|WS_CHILD | TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS /*| TVS_TRACKSELECT */

HWND CreateMyTreeView( MYTREEVIEWPARAM * param )
{
	//RECT rcClient;				// dimensions of client area 
    // HIMAGELIST himl;  // handle to image list
	// HICON hicon; 
	// int i;
HFONT hfont;

	/* Ensure that the common control DLL is loaded.  */
	InitCommonControls();

	//GetClientRect( param->hwndParent, &rcClient);

	param->hwndTV = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, "Select Object", MY_TREEVIEW_STYLE, 0, 0, 0, 0, param->hwndParent, NULL, param->hinstance, NULL);
	
	hfont = GetStockObject( DEFAULT_GUI_FONT );
	SendMessage( param->hwndTV, WM_SETFONT, (WPARAM) hfont, 1 );

	ClearMyTreeView(param);
	
	return param->hwndTV;
}



// AddItemToMyTreeview - adds items to a tree view control. 
// Returns the handle to the newly added item. 
// hwndTV - handle to the tree view control. 
// text - text of the item to add. 
// level - level at which to add the item. 



HTREEITEM AddItemToMyTreeView(MYTREEVIEWPARAM* tv, HTREEITEM parent, LPSTR text, LPARAM id, int icon_index )
{ 
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
   	HTREEITEM hti; 
 
	memset(&tvi, 0, sizeof(tvi) );

    tvi.mask = TVIF_TEXT | TVIF_IMAGE  | TVIF_SELECTEDIMAGE | TVIF_PARAM; 	
 
    // Set the text of the item. 
    tvi.pszText = text; 
    //tvi.cchTextMax = lstrlen(text); 

    tvi.iImage = icon_index; 
    tvi.iSelectedImage = icon_index; 
	tvi.cChildren = 1;

	// Eltern und Bruder setzen
	tvins.hParent = parent;
	tvins.hInsertAfter = TVI_LAST;

 
	// lParam setzen
    tvi.lParam = (LPARAM) id; 


    // Add the item to the tree view control.
	tvins.item = tvi;
	hti= TreeView_InsertItem( tv->hwndTV, &tvins );
    
 
    // The new item is a child item. Give the parent item a 
    // closed folder bitmap to indicate it now has child items. 
    // if (level > 1) { 
        // hti = TreeView_GetParent(hwndTV, hPrev); 
        // tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
        // tvi.hItem = hti; 
        // tvi.iImage = 0; 
        // tvi.iSelectedImage = 0; 
        // TreeView_SetItem(hwndTV, &tvi); 
    // } 
 
    return hti; 
} 

LRESULT CALLBACK HandleMyTreeViewMessages(MYTREEVIEWPARAM * tv, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMTREEVIEW pnmtv;
	HTREEITEM hti;
	TVITEM item;

	switch (wMsg) {

	case WM_NOTIFY:
		pnmtv = (LPNMTREEVIEW)lParam;

		if (pnmtv->hdr.hwndFrom != tv->hwndTV) {
			return 1;
		}

		switch (pnmtv->hdr.code) {
		case NM_DBLCLK:  // Doppel- Klick
			if( tv->cbdblclick != NULL ){
			    hti = TreeView_GetSelection(tv->hwndTV);
				memset( &item, 0, sizeof(item) );
				item.mask = TVIF_HANDLE;
				item.hItem = hti;
				TreeView_GetItem( tv->hwndTV, &item );

				if( hti != NULL ){
					(tv->cbdblclick) (hti, item.lParam  );
				}
			}
			return 0;
		break;


		case NM_RCLICK:  // Rechts- Klick
			if( tv->cbrclick != NULL ){
				hti = TreeView_GetDropHilight(tv->hwndTV);
				if( hti == NULL )
					hti = TreeView_GetSelection(tv->hwndTV);
				else
					TreeView_SelectItem(tv->hwndTV, hti);
				memset( &item, 0, sizeof(item) );
				item.mask = TVIF_HANDLE;
				item.hItem = hti;
				TreeView_GetItem( tv->hwndTV, &item );

				if( hti != NULL ){
					(tv->cbrclick) ( hti, item.lParam  );
				}
			}
			return 0;
		break;

		case TVN_SELCHANGED:	// Selection changed
			if (tv->cbselected != NULL) {
				(tv->cbselected) (pnmtv->itemNew.hItem, pnmtv->itemNew.lParam);
			}
			return 0;
		break;

		case TVN_BEGINDRAG:	// Begin Draggin
			if (tv->cbbegindrag != NULL) {
				(tv->cbbegindrag) (pnmtv->itemNew.hItem, pnmtv->itemNew.lParam);
			}
			return 0;
		}
		break;





	}
	return 1;
}
