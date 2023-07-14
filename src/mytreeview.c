#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "mytreeview.h"
#include "mylistview.h"
#include "main.h"

#define CX_BITMAP 			16   /* Breite eines Bitmaps */
#define CY_BITMAP 			16   /* Hoehe eines Bitmaps */



/* delete all elements from tree view */

void MyTreeViewClearAll( MYTREEVIEWPARAM *tv ) {

	TreeView_DeleteAllItems( tv->hwndTV );

}


// CreateMyTreeView - creates a tree view control.
// Returns the handle to the new control if successful,
//  or NULL otherwise.
// hwndParent - handle to the control's parent window.

#define MY_TREEVIEW_STYLE WS_VISIBLE|WS_CHILD | TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS /*| TVS_TRACKSELECT */

HWND MyTreeViewCreateWindow( MYTREEVIEWPARAM * param ) {
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

	MyTreeViewClearAll(param);

	return param->hwndTV;
}



// AddItemToMyTreeview - adds items to a tree view control.
// Returns the handle to the newly added item.
// hwndTV - handle to the tree view control.
// text - text of the item to add.
// level - level at which to add the item.



HTREEITEM MyTreeViewAddItem(MYTREEVIEWPARAM* tv, HTREEITEM parent, LPSTR text, LPARAM id, int icon_index ) {
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

LRESULT CALLBACK MyTreeViewHandleMessages(MYTREEVIEWPARAM * tv, UINT wMsg, WPARAM wParam, LPARAM lParam) {
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
					if( tv->cbdblclick != NULL ) {
						hti = TreeView_GetSelection(tv->hwndTV);
						memset( &item, 0, sizeof(item) );
						item.mask = TVIF_HANDLE;
						item.hItem = hti;
						TreeView_GetItem( tv->hwndTV, &item );

						if( hti != NULL ) {
							(tv->cbdblclick) (hti, item.lParam  );
						}
					}
					return 0;
					break;


				case NM_RCLICK:  // Rechts- Klick
					if( tv->cbrclick != NULL ) {
						hti = TreeView_GetDropHilight(tv->hwndTV);
						if( hti == NULL )
							hti = TreeView_GetSelection(tv->hwndTV);
						else
							TreeView_SelectItem(tv->hwndTV, hti);
						memset( &item, 0, sizeof(item) );
						item.mask = TVIF_HANDLE;
						item.hItem = hti;
						TreeView_GetItem( tv->hwndTV, &item );

						if( hti != NULL ) {
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



/* get all child of object and enters them in tree view */

static void TreeViewInsertChildObjects( HTREEITEM hti, PVIOBJECT *parent ) {
	PVIOBJECT *child;
	int imageindex;
	TVITEM tvitem;

	if( parent != NULL ) {

		ExpandPviObject(parent);

		// there are more information after expanding
		if( parent->type == POBJ_CPU ) {
			char tempstring[512];
			sprintf( tempstring, "%s %s %s", parent->ex.cpu.cputype, parent->ex.cpu.arversion, parent->descriptor );
			memset( &tvitem, 0, sizeof(tvitem) );
			tvitem.hItem = hti;
			tvitem.mask = TVIF_TEXT;
			tvitem.pszText = tempstring;
			TreeView_SetItem( mytreeviewparam.hwndTV, &tvitem );
		} else if( parent->type == POBJ_PVAR ) {
			char tempstring[512];
			if( parent->ex.pv.type == BR_STRUCT ) {
				if( parent->ex.pv.dimension > 1 ) { // arrays of structs
					sprintf( tempstring, "%s   :%s[%u]",parent->descriptor, parent->ex.pv.pdatatype, parent->ex.pv.dimension );
				} else {
					sprintf( tempstring, "%s   :%s",parent->descriptor, parent->ex.pv.pdatatype );
				}
				memset( &tvitem, 0, sizeof(tvitem) );
				tvitem.hItem = hti;
				tvitem.mask = TVIF_TEXT;
				tvitem.pszText = tempstring;
				TreeView_SetItem( mytreeviewparam.hwndTV, &tvitem );
			} else {
				if( parent->ex.pv.dimension > 1 ) { // arrays of basic data types
					sprintf( tempstring, "%s   :%s[%u]",parent->descriptor, parent->ex.pv.pdatatype, parent->ex.pv.dimension );
					memset( &tvitem, 0, sizeof(tvitem) );
					tvitem.hItem = hti;
					tvitem.mask = TVIF_TEXT;
					tvitem.pszText = tempstring;
					TreeView_SetItem( mytreeviewparam.hwndTV, &tvitem );
				}
			}
		}

		child = FindPviChildObject( parent, TRUE );
		while( child != NULL ) {
			char tempstring[512];

			switch( parent->type ) {
				case POBJ_PVAR:
					if( parent->ex.pv.type == BR_STRUCT && parent->ex.pv.dimension == 1) {
						strcpy( tempstring, child->descriptor + strlen(parent->descriptor) + 1);
					} else {
						strcpy( tempstring, child->descriptor );
					}
					break;

				case POBJ_DEVICE:
					sprintf( tempstring, "%s %s %s", child->ex.cpu.cputype, child->ex.cpu.arversion, child->descriptor );
					break;

				default:
					strcpy( tempstring, child->descriptor );
					break;

			}

			imageindex = ResourcesGetPviObjectImage( child );

			MyTreeViewAddItem( &mytreeviewparam, hti, tempstring, (LPARAM) child->name, imageindex );
			child = FindPviChildObject( parent, FALSE );
		} /* End while()*/
	}

}



/* an element in tree view was clicked */

void MyTreeViewItemSelected( HTREEITEM hti, LPARAM lParam ) {
	PVIOBJECT *parent;  // das angeklickte Objekte ist parent
	HCURSOR oldcursor;

	if( TreeView_GetChild( mytreeviewparam.hwndTV, hti ) == NULL ) { // noch nix eingelesen
		parent = FindPviObjectByName( (char*) lParam );
		if( parent != NULL ) {
			if( parent->error == 0 ) {
				oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
				TreeViewInsertChildObjects( hti, parent );
				SetCursor(oldcursor);
			}
		}
	}
}



void MyTreeViewBeginDragging( HTREEITEM hti, LPARAM lParam ) {
	HIMAGELIST himl;

	TreeView_Select( mytreeviewparam.hwndTV, hti, TVGN_DROPHILITE ); // nur wg. Optik

	himl = TreeView_CreateDragImage( mytreeviewparam.hwndTV, hti);
	ImageList_BeginDrag(himl, 0, 0, 0);
	// start drag effect
	ImageList_DragEnter(g_hwndMainWindow,0,0);
	SetCapture(g_hwndMainWindow);

	g_draggedPVIObject = FindPviObjectByName( (char*) lParam);
}



/* an element in tree view was double clicked */

void MyTreeViewDblClick(HTREEITEM hti, LPARAM lParam) {
	PVIOBJECT *object;
	object = FindPviObjectByName( (char*) lParam );
	if( object != NULL ) {
		object->watchsort = ListView_GetItemCount( mylistviewparam.hwndLV );
		MyListViewInsertPVIObjects( object);
	}
}


/* mouse right click on tree view */
void MyTreeViewRClick(HTREEITEM hti, LPARAM lParam) {
	PVIOBJECT *object;
	RECT rect_window;
	RECT rect_item;
	LPRECT lprect_item = &rect_item;

	if( GetWindowRect( mytreeviewparam.hwndTV, &rect_window ) ) {
		if( TreeView_GetItemRect( mytreeviewparam.hwndTV, hti, lprect_item, TRUE ) ) {

			object = FindPviObjectByName( (char*) lParam );
			if( object != NULL ) {
				switch( object->type ) {
					case POBJ_DEVICE:
						break;

					case POBJ_PVAR:
						switch( object->ex.pv.type ) {
							case BR_STRUCT:
								MyTreeViewItemSelected( hti, lParam );
								if( object->ex.pv.pdatatype != NULL ) {
								}
								break;

							default:
								break;
						}
						break;

					default:
						break;

				}
			}
		}
	}
}



